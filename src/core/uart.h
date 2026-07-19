#ifndef UART_H
#define UART_H

/* default mapping for UART
 * 
 * USART1: TX=PA9  RX=PA10
 * USART2: TX=PA2  RX=PA3
 * USART3: TX=PB10 RX=RP11
 *
 */


#include "defs.h"
#include "core/gpio.h"

#define   DATALEN_8BITS  0b000001
#define   DATALEN_9BITS  0b000010
#define   ONE_STOPBIT    0b000100
#define   TWO_STOPBITS   0b001000
#define   TX_EMPTY         (1U << 7)
#define   TX_COMPLETE      (1U << 6)
#define   RXNE             (1U << 5)
#define   HSI_ON           (1U << 0)
#define   UART_ENABLE      (1 << 13)
#define   UART_TX_ENABLE   (1 << 3)
#define   UART_RX_ENABLE   (1 << 2)

bool uart_init(USART_t* uart, u16 flags);
void uart_set_baudrate(USART_t *uart, u32 periph_clk, u32 baudrate);
void uart_write(const char* str);
void uart_reset(USART_t *uart);
void uart_enable(USART_t *uart);
void uart_putchar(int ch);
ret uart_getline(char* buffer, u64 size);
ret uart_getchar(char *c);
u32 uart_read_word(void);
bool uart_rx_ready();
bool uart_wait_rx_ready(u32 timeout);
void uart_writef(const char* str, ...);
//TODO: void uart_writef_ex(USART_t *usart, const char* str, ...);
u32 uart_data();
void uart_flush(void);
void uart_flush_tx(void);
void uart_flush_rx(void);
ret uart_read_buffer(u8* buf, u32 len);


//extended: need to be fixed though FIXME
void uart_write_ex(USART_t* uart, const char* str);
void uart_putchar_ex(USART_t* uart, int ch);
#endif 
