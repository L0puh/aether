#include <aether.h>


void uart_set_baudrate(u32 periph_clk, u32 baudrate)
{
   u16 value = (periph_clk + (baudrate/2U)) / baudrate;
   USART2->BAUD = value;
}

void uart_init()
{
}

void uart_write() 
{
}


