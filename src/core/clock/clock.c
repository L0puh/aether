#include "aether.h"

void set_system_clock(u8 freq)
{
   RCC->CR |= ENABLE_HSE;
   while (!(RCC->CR & HSE_READY));
   
   if (freq == SYSTEM_CLOCK_25Mhz) {
      RCC->CFGR |= HSE_AS_SYSTEMCLOCK;
      return;
   } 

   FLASH->ACR |= FLASH_ARC_PRFTBE | FLASH_ACR_LATENCY_2;
   RCC->CFGR |= HPRE_DIV1;
   RCC->CFGR |= PPRE2_DIV1;
   RCC->CFGR |= PPRE1_DIV2;

   RCC->CFGR |= PLLSRC_HSE | PLLMULL9;

   RCC->CR |= ENABLE_PLL;
   while (!(RCC->CR & PLL_READY));
   RCC->CFGR |= PLL_AS_SYSTEMCLOCK;
   
   while (!(RCC->CR & PLL_SWITCH_ON));
}

