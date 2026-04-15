#include <aether.h>

USART_t *opened_usart_g = 0;

void uart_set_baudrate(USART_t *uart, u32 periph_clk, u32 baudrate)
{
   u16 value = (periph_clk * 2 + baudrate) / (2 * baudrate);
   uart->BAUD = value;
}

bool uart_init(USART_t* uart, u16 flags)
{
   UNUSED(flags);
   opened_usart_g = uart;

   //TODO: process flags and
   //set proper baudrate with the function
   
   //FIXME: enable clocks for pins too

   uart->CR1 |= (1 << 13);
   uart->CR1 |= (0 << 12);
   uart->CR2 |= (0b00 << 12);
   uart->BAUD = 0x1d4c;

   uart->CR1 |= (1 << 3);
   uart->CR1 |= (1 << 2);

   return 1;
}

bool uart_write(char *str)
{
    if (!opened_usart_g)
        return 0;

    while (*str)
    {
        while (!(opened_usart_g->STATUS & (1 << 7))); 
        opened_usart_g->DATA = *str++;
    }
    return 1;
}
