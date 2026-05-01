#ifndef UART_H
#define UART_H

#include "core/gpio.h"
#include <stdbool.h>

#define   DATALEN_8BITS  0b000001
#define   DATALEN_9BITS  0b000010
#define   ONE_STOPBIT    0b000100
#define   TWO_STOPBITS   0b001000
#define   TX_EMPTY      (1U << 7)
#define   TX_COMPLETE   (1U << 6)
#define   RXNE          (1U << 5)

extern USART_t *opened_usart_g;
bool uart_init(USART_t* uart, u16 flags);
void uart_set_baudrate(USART_t *uart, u32 periph_clk, u32 baudrate);
void uart_write(char* str);
void uart_reset(USART_t *uart);
void uart_enable(USART_t *uart);
ret uart_getline(char* buffer, u64 size);
ret uart_getchar(char *c);
bool uart_rx_ready();

#endif 
