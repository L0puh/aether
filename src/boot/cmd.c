#include <aether.h>

/*
 * MESSAGE IN: [F][SIZE][ADDR][APP]
 *
 * @desc: 
 * app has key embedded into it, 
 * so we need to verify it with 
 * private key 
 *
 */

bool execute_cmd() {
   u8 cmd = uart_data();
   if (cmd == CMD_UPDATE)
   {
      BOOTLOADER_DEBUG("CMD UPDATE: %c\r\n", cmd);
      cmd_update();
      return true;
   } 

   if (cmd == CMD_WATCH) {
      BOOTLOADER_DEBUG("CMD WATCH: %c\r\n", cmd);
      return true;
   }

   if (cmd == CMD_SCAN) {
      BOOTLOADER_DEBUG("SCAN APPS: %c\r\n", cmd);
      cmd_scan();
      return true;
   }
   
   return false;
}

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

bool cmd_scan()
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
   }

   BOOTLOADER_DEBUG("NO MORE APPS\r\n");
   return true;
}

bool cmd_update(void)
{

   BOOTLOADER_DEBUG("flash command received!\r\n");

   static u8 buff[MAX_APP_SIZE + SIGNATURE_SIZE];
   u32 size, addr, app_size;

   uart_flush_rx();
   FLASHER_DEBUG("waiting for uart binary via UART...\r\n");

   size = uart_read_word();

   if (size < SIGNATURE_SIZE || size > MAX_APP_SIZE + SIGNATURE_SIZE) {
      FLASHER_DEBUG("invalid size: %lu\r\n", size);
      return false;
   }

   FLASHER_DEBUG("size received: %lu\r\n", size);
   
   addr = uart_read_word();
   if (addr < START_APP_SLOT || addr > END_APP_SLOT){
      FLASHER_DEBUG("invalid addr: 0x%x\r\n", addr);
      return false;;
   }

   FLASHER_DEBUG("addr received: 0x%x\r\n", addr);

   for (u32 i = 0; i < size; i++) {
      if (!uart_wait_rx_ready(FLASHER_WAIT_TIMEOUT)){
         FLASHER_DEBUG("receive data timeout!\r\n");
         flash_erase_app_slot(addr, size);
         mpu_enable();
         return false;;
      }
         
      buff[i] = uart_data();
   }
  
   FLASHER_DEBUG("data received!\r\n");
   
   app_size = size - SIGNATURE_SIZE;
   u8* app_data = buff;
   u8* sign = buff + app_size;

   FLASHER_DEBUG("data: 0x%x, sign: 0x%x!\r\n", buff, sign);

   if (!verify_app_buffer(app_data, app_size, sign)){
      FLASHER_ERROR("failed to flash app, security violation!\r\n");
      return false;
   }
   
   FLASHER_DEBUG("app is verified!\r\n");

   mpu_disable();
   app_desc_t* desc = (app_desc_t*)app_data;
   if (desc->magic == APP_MAGIC) {
      FLASHER_DEBUG("magic is found: entry: 0x%x stack: 0x%x, version: %d\r\n", desc->entry, desc->p_stack, desc->version);
      desc->size = app_size;
   } else {
      FLASHER_ERROR("magic value is wrong!\r\n");
      return false;
   }

   flash_erase_app_slot(addr, app_size);
   flash_write_buffer(addr, app_data, app_size);
 
   mpu_enable();

   FLASHER_DEBUG("flashing done!\r\n");
   return true;
}


