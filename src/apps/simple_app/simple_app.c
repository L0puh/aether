#include <aether.h>

int main(void)
{

   if (hv_request_periph(PERIPH_USART1, AP_PRIV_RW_USER_RW) != 0) {
      return -1;
   }

   /* USART_t *usart = (USART_t*)(USART1_BASE); */

   uart_writef("HELLO WORLD!\r\n");

   volatile int i = 0;
   for (; i < 10000; i++){
      __asm volatile ("nop");
   }

   return 0;
}
