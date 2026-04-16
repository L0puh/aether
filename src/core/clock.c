#include <aether.h>

void set_system_clock(u8 freq)
{
   RCC->CR |= ENABLE_HSE;
   while (!(RCC->CR & HSE_READY));
   
   FLASH->ACR |= FLASH_ARC_PRFTBE | FLASH_ACR_LATENCY_1;
   RCC->CFGR |= HPRE_DIV1;
   RCC->CFGR |= PPRE2_DIV1;
   RCC->CFGR |= PPRE1_DIV2;
   
   if (freq == SYSTEM_CLOCK_25Mhz) {
      RCC->CFGR &= ~(0x3);
      RCC->CFGR |=  HSE_AS_SYSTEMCLOCK;
      while ((RCC->CFGR & 0x3) != 0b01){}
      return;
   } else {
      //72Mhz is deprecated...
      return;
   }


   RCC->CFGR |= PLLSRC_HSE | PLLMULL9;

   RCC->CR |= ENABLE_PLL;
   while (!(RCC->CR & PLL_READY));
   RCC->CFGR |= PLL_AS_SYSTEMCLOCK;
   
   while (!(RCC->CR & PLL_SWITCH_ON));
}

void AF_enable(GPIO_t *pin){
   pin->CRH &= ~(0xF << 4);
   pin->CRH |= (0xB << 4);
}
