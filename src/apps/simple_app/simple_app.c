#include "core/clock.h"
#include "core/uart.h"
#include <aether.h>

int main(void)
{
    USART_t *usart = (USART_t*)USART1_BASE;  
    if (hv_request_periph(PERIPH_USART1, AP_PRIV_RW_USER_RW) != 0) {
        return -1;
    }
    uart_write_ex(usart, "HELLO WORLD FROM APP! I JUST GOT FREEDOM OF SPEECH\r\n");

    return 0;
}
