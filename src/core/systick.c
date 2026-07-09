#include "core/debug.h"
#include <core/systick.h>

volatile u32 system_ticks_g = 0;

void systick_init()
{
   u32 ticks = get_tick_rate() / 1000;

   SYSTICK->LOAD = ticks - 1;
   SYSTICK->VAL = 0;

   SYSTICK->CTRL =
      (1U << 2) |  // CLKSOURCE
      (1U << 1) |  // TICKINT
      (1U << 0);   // ENABLE
}

void systick_msec_delay(u32 delay)
{
   u32 start = system_ticks_g;

   DEBUG_PRINT("DELAY: %d\r\n", delay);
   DEBUG_PRINT("START: %d\r\n", start);
   while ((system_ticks_g - start) < delay) {
   }
}

void systick_reset()
{
    SYSTICK->CTRL = 0;
    SYSTICK->LOAD = 0;
    SYSTICK->VAL = 0;
}


 
