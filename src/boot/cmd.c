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
      CMD_PRINT("CMD UPDATE: %c\r\n", cmd);
      uart_flush_rx();
      cmd_update();
      return true;
   } 

   if (cmd == CMD_WATCH) {
      CMD_PRINT("CMD WATCH: %c\r\n", cmd);
      return true;
   }

   if (cmd == CMD_SCAN) {
      CMD_PRINT("SCAN APPS: %c\r\n", cmd);
      uart_flush_rx();
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

      BOOTLOADER_DEBUG( "APP FOUND @ 0x%x [%d bytes]\r\n", addr, desc->size);
      dump_memory((const void*) addr, desc->size, &uart_writef);
      BOOTLOADER_DEBUG( "ENTRY = 0x%x\r\n", desc->entry);

      run_app(desc);
   }

   CMD_PRINT("NO MORE APPS\r\n");
   return true;
}

bool cmd_update(void)
{

   CMD_PRINT("flash command received!\r\n");

#ifdef FEATURE_SIGN_APP
   static u8 buff[MAX_APP_SIZE + SIGNATURE_SIZE];
#else
   static u8 buff[MAX_APP_SIZE];
#endif 

   u32 size, addr, app_size;

   uart_flush_rx();

   CMD_PRINT("waiting for uart binary via UART...\r\n");

   size = uart_read_word();

#ifdef FEATURE_SIGN_APP
   if (size < SIGNATURE_SIZE || size > MAX_APP_SIZE + SIGNATURE_SIZE) {
      FLASHER_DEBUG("invalid size: %lu\r\n", size);
      return false;
   }
#else 
   if (size > MAX_APP_SIZE || size == 0) {
      FLASHER_DEBUG("invalid size: %lu\r\n", size);
      return false;
   }
#endif 

   CMD_PRINT("size received: %lu\r\n", size);
   
   addr = uart_read_word();
   if (addr < START_APP_SLOT || addr > END_APP_SLOT){
      FLASHER_DEBUG("invalid addr: 0x%x\r\n", addr);
      return false;;
   }

   CMD_PRINT("addr received: 0x%x\r\n", addr);

   for (u32 i = 0; i < size; i++) {
      if (!uart_wait_rx_ready(FLASHER_WAIT_TIMEOUT)){
         CMD_PRINT("receive data timeout!\r\n");
         flash_erase_app_slot(addr, size);
         mpu_enable();
         return false;;
      }
         
      buff[i] = uart_data();
   }
  
   CMD_PRINT("data received!\r\n");

#ifdef FEATURE_SIGN_APP 
   app_size = size - SIGNATURE_SIZE;
   u8* app_data = buff;
   u8* sign = buff + app_size;
   FLASHER_DEBUG("data: 0x%x, sign: 0x%x!\r\n", buff, sign);
   
   if (!verify_app_buffer(app_data, app_size, sign)){
      CMD_PRINT("failed to flash app, security violation!\r\n");
      return false;
   }
   
   CMD_PRINT("app is verified!\r\n");
#else
   app_size = size;
   u8* app_data = buff;
   CMD_PRINT("data: 0x%x!\r\n", buff);
#endif 

   mpu_disable();
   app_desc_t* desc = (app_desc_t*)app_data;
   if (desc->magic == APP_MAGIC) {
      CMD_PRINT("magic is found: entry: 0x%x stack: 0x%x, version: %d\r\n", desc->entry, desc->p_stack, desc->version);
      desc->size = app_size;
   } else {
      CMD_PRINT("magic value is wrong!\r\n");
      return false;
   }

   flash_erase_app_slot(addr, app_size);
   flash_write_buffer(addr, app_data, app_size);
 
   mpu_enable();

   CMD_PRINT("flashing done!\r\n");
   system_reset();
   return true;
}


