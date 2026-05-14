#include <aether.h>

#define SCB_KEY_WRITE      (0x5FA << 16)
#define SCB_ACTIVATE_RESET (1 << 2)

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


void wait_interrupt() {
   while(1) {
      cpu_wait_for_interrupt();
   }
}


void cpu_wait_for_interrupt() 
{
   __asm volatile ("wfi");
}

__attribute__((noreturn))
void system_reset(void)
{
   disable_irq();
   data_sync_barrier();

   SCB->AIRCR = SCB_KEY_WRITE | SCB_ACTIVATE_RESET;

   instr_sync_barrier();

   while(1);
}
