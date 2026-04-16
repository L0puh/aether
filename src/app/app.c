#include <aether.h>

void main(void) 
{

   set_system_clock(SYSTEM_CLOCK_25Mhz);
   
   //PA9 as TX
   RCC->APB2ENR |= RCC_GPIOAEN;
   RCC->APB2ENR |= RCC_USART1EN;
   AF_enable(GPIOA);
   uart_init(USART1, 0);

   // LED 
   RCC->APB2ENR |= RCC_GPIOCEN;
   GPIOC->CRH &= ~(0xF << 20);
   GPIOC->CRH |= (0x3 << 20);

   char* msg = "Hello, UART!\r\n";
   while(1) 
   {
      GPIOC->ODR ^= (1 << 13);
      uart_write(msg);
      systick_msec_delay(3000);
   }

}
