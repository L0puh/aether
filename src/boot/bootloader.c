#include <aether.h>


void fetch_commands()
{
   if (!uart_rx_ready()){
      BOOTLOADER_DEBUG("timeout of waiting for updates is out\r\n");
      return;
   }

   u8 cmd = recv_cmd();
   if (!execute_cmd(cmd)){
      BOOTLOADER_DEBUG("failed to execute cmd!\r\n");
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

#ifndef MODE_AUTO
   
   while (true) {
      fetch_commands();
   }

#else 
  
   while(true) {
      cmd_scan();
   }

#endif 
}
