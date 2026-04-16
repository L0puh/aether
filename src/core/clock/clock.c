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


void AF_enable(GPIO_t *pin)
{
    pin->CRH &= ~(0xF << 4);
    pin->CRH |= (0xB << 4);
}


ret rcc_enable_clock_pin(GPIO_t* pin){
   if (!pin) 
      return INVALID_PARAMETR;

   if (pin == GPIOA)
      RCC->APB2ENR |= RCC_GPIOAEN;
   else if (pin == GPIOB)
      RCC->APB2ENR |= RCC_GPIOBEN;
   else if (pin == GPIOC)
      RCC->APB2ENR |= RCC_GPIOCEN;
   else 
      return NOT_FOUND;

   return SUCCESS;
}

ret rcc_uart_clock_enable(USART_t* uart){
   if (uart == USART1)
      RCC->APB2ENR |= RCC_USART1EN;
   else if (uart == USART2) 
      RCC->APB1ENR |= RCC_USART2EN;
   else if (uart == USART3) 
      RCC->APB1ENR |= RCC_USART3EN;
   else
      return NOT_FOUND;
}

ret rcc_init_uart_clock(USART_t* uart, GPIO_t *tx, GPIO_t *rx)
{

   ret status;
   if (!uart || (!tx && !rx)) {
      return NOT_FOUND;
   }

   if (tx) {
      status = rcc_enable_clock_pin(tx);
      if (IS_ERROR(status)){
         return status;
      }
      AF_enable(tx);
   }

   if (rx) {
      status = rcc_enable_clock_pin(rx);
      if (IS_ERROR(status)){
         return status;
      }
      AF_enable(rx);
   }

   return rcc_uart_clock_enable(uart);
}
