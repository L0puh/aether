#ifndef SYSTICK_H
#define SYSTICK_H


#define CTRL_COUNTFLAG     (1U<<16)
#define ONE_MSEC_LOAD      16000 // 16Mhz

#include <defs.h>

// cortex m3
typedef struct _systick {
   u32 CTRL;        // CSR: control and status
   u32 LOAD;        // RVR: reload value
   u32 VAL;         // CVR: current value
   u32 CALIB;       // RCR: calibration value
} systick_t;
#define SYSTICK_BASE    0xE000E010UL
#define SYSTICK           ((systick_t*) SYSTICK_BASE)

void systick_msec_delay(u64 delay);
void systick_reset();

#endif 
