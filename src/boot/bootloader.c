#include <aether.h>
#include <module_info.h>

__attribute__((naked))
static void jump_to_app(uint32_t pc, uint32_t sp)
{
    __asm volatile(
        "msr msp, r1      \n"   // set stack
        "mov r2, r0       \n"   // pc
        "bx  r2           \n"
    );
}

void load_from_desc(const app_desc_t *desc) 
{
    BOOTLOADER_DEBUG("loading entry point!");

    uint32_t pc = desc->entry_point;
    uint32_t sp = desc->p_stack;

    BOOTLOADER_DEBUG("jummping to module!");
    disable_irq();
    jump_to_app(pc, sp);
    enable_irq();
}

void load_from_flash(u32 addr)
{
    app_desc_t *desc = (app_desc_t*)addr;
    
    uint32_t pc = desc->entry_point;
    uint32_t sp = desc->p_stack;
    
    jump_to_app(pc, sp);
}

void init_debug_led()
{
   rcc_enable_clock_pin(GPIOC);
   GPIOC->CRH &= CLEAR_BITS(4, 20);
   GPIOC->CRH |= OUTPUT_50MHZ | GENERAL_PUSHPULL;
}

void toggle_degug_led() 
{
   GPIOC->ODR ^= (1 << 13);
}

ret setup_system() 
{
   ret status;
   
   init_debug_led();
   
#ifdef SYSTEM_CLOCK_72Mhz
   status = set_system_clock_72Mhz();
#else 
   status = set_system_clock_25Mhz();
#endif 
   
   systick_init();

   status = rcc_init_uart_clock(USART1, GPIOA, 9, GPIOA, 10);
   if (IS_ERROR(status)) {
      toggle_degug_led();
      return status;
   }
   uart_init(USART1, 0);
  
   if (IS_ERROR(status)) {
      toggle_degug_led();
      return status;
   }
   
   BOOTLOADER_DEBUG("SETUP SYSTEM ... OK\r\n");

   toggle_degug_led();
   return status;
}

int bootloader_entry() 
{
   ret res;
   u32 count;

   res = setup_system();
   if (IS_ERROR(res)) {
      BOOTLOADER_ERROR("failed to set up system!\n");
      return -1;
   }

   run_scheduler(MODULES, MODULES_COUNT);

   BOOTLOADER_ERROR("=== POINT OF NO REACH ===\n");
   return 0;
}
