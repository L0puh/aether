#include "core/gpio.h"

void disable_irq(void)
{
   __asm volatile("cpsid i");
}

void enable_irq(void)
{
   __asm volatile("cpsie i");
}

/* waits for all memory accesses to complete */
void data_sync_barrier(void)
{
    __asm volatile ("dsb" : : : "memory");
}

void instr_sync_barrier(void)
{
    __asm volatile ("isb" : : : "memory");
}


