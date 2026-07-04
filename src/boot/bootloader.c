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

bool is_app_exists(app_desc_t* desc) {
   u32 start = START_APP_SLOT;
   u32 end   = END_APP_SLOT;

   for (u32 addr = start; addr < end; addr += 4)
   {
      desc = (app_desc_t*)addr;

      if (desc->magic != APP_MAGIC)
         continue;

      BOOTLOADER_DEBUG( "APP FOUND @ 0x%x [%d bytes]\r\n", addr, desc->size);
      dump_memory((const void*) addr, desc->size, &uart_writef);
      BOOTLOADER_DEBUG( "ENTRY = 0x%x\r\n", desc->entry);

      return true;
   }

   BOOTLOADER_DEBUG("APPS NOT FOUND\r\n");
   return false;
}

static void run_app(app_desc_t* desc)
{
   app_entry_t entry;
   
   desc->state.apps_start_ms = system_ticks_g;
   desc->state.last_check_ms = system_ticks_g;

   entry = (app_entry_t)((u32)desc->entry | 1);

   BOOTLOADER_DEBUG("RUNNING APP\r\n");
   entry();
   BOOTLOADER_DEBUG("APP RETURNED\r\n");

   running_app_g = NULL;
}

u32 recv_size(void) 
{
   u32 size;

   size = uart_read_word();

#ifdef FEATURE_SIGN_APP
   if (size < SIGNATURE_SIZE || size > MAX_APP_SIZE + SIGNATURE_SIZE) {
      FLASHER_DEBUG("invalid size: %lu\r\n", size);
      return 0;
   }
#else 
   if (size > MAX_APP_SIZE || size == 0) {
      FLASHER_DEBUG("invalid size: %lu\r\n", size);
      return 0;
   }
#endif 

   UART_PRINT("size received: %lu\r\n", size);
   return size;
}

u32 recv_addr(void) 
{
   u32 addr;
   addr = uart_read_word();
   if (addr < START_APP_SLOT || addr > END_APP_SLOT){
      FLASHER_DEBUG("invalid addr: 0x%x\r\n", addr);
      return 0;
   }

   UART_PRINT("addr received: 0x%x\r\n", addr);
   return addr;
}

bool update_app_desc(app_desc_t *desc, const u8* app_data, const u32 app_size) 
{
   desc = (app_desc_t*)app_data;
   if (desc->magic == APP_MAGIC) {
      UART_PRINT("magic is found: entry: 0x%x stack: 0x%x, version: %d\r\n", desc->entry, desc->p_stack, desc->version);
      desc->size = app_size;
   } else {
      UART_PRINT("magic value is wrong!\r\n");
      return false;
   }
   desc->state = (app_state_t){ .max_runtime_ms = MAX_RUNTIME_MS, 
                                .watchdog_timeout_ms = DEFAULT_CLOCK_TIMEOUT};

   return true;
}

bool fetch_app(app_desc_t *desc) 
{

   UART_PRINT("flash command received!\r\n");

   uart_flush_rx();
   u32 size, addr, app_size;
   static u8 buff[MAX_APP_SIZE];

   size = recv_size();
   if (size == 0) return false;

   addr = recv_addr();
   if (addr == 0) return false;

   for (u32 i = 0; i < size; i++) {
      if (!uart_wait_rx_ready(FLASHER_WAIT_TIMEOUT)){
         UART_PRINT("receive data timeout!\r\n");
         flash_erase_app_slot(addr, size);
         mpu_enable();
         return false;;
      }
         
      buff[i] = uart_data();
   }
  
   UART_PRINT("data received!\r\n");

#ifdef FEATURE_SIGN_APP 
   app_size = size - SIGNATURE_SIZE;
   u8* app_data = buff;
   u8* sign = buff + app_size;
   FLASHER_DEBUG("data: 0x%x, sign: 0x%x!\r\n", buff, sign);
   
   if (!verify_app_buffer(app_data, app_size, sign)){
      UART_PRINT("failed to flash app, security violation!\r\n");
      return false;
   }
   
   UART_PRINT("app is verified!\r\n");
#else
   app_size = size;
   u8* app_data = buff;
   UART_PRINT("data: 0x%x!\r\n", buff);
#endif 

   mpu_disable();

   if (!update_app_desc(desc, app_data, app_size)) {
      return false;
      mpu_enable();
   }

   flash_erase_app_slot(addr, app_size);
   flash_write_buffer(addr, app_data, app_size);
 
   mpu_enable();

   UART_PRINT("flashing done!\r\n");

   system_reset();

   return true;
}

__attribute__((noreturn))
int bootloader_entry()
{
   int ret;
   enable_irq();

   if (IS_ERROR(setup_system())) {
      toggle_debug_led();
   }

   BOOTLOADER_DEBUG("BOOTLOADER START\r\n");

   app_desc_t* desc = NULL;
   ret = is_app_exists(desc);
   while (!ret || desc == NULL) {
      ret = fetch_app(desc);
   }
   
   run_app(desc);

   BOOTLOADER_DEBUG("POINT OF NO RETURN\r\n");
}

