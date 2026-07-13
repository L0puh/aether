#include "target.h"
#include <aether.h>


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
   if (!uart) {
      return INVALID_PARAMETER;
   }
   
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

      gpio_set_mode(tx, tx_num, AF_PUSH_PULL_50MHz);
   }

   if (rx) {
      status = rcc_enable_clock_pin(rx);
      if (IS_ERROR(status)){
         return status;
      }
      
      gpio_set_mode(rx, rx_num, INPUT_FLOATING);
   }

   return rcc_uart_clock_enable(uart);
}

#ifdef _DEBUG
void check_reset_cause(void) 
{
   u32 flags = RCC->CSR;

   DEBUG_PRINT("[!] reset cause: ");

   for (u32 i = 1; i < NUM_OF_RESET_CAUSES; i++) {
      if (flags & reset_causes_g[i].mask) {
         DEBUG_PRINT("%s\r\n", reset_causes_g[i].name);
         goto found;
      }
   }

   DEBUG_PRINT("unknown reset source (0x%08x)\r\n", flags);

found:
   RCC->CSR |= CSR_RMVF;
}
#else 
void check_reset_cause(void) {
   RCC->CSR |= CSR_RMVF;
}
#endif 
