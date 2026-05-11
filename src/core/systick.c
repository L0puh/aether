#include <core/systick.h>

void systick_init() 
{
   systick_reset();

   u32 ticks = get_tick_rate() / 1000;
   SYSTICK->LOAD = ticks -1;
   SYSTICK->VAL = 0;
   SYSTICK->CTRL  = (1U << 2); //internal clock
   SYSTICK->CTRL |= (1U << 0); //enable clock
   SYSTICK->CTRL |= (1U << 1); //enable interrupt  
                               
}

void systick_msec_delay(u64 delay)
{
   for (u64 i = 0; i < delay; i++){
      while ((SYSTICK->CTRL & CTRL_COUNTFLAG) == 0) {
      }
   }

   SYSTICK->CTRL = 0;
}

void systick_reset()
{
    SYSTICK->CTRL = 0;
    SYSTICK->LOAD = 0;
    SYSTICK->VAL = 0;
}

void cpu_wait_for_interrupt() 
{
   __asm volatile ("wfi");
}
 
