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

#define CSR_OBLRSTF        (1 << 25)  // option byte loader reset flag
#define CSR_RMVF           (1 << 24)  // remove reset flag 
#define CSR_LSION          (1 << 0)   // LSI oscillator enable
#define CSR_LSIRDY         (1 << 1)   // LSI oscillator ready
                                      
#ifdef _DEBUG

typedef struct {
   u32 mask;
   const char* name;
} reset_cause_t;

#define CSR_LOWPOWER       (1 << 31)  
#define CSR_WWATCHDOG      (1 << 30)  
#define CSR_IWATCHDOG      (1 << 29) 
#define CSR_SOFTW_RESET    (1 << 28)  
#define CSR_POWER          (1 << 27)  
#define CSR_PIN            (1 << 26)  


static const reset_cause_t reset_causes_g[] = {
   { CSR_LOWPOWER,      "low power reset"             },
   { CSR_WWATCHDOG,     "window watchdog reset"       },
   { CSR_IWATCHDOG,     "independent watchdog reset"  },
   { CSR_SOFTW_RESET,   "software reset"              },
   { CSR_POWER,         "power on / power down reset" },
   { CSR_PIN,           "external pin reset"          }

};

#define NUM_OF_RESET_CAUSES (sizeof(reset_causes_g)/sizeof(reset_causes_g[0]))

#endif 

ret rcc_enable_clock_pin(GPIO_t* pin);
ret set_system_clock_25Mhz();
ret set_system_clock_72Mhz();
ret reset_system_clock();
ret rcc_init_uart_clock(USART_t* uart, GPIO_t *tx, u8 tx_num, GPIO_t *rx, u8 rx_num);
u32 get_tick_rate();
void check_reset_cause(void);

#endif 
