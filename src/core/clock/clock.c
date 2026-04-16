#include <aether.h>

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

ret set_system_clock_72MHz()
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

void AF_enable(GPIO_t *port, u8 num)
{
   //TODO: add general for every num
   GPIOA->CRH &= ~(0xF << 4);    
   GPIOA->CRH |= (0xB << 4);      
   UNUSED(num);
}


ret rcc_enable_clock_pin(GPIO_t* pin){
   if (!pin) 
      return INVALID_PARAMETER;

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
   if (!uart) return INVALID_PARAMETER;
   if (uart == USART1)
      RCC->APB2ENR |= RCC_USART1EN;
   else if (uart == USART2) 
      RCC->APB1ENR |= RCC_USART2EN;
   else if (uart == USART3) 
      RCC->APB1ENR |= RCC_USART3EN;
   else
      return NOT_FOUND;

   return SUCCESS;
}

ret rcc_init_uart_clock(USART_t* uart, GPIO_t *tx, u8 tx_num, GPIO_t *rx, u8 rx_num)
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
      AF_enable(tx, tx_num);
   }

   if (rx) {
      status = rcc_enable_clock_pin(rx);
      if (IS_ERROR(status)){
         return status;
      }
      UNUSED(rx_num);
      //TODO: INPUT FLOATING 
   }

   return rcc_uart_clock_enable(uart);
}
