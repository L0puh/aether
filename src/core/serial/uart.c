#include "core/systick.h"
#include <aether.h>

USART_t *opened_usart_g = 0;

void uart_set_baudrate(USART_t *uart, u32 periph_clk, u32 baudrate)
{
   uart->BAUD = ((periph_clk + (baudrate/2U))/baudrate);
}


bool uart_init(USART_t* uart, u16 flags)
{
   UNUSED(flags);
   opened_usart_g = uart;

   uart_reset(uart);
   uart_set_baudrate(uart, SYS_FREQ, DBG_UART_BAUDRATE);
   uart_enable(uart);

   systick_msec_delay(100); 
   return 1;
}

static void uart_putchar(int ch)
{
   if (!opened_usart_g)
      return;

   opened_usart_g->DATA = (ch & 0xFF);
   while (!(opened_usart_g->STATUS & TX_COMPLETE));
}

void uart_write(char* str) {
   while(*str) {
      uart_putchar(*str++);
   }
}

int __io_putchar(int ch)
{
   uart_putchar(ch);

   return ch;
}

void uart_enable(USART_t *uart)
{
   // UE, TE, RE
   uart->CR1 = (1 << 13) | (1 << 3) | (1 << 2);
}

void uart_reset(USART_t *uart)
{
   uart->CR1 = 0;
   uart->CR2 = 0;
   uart->CR3 = 0;
}
