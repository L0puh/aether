#include <aether.h>

void hv_tick_hook(void) 
{
   app_desc_t* running_app = NULL;
   //TODO get_app(running_app)
   
   app_state_t state = running_app->state;
   u32 app_age = system_ticks_g - state.apps_start_ms;
   if (state.max_runtime_ms && app_age >= state.max_runtime_ms) {
      BOOTLOADER_ERROR("RUNTIME APP TIMEOUT!\r\n");
      //TODO: kill app
   }

   //TODO: also check stack canary and watchdog 
}


inline bool is_valid_app_call(u32 _lr)
{

#ifdef _DEBUG
   DEBUG_PRINT("\r\n DUMP OF REGISTERS \r\n");
   dump_regs();
   DEBUG_PRINT("\r\n FINISH DUMP \r\n\r\n");
#endif 
   UNUSED(_lr);
// FIXME: 
// if (_lr < FLASH_APP_ORIGIN || _lr > END_APP_SLOT){
//     return false;
//  }

   /* for (u32 addr = START_APP_SLOT; addr < END_APP_SLOT; addr += 4){ */
   /*    app_desc_t* desc = (app_desc_t*) addr; */
   /*    if (desc->magic != APP_MAGIC) { */
   /*       continue; */
   /*    } */
      
   /*    if (_lr > addr && _lr < (addr + desc->size)){ */
   /*       DEBUG_PRINT(" (hv) found app @ 0x%x with LR=0x%x\r\n", addr, _lr); */
   /*       return true; */
   /*    } */
   /* } */

   return true;
}


inline void dump_regs(void)
{
   u32 ret_addr;
   __asm volatile (
         "mov %0, lr\n"
         : "=r" (ret_addr)
         );

   u32 current_pc;
   __asm volatile (
         "mov %0, pc\n"
         : "=r" (current_pc)
         );

   u32 sp;
   __asm volatile (
         "mov %0, sp\n"
         : "=r" (sp)
         );

   DEBUG_PRINT("lr: 0x%x, pc: 0x%x, sp: 0x%x\r\n", 
         ret_addr, current_pc, sp);
   DEBUG_PRINT("app region: 0x%x - 0x%x\r\n", 
         FLASH_APP_ORIGIN, FLASH_APP_ORIGIN + FLASH_APP_LENGTH);
   DEBUG_PRINT("hv region: 0x%x - 0x%x\r\n", 
         FLASH_HV_ORIGIN, FLASH_HV_ORIGIN + FLASH_HV_LENGTH);
   DEBUG_PRINT("app desc @ 0x%x\r\n", 
         APP_DESC_ADDR);
   DEBUG_PRINT("guard @ 0x%x\r\n", 
         RAM_GUARD_ORIGIN);

   dump_memory((void*)ret_addr, 64, &uart_writef);
}

void init_debug_led(void)
{
   rcc_enable_clock_pin(GPIOC);

   GPIOC->CRH &= CLEAR_BITS(4, 20);
   GPIOC->CRH |= OUTPUT_50MHZ | GENERAL_PUSHPULL;
}

void toggle_debug_led(void)
{
   GPIOC->ODR ^= (1 << 13);
}

void led_blink(int count, int time)
{
   while (count--)
   {
      toggle_debug_led();

      for (volatile int i = 0; i < time; i++);

      toggle_debug_led();

      for (volatile int i = 0; i < time; i++);
   }
}

ret setup_system(void)
{
   ret status;

#ifdef SYSTEM_CLOCK_72Mhz
   status = set_system_clock_72Mhz();
#else
   status = set_system_clock_25Mhz();
#endif

   systick_init();
   init_debug_led();

   status = rcc_init_uart_clock(
         USART1,
         GPIOA, 9,
         GPIOA, 10
         );

   if (IS_ERROR(status))
      return status;

   uart_init(USART1, 0);

   flash_lock();
   mpu_init();
   mpu_enable();
   BOOTLOADER_DEBUG("SYSTEM OK\r\n");

   return SUCCESS;
}


