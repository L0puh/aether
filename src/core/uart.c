#include <aether.h>
#include <core/systick.h>

USART_t *opened_usart_g = 0;

void uart_set_baudrate(USART_t *uart, u32 pclk, u32 baudrate)
{
    uart->BAUD= (pclk + (baudrate / 2U)) / baudrate;
}

bool uart_init(USART_t* uart, u16 flags)
{
   UNUSED(flags);
   opened_usart_g = uart;

   uart_reset(uart);
   uart_enable(uart);

#ifdef SYSTEM_CLOCK_72Mhz
   uart_set_baudrate(uart, 72000000, DBG_UART_BAUDRATE);
#elif defined(SYSTEM_CLOCK_25Mhz)
   uart_set_baudrate(uart, 25000000, DBG_UART_BAUDRATE);
#else
   uart_set_baudrate(uart, 8000000, DBG_UART_BAUDRATE);
#endif
   return 1;
}

void uart_putchar(int ch)
{
   if (!opened_usart_g)
      return;

   while (!(opened_usart_g->STATUS & TX_EMPTY));

   opened_usart_g->DATA = (ch & 0xFF);

   while (!(opened_usart_g->STATUS & TX_COMPLETE));
}

void uart_write(const char* str) 
{
   while(*str) {
      uart_putchar(*str++);
   }
}

bool uart_rx_ready()
{
   return opened_usart_g && (opened_usart_g->STATUS & RXNE);
}

bool uart_wait_rx_ready(u32 timeout_ms)
{
    u32 start = system_ticks_g;  

    while (!uart_rx_ready()) {
        if ((system_ticks_g - start) > timeout_ms) {
            return false;  
        }
    }

    return true;  
}

ret uart_getchar(char *c)
{
   if (uart_rx_ready()) { // TODO: add watchdog (?)
      *c = opened_usart_g->DATA;
      return SUCCESS;
   }
   
   return NOT_FOUND;
}

ret uart_getline(char* buffer, u64 size)
{
   ret res;
   u64 i = 0;

   while (i < size - 2)
   {
      res = uart_getchar(&buffer[i]);
      if (res == NOT_FOUND)
      {
         continue; //TODO: current read is blocking, add non-blocking
      }
      if (buffer[i] == '\n' || buffer[i] == '\r') {
         break;
      }
      i++;
   }
   
   if (i == 0) {
      return WRONG_DATA;
   }

   buffer[++i] = '\n';
   buffer[++i] = '\0';
   return SUCCESS;
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

u32 uart_data() {
   if (!opened_usart_g){
      return 0;
   }
   return opened_usart_g->DATA;
}

void uart_flush_rx(void)
{
    while (uart_rx_ready()) {
        volatile u8 dummy = uart_data(); 
        UNUSED(dummy);
    }
}

void uart_flush_tx(void)
{
    while (!(opened_usart_g->STATUS & TX_EMPTY));
    while (!(opened_usart_g->STATUS & TX_COMPLETE));
}

void uart_flush(void)
{
    uart_flush_rx();
    uart_flush_tx();
}
