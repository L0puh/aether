#include <aether.h>

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

    if (!(SYSTICK->CTRL & (1 << 0))) {
        DEBUG_PRINT("error: systick not enabled!\r\n");
        return;
    }

    if (!(SYSTICK->CTRL & (1 << 1))) {
        DEBUG_PRINT("error: systick interrupt not enabled!\r\n");
        return;
    }
    
    DEBUG_PRINT("> delay [%d]... ", delay);
    while ((system_ticks_g - start) < delay) {
         cpu_wait_for_interrupt();
    }
    
    DEBUG_PRINT("OK!\r\n");
}


void systick_reset()
{
    SYSTICK->CTRL = 0;
    SYSTICK->LOAD = 0;
    SYSTICK->VAL = 0;
}


 
