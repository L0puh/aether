#include "boot/flasher.h"
#include "core/uart.h"
#include "defs.h"
#include <aether.h>

typedef void (*app_entry_t)(void);


static void run_app(app_desc_t* desc)
{
   app_entry_t entry;
   entry = (app_entry_t)((u32)desc->entry | 1);
   BOOTLOADER_DEBUG("RUNNING APP\r\n");
   entry();
   BOOTLOADER_DEBUG("APP RETURNED\r\n");
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
            "APP FOUND @ 0x%08x\r\n",
            addr
            );

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

   if (!cmd_update())
   {
      BOOTLOADER_DEBUG("wrong ACK recieved, skipping updates\r\n");
   }
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
