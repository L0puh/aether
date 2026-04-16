#ifndef CLOCK_H
#define CLOCK_H

#include "defs.h"
#include "core/gpio.h"

#define SYSTEM_CLOCK_25Mhz 0b1
#define SYSTEM_CLOCK_72Mhz 0b0

#define ENABLE_HSE (1 << 16)
#define ENABLE_PLL (1 << 24)

#define HSE_READY  (1 << 17)
#define PLL_READY  (1 << 25)

#define HSE_AS_SYSTEMCLOCK (0b01 << 0)
#define HSI_AS_SYSTEMCLOCK (0b00 << 0)
#define PLL_AS_SYSTEMCLOCK (0b10 << 0)

#define FLASH_ARC_PRFTBE      (1 << 5)
#define FLASH_ACR_LATENCY_2   (0b010 << 0)
#define FLASH_ACR_LATENCY_1   (0b001 << 0)
#define FLASH_ACR_LATENCY_0   (0b000 << 0)
#define HPRE_DIV1             (0b000 << 4)
#define PPRE2_DIV1            (0b100 << 11)
#define PPRE1_DIV2            (0b100 << 8)

#define PLLMULL9              (0b0111 << 18)
#define PLLSRC_HSE            (1      << 16)
#define PLL_SWITCH_ON         (0b10   << 2)

void set_system_clock(u8 freq);
void AF_enable(GPIO_t *pin);

#endif 
