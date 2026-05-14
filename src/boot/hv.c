#include <aether.h>

bool is_valid_app_call(void)
{
   u32 lr;
   __asm volatile("mov %0, lr" : "=r"(lr));
   u32 addr = lr & ~1;

   return (addr >= START_APP_SLOT && addr <= END_APP_SLOT);
}

void init_debug_led()
{
   rcc_enable_clock_pin(GPIOC);

   GPIOC->CRH &= CLEAR_BITS(4, 20);
   GPIOC->CRH |= OUTPUT_50MHZ | GENERAL_PUSHPULL;
}

void toggle_debug_led()
{
   GPIOC->ODR ^= (1 << 13);
}

void led_blink(int count, int time)
{
   while (count--)
   {
      toggle_debug_led();

      for (volatile int i = 0; i < time; i++);

      toggle_debug_led();

      for (volatile int i = 0; i < time; i++);
   }
}

ret setup_system()
{
   ret status;

   mpu_init();

#ifdef SYSTEM_CLOCK_72Mhz
   status = set_system_clock_72Mhz();
#else
   status = set_system_clock_25Mhz();
#endif

   systick_init();
   init_debug_led();

   status = rcc_init_uart_clock(
         USART1,
         GPIOA, 9,
         GPIOA, 10
         );

   if (IS_ERROR(status))
      return status;

   uart_init(USART1, 0);

   hv_api_init();

   BOOTLOADER_DEBUG("SYSTEM OK\r\n");

   return SUCCESS;
}
