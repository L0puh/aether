#include "core/uart.h"
#include <aether.h>

int main(void)
{

   if (hv_request_periph(PERIPH_USART1, AP_PRIV_RW_USER_RW) != 0) {
      return -1;
   }

   USART_t *usart = (USART_t*)USART1_BASE;  
   uart_write_ex(usart, "HELLO WORLD FROM APP! I JUST GOT FREEDOM OF SPEECH\r\n");
   uart_write_ex(usart, "now, let's try corrupt some memory :) \r\n");
   
   uart_write_ex(usart, "attempting to get ungranted periph\r\n");
   if (hv_request_periph(PERIPH_USART2, AP_PRIV_RW_USER_RW) != 0) {
      uart_write_ex(usart, "error! still trying to mess around\r\n");
      USART_t *usart2 = (USART_t*)USART2_BASE;
      uart_init(usart2, 0);
      uart_write_ex(usart2, "heyy from usart 2 (ungranted)\r\n");

   }

   volatile u32 *desc_app = (volatile u32*) FLASH_APP_ORIGIN;
   uart_write_ex(usart, "attempting to write to app desc (protected by MPU)\r\n");
   
   *desc_app = 0xDEAD;

   uart_write_ex(usart, "done, now write to hv memory\r\n");

   volatile u32 *some_hv_stuff = (volatile u32*) (RAM_HV_ORIGIN + 8);
   *some_hv_stuff = 0xDEAD;

   uart_write_ex(usart, "done\r\n");

   return 0;
}
