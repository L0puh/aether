#include "core/gpio.h"

void disable_irq(void)
{
   __asm volatile("cpsid i");
}

void enable_irq(void)
{
   __asm volatile("cpsie i");
}

