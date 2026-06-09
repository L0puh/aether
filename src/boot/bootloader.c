#include "boot/flasher.h"
#include "core/uart.h"
#include "defs.h"
#include <aether.h>

typedef void (*app_entry_t)(void);


static void run_app(app_desc_t* desc)
{
   app_entry_t entry;


#ifdef MEASURE_TIME
   BOOTLOADER_DEBUG("measuring time...\r\n");
   volatile u32 t_start, t_end;
   t_start = system_ticks_g;
#endif 

   entry = (app_entry_t)((u32)desc->entry | 1);
   BOOTLOADER_DEBUG("RUNNING APP\r\n");
   entry();
   BOOTLOADER_DEBUG("APP RETURNED\r\n");

#ifdef MEASURE_TIME
   t_end = system_ticks_g;
   BOOTLOADER_DEBUG("TIME ON EXECUTION: %d\r\n", t_end - t_start);
#endif 

}

void scan_for_apps()
{
   u32 start = START_APP_SLOT;
   u32 end   = END_APP_SLOT;

   for (u32 addr = start; addr < end; addr += 4)
   {
      app_desc_t* desc = (app_desc_t*)addr;

      if (desc->magic != APP_MAGIC)
         continue;

      BOOTLOADER_DEBUG(
            "APP FOUND @ 0x%08x size: %d\r\n",
            addr, desc->size
            );
      dump_memory((const void*) addr, desc->size, &uart_writef);

      BOOTLOADER_DEBUG(
            "ENTRY = 0x%08x\r\n",
            desc->entry
            );

      run_app(desc);
      wait_interrupt();
   }

   BOOTLOADER_DEBUG("NO MORE APPS\r\n");
   wait_interrupt();
}

//FIXME: this is damn insecure
void check_for_updates()
{

   if (!uart_wait_rx_ready(1000)){
      BOOTLOADER_DEBUG("timeout of waiting for updates is out\r\n");
      return;
   }

#ifdef MEASURE_TIME
   BOOTLOADER_DEBUG("measuring time...\r\n");
   volatile u32 t_start, t_end;
   t_start = system_ticks_g;
#endif 

   if (!cmd_update())
   {
      BOOTLOADER_DEBUG("wrong ACK recieved, skipping updates\r\n");
   }

#ifdef MEASURE_TIME
   t_end = system_ticks_g;
   BOOTLOADER_DEBUG("TIME ON EXECUTION: %d\r\n", t_end - t_start);
#endif 
}

__attribute__((noreturn))
int bootloader_entry()
{
   enable_irq();
   if (IS_ERROR(setup_system())) {
      toggle_debug_led();
   }

   BOOTLOADER_DEBUG("BOOTLOADER START\r\n");

   check_for_updates();

   while(true) {
      scan_for_apps();
   }
}
