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

#define PREDIV2SRC_PLL2 (1 << 16)
#define PREDIV2SRC_HSE  (1 << 16)
#define PLLSRC_HSI      (0 << 16)
#define PLLSRC_PREDIV1  (1 << 16)

#define HSE_AS_SYSTEMCLOCK (0b01 << 0)
#define HSI_AS_SYSTEMCLOCK (0b00 << 0)
#define PLL_AS_SYSTEMCLOCK (0b10 << 0)

#define LATENCY_2   (0b010 << 0)
#define LATENCY_1   (0b001 << 0)
#define LATENCY_0   (0b000 << 0)
#define HPRE_DIV1             (0b000 << 4)
#define PPRE2_DIV1            (0b100 << 11)
#define PPRE1_DIV2            (0b100 << 8)

#define CLEAR_PREDIV1 (0b0000 << 0)
#define PREDIV2_FACTOR(num)  (((num) - 1) << 4)
#define PREDIV1_FACTOR(num)  (((num) - 1) << 0)
#define PLL2MULL_FACTOR(num) (((num) - 2) << 8)

#define PLLMULL9              (0b0111 << 18)
#define PLLSRC_HSE            (1      << 16)
#define PLL_SWITCH_ON         (0b10   << 2)
#define APB1_DIV2             (0b100  << 8)


void AF_enable(GPIO_t *port, u8 num);
ret rcc_enable_clock_pin(GPIO_t* pin);
ret set_system_clock_25Mhz();
ret set_system_clock_72MHz();
ret reset_system_clock();
ret rcc_init_uart_clock(USART_t* uart, GPIO_t *tx, u8 tx_num, GPIO_t *rx, u8 rx_num);

#endif 
