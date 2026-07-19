#include <aether.h>

volatile u32 system_ticks_g = 0;
volatile u32 last_tick = 0;

void tick_hook(void) 
{
   if (get_system_ticks() - last_tick < TICK_HOOK_MS){
      return;
   }

   last_tick = get_system_ticks();

   if (is_iwdg_enabled() && is_app_healthy()){
      DEBUG_PRINT("app is healthy, kicking watchdog!");
      watchdog_kick();
   }
}

void systick_init(void)
{
   u32 ticks = get_tick_rate() / 1000;

   SYSTICK->LOAD = ticks - 1;
   SYSTICK->VAL = 0;

   SYSTICK->CTRL =
      (1U << 2) |  // CLKSOURCE
      (1U << 1) |  // TICKINT
      (1U << 0);   // ENABLE

}

void systick_msec_delay(u32 delay)
{
    u32 start = system_ticks_g;

    if (!(SYSTICK->CTRL & (1 << 0))) {
        DEBUG_PRINT("error: systick not enabled!");
        return;
    }

    if (!(SYSTICK->CTRL & (1 << 1))) {
        DEBUG_PRINT("error: systick interrupt not enabled!");
        return;
    }
    
    PLAIN_PRINT("- delay [%d] ... ", delay);
    while ((system_ticks_g - start) < delay) {
         cpu_wait_for_interrupt();
    }
    
    PLAIN_PRINT("OK!\r\n");
}


void systick_reset()
{
    SYSTICK->CTRL = 0;
    SYSTICK->LOAD = 0;
    SYSTICK->VAL = 0;
}

u32 get_tick_rate()
{
   u32 sysclk = 8000000; 

   u32 sws = (RCC->CFGR >> 2) & 0x3;

   switch (sws)
   {
      case 0x0: // HSI
      case 0x1: // HSE (no 25mhz)
         break;

      case 0x2: // PLL
      {
         u32 pllmul = ((RCC->CFGR >> 18) & 0xF) + 2;
         u32 pllsrc = (RCC->CFGR >> 16) & 0x1;

         if (pllsrc) {
            sysclk = sysclk * pllmul;
         } else {
            sysclk = (8000000 / 2) * pllmul;
         }
         
         if (sysclk > 72000000) {
            sysclk = 8000000;
         }
         break;
      }

      default:
         return 0;
   }
   u32 hpre = (RCC->CFGR >> 4) & 0xF;

   static const u16 ahb_prescaler[16] = {
      1, 1, 1, 1,
      1, 1, 1, 1,
      2, 4, 8, 16,
      64, 128, 256, 512
   };

   return sysclk / ahb_prescaler[hpre];
}

ret reset_system_clock() 
{
   u64 timeout = DEFAULT_CLOCK_TIMEOUT;
   RCC->CFGR &= CLEAR_BITS(0x3, 0);
   RCC->CFGR |= HSI_AS_SYSTEMCLOCK; 
   
   while((RCC->CR & HSI_READY) == 0 && timeout > 0){
      timeout--;
   }

   return timeout == 0 ? TIMEOUT: SUCCESS;
}

// that's probably depricated, should be used with caution
ret set_system_clock_25Mhz()
{
   u64 timeout = DEFAULT_CLOCK_TIMEOUT; 

   RCC->CR |= ENABLE_HSE;
   while ((RCC->CR & HSE_READY) == 0 && timeout > 0) {
      timeout--;
   }
   if (timeout == 0) return TIMEOUT;

   RCC->CFGR &= ~(0xF << 4);    // clear HPRE  (AHB prescaler) 
   RCC->CFGR &= ~(0x7 << 8);    // clear PPRE1 (APB1 prescaler)
   RCC->CFGR &= ~(0x7 << 11);   // clear PPRE2 (APB2 prescaler)
   
   RCC->CFGR |= (0x0 << 4);     // HPRE  = /1 (SYSCLK = HCLK)
   RCC->CFGR |= (0x0 << 8);     // PPRE1 = /1 (HCLK = PCLK1)
   RCC->CFGR |= (0x0 << 11);    // PPRE2 = /1 (HCLK = PCLK2)
   
   RCC->CFGR &= CLEAR_BITS(0b11, 0);
   RCC->CFGR |= HSE_AS_SYSTEMCLOCK;
   
   timeout = DEFAULT_CLOCK_TIMEOUT;
   while (((RCC->CFGR >> 2) & 0x3) != 0x1 && timeout > 0) {
      timeout--;
   }
   if (timeout == 0) return TIMEOUT;

   return SUCCESS;
}

u32 get_system_ticks()
{
   return system_ticks_g;
}

void increment_system_ticks(void)
{
   system_ticks_g++;
}

ret set_system_clock_72Mhz()
{
   u64 timeout = DEFAULT_CLOCK_TIMEOUT;

   RCC->CR |= ENABLE_HSE;
   while ((RCC->CR & HSE_READY) == 0 && timeout > 0) {
      timeout--;
   }
   if (timeout == 0) return TIMEOUT;

   FLASH->ACR = PREFETCH_ENABLE | HALF_CYCLE_ACCESS_DISABLE | LATENCY_2;

   RCC->CFGR &= ~(0xF << 18);  // clear PLLMUL bits
   RCC->CFGR &= ~(1 << 17);    // clear PLLXTPRE bit
   RCC->CFGR &= ~(1 << 16);    // clear PLLSRC bit
   
   RCC->CFGR |= (1 << 16);      // PLLSRC = HSE
   RCC->CFGR |= (0x7 << 18);    // PLLMUL = 9 
   
   RCC->CR |= ENABLE_PLL;
   
   timeout = DEFAULT_CLOCK_TIMEOUT;
   while (!(RCC->CR & PLL_READY) && timeout > 0) {
      timeout--;
   }
   if (timeout == 0) return TIMEOUT;

   RCC->CFGR &= ~(0xF << 4);    // clear HPRE  (AHB prescaler)
   RCC->CFGR &= ~(0x7 << 8);    // clear PPRE1 (APB1 prescaler)
   RCC->CFGR &= ~(0x7 << 11);   // clear PPRE2 (APB2 prescaler)
   
   RCC->CFGR |= (0x4 << 8);      // PPRE1 = /2
   
   RCC->CFGR &= ~(0x3 << 0);     // clear SW bits
   RCC->CFGR |= PLL_AS_SYSTEMCLOCK;
   
   timeout = DEFAULT_CLOCK_TIMEOUT;
   while (((RCC->CFGR >> 2) & 0x3) != 0x2 && timeout > 0) {
      timeout--;
   }
   if (timeout == 0) return TIMEOUT;

   return SUCCESS;
}
