#ifndef UART_H
#define UART_H

#include "core/gpio.h"

#define   DATALEN_8BITS 0b000001
#define   DATALEN_9BITS 0b000010
#define   ONE_STOPBIT   0b000100
#define   TWO_STOPBITS  0b001000

extern USART_t opened_usart;

bool uart_init(USART_t* uart, u16 flags);

#endif 
