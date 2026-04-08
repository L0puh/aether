#include <aether.h>

void uart_set_baudrate(USART_t *uart, u32 periph_clk, u32 baudrate)
{
   u16 value = (periph_clk + (baudrate/2U)) / baudrate;
   uart->BAUD = value;
}

bool uart_init(USART_t* uart, u16 flags)
{
   UNUSED(flags);
   //TODO: process flags and 
   //set proper baudrate with the function
   
   uart->CR1 |= (1 << 13);
   uart->CR1 |= (0 << 12);
   uart->CR2 |= (0b00 << 12);
   uart->BAUD = 0x1d4c;

   uart->CR1 |= (1 << 3);
   uart->CR1 |= (1 << 2);
   
   return 1;
}

void uart_write() 
{
}


