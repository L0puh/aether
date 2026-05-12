#include <aether.h>

__attribute__((naked))
static void jump_to_app(uint32_t pc, uint32_t sp)
{
    __asm volatile(
        "msr msp, r1           \n"  // set main stack pointer
        "msr psp, r1           \n"  // set process stack pointer  
        "mov r2, #0            \n"  // 
        "msr control, r2       \n"  // use msp, privileged mode
        "isb                   \n"  // instruction sync barrier
        "dsb                   \n"  // data sync barrier
        "bx r0                 \n"  // jump to app entry
        ::: "r0", "r1", "r2", "memory"
    );
}

void init_debug_led()
{
   rcc_enable_clock_pin(GPIOC);
   GPIOC->CRH &= CLEAR_BITS(4, 20);
   GPIOC->CRH |= OUTPUT_50MHZ | GENERAL_PUSHPULL;
}

void led_blink(int count, int time) {
   while(count != 0){
      toggle_degug_led();
      for(volatile int delay = 0; delay < time; delay++);
      toggle_degug_led();
      count--;
   }
}

void toggle_degug_led() 
{
   GPIOC->ODR ^= (1 << 13);
}

ret setup_system() 
{
   ret status;
   
#ifdef SYSTEM_CLOCK_72Mhz
   status = set_system_clock_72Mhz();
#else 
   status = set_system_clock_25Mhz();
#endif 

   systick_init();
   init_debug_led();

   status = rcc_init_uart_clock(USART1, GPIOA, 9, GPIOA, 10);
   if (IS_ERROR(status)) {
      toggle_degug_led();
      return status;
   }
  
   uart_init(USART1, 0);
   hv_api_init();
   
   if (IS_ERROR(status)) {
      toggle_degug_led();
      return status;
   }
   
   BOOTLOADER_DEBUG("SETUP SYSTEM ... OK\r\n");

   toggle_degug_led();
   return status;
}

void scan_for_apps()
{
   u32 start_addr = 0x08002000;
   u32 end_addr = 0x8008000;
   for (u32 addr = start_addr; addr < end_addr; addr += 0x400){
      app_desc_t* desc = (app_desc_t*)addr;

      if (desc->magic == APP_MAGIC){
         BOOTLOADER_DEBUG("app found!\r\n");
         disable_irq();
         jump_to_app((u32)desc->entry, (u32)desc->p_stack);
      }
   }
   BOOTLOADER_DEBUG("no valid app found!\r\n");
}

int bootloader_entry() 
{
   enable_irq();
   ret res = setup_system();
   BOOTLOADER_DEBUG("BOOTLOADER ENTRY\r\n");

   if (IS_ERROR(res)) {
      return -1;
   }
   led_blink(4, 50000);
   scan_for_apps();

   BOOTLOADER_ERROR("=== POINT OF NO REACH ===\n");
   return 0;
}
