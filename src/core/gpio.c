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

inline u32 get_msp(void)
{
   u32 msp;
   __asm volatile ("mrs %0, msp" : "=r" (msp));
   return msp;
}

inline u32 get_psp(void)
{
   u32 psp;
   __asm volatile ("mrs %0, psp" : "=r" (psp));
   return psp;
}

void gpio_set_mode(GPIO_t* port, u8 pin, u32 mode)
{
   volatile u32 *cr = (pin < 8) ? &port->CRL : &port->CRH;
   u8 shift = (pin % 8) * 4;
   *cr &= ~(0xFu << shift);
   *cr |= (mode << shift);
}

void set_psp(u32 psp)
{
   __asm volatile ("msr psp, %0" : : "r" (psp) : "memory");
}

u32 get_control(void)
{
   u32 control;
   __asm volatile ("mrs %0, control" : "=r" (control));
   return control;
}

void set_control(u32 control)
{
   __asm volatile ("mrs %0, control" : : "r" (control) : "memory");
}
