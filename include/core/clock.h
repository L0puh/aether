#ifndef CLOCK_H
#define CLOCK_H

#include "defs.h"
#include "core/gpio.h"

#define DEFAULT_CLOCK_TIMEOUT 100000

#define ENABLE_HSE  (1 << 16)
#define ENABLE_PLL  (1 << 24)
#define ENABLE_PLL2 (1 << 26)

#define HSE_READY  (1 << 17)
#define HSI_READY  (1 << 1)
#define PLL_READY  (1 << 25)
#define PLL2_READY  (1 << 27)

#define HSE_AS_SYSTEMCLOCK (0b01 << 0)
#define HSI_AS_SYSTEMCLOCK (0b00 << 0)
#define PLL_AS_SYSTEMCLOCK (0b10 << 0)

#define LATENCY_2   (0b010 << 0)
#define LATENCY_1   (0b001 << 0)
#define LATENCY_0   (0b000 << 0)


void AF_enable(GPIO_t *port, u8 num);
ret rcc_enable_clock_pin(GPIO_t* pin);
ret set_system_clock_25Mhz();
ret set_system_clock_72MHz();
ret reset_system_clock();
ret rcc_init_uart_clock(USART_t* uart, GPIO_t *tx, u8 tx_num, GPIO_t *rx, u8 rx_num);

#endif 
