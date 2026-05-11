#include <aether.h>

__attribute__((naked))
static void jump_to_app(uint32_t pc, uint32_t sp) {
   USED(pc); USED(sp);

    __asm volatile(
        "msr msp, r1\n"
        "dsb\n"
        "isb\n"
        "bx  r0\n"
    );
}

void load_from_desc(const app_desc_t *desc) 
{
    uint32_t pc = desc->entry_point;
    uint32_t sp = desc->p_stack;
    
    jump_to_app(pc, sp);
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
   status = rcc_init_uart_clock(USART1, GPIOA, 9, GPIOA, 10);
   if (IS_ERROR(status)) {
      toggle_degug_led();
      return status;
   }
   uart_init(USART1, 0);

#ifdef SYSTEM_CLOCK_72Mhz
   status = set_system_clock_72MHz();
#else 
   status = set_system_clock_25MHz();
#endif 
  
   if (IS_ERROR(status)) {
      BOOTLOADER_ERROR("error in setting up clock\n");
      return status;
   }
   
   BOOTLOADER_DEBUG("SETUP SYSTEM ... OK\r\n");

   return status;
}

int bootloader_entry() 
{
   ret res;
   u32 count;
   module_t mods[MAX_MODULES];

   res = setup_system();
   if (IS_ERROR(res)) {
      BOOTLOADER_ERROR("failed to set up system!\n");
      return -1;
   }

   count = config_lookup_mods_count();
   
   if (count > MAX_MODULES || count == 0) {
      BOOTLOADER_ERROR("count of modules exceeds max value or equals zero!\n");
      return -1;
   }

   res = config_parse_mods(mods, count);

   if (IS_ERROR(res)) {
      BOOTLOADER_ERROR("failed to parse mods\n");
      return -1;
   }

   run_scheduler(mods, count);

   BOOTLOADER_ERROR("=== POINT OF NO REACH ===\n");
   return 0;
}
