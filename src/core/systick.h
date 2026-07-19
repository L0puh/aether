#ifndef SYSTICK_H
#define SYSTICK_H


#define CTRL_COUNTFLAG     (1U<<16)
#define ONE_MSEC_LOAD      16000 // 16Mhz

#include <defs.h>
#include <core/clock.h>

// cortex m3
typedef struct _systick {
   u32 CTRL;        // CSR: control and status
   u32 LOAD;        // RVR: reload value
   u32 VAL;         // CVR: current value
   u32 CALIB;       // RCR: calibration value
} systick_t;


#define SYSTICK_BASE    0xE000E010UL
#define SYSTICK           ((systick_t*) SYSTICK_BASE)

void systick_msec_delay(u32 delay);
void systick_reset(void);
void systick_init(void);
u32 get_tick_rate(void);
ret set_system_clock_25Mhz(void);
ret set_system_clock_72Mhz(void);
ret reset_system_clock(void);
void tick_hook(void);
u32 get_system_ticks();
void increment_system_ticks(void);

#endif 
