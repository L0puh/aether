#include <core/systick.h>

void systick_msec_delay(u64 delay)
{
   SYSTICK->LOAD = ONE_MSEC_LOAD -1;
   SYSTICK->VAL = 0;
   SYSTICK->CTRL  = (1U << 2); //internal clock
   SYSTICK->CTRL |= (1U << 0); //enable clock
   
   for (u64 i = 0; i < delay; i++){
      while ((SYSTICK->CTRL & CTRL_COUNTFLAG) == 0) {
      }
   }

   SYSTICK->CTRL = 0;
}

