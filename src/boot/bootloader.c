#include <aether.h>

bool is_app_exists(app_desc_t* desc) {

   desc = (app_desc_t*) APP_DESC_ADDR;

   if (desc->magic == APP_MAGIC) {

      BOOTLOADER_DEBUG( "APP FOUND @ 0x%x [%d bytes]\r\n", APP_DESC_ADDR, desc->size);
      dump_memory((const void*) APP_DESC_ADDR, desc->size, &uart_writef);
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
   if (size > FLASH_APP_LENGTH || size == 0) {
      FLASHER_DEBUG("invalid size: %lu\r\n", size);
      return 0;
   }
#endif 

   UART_PRINT("size received: %lu\r\n", size);
   return size;
}

bool update_app_desc(app_desc_t *desc, const u32 app_size) 
{
   desc = (app_desc_t*)APP_DESC_ADDR;

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
   if (!uart_rx_ready() && (uart_data() != 'U')){
      UART_PRINT("failed to recv [sync] byte");
      return false;
   }

   UART_PRINT("flash command received!\r\n");

   uart_flush_rx();
   u32 size, addr;

   addr = FLASH_APP_ORIGIN;
   size = recv_size();
   if (size == 0) return false;
 

//TODO:
// update mpu settings for current memory map
// should i lock and unlock flash? if i use only app region 
// should i enable and disable mpu? for same reason
//

#ifdef FEATURE_SIGN_APP 
   //FIXME: should recv signature separately? 
   //i.e. sign = recv_sign(SIGNATURE_SIZE);
   //without buffer allocation (since memory unwise 
   //to do that lol)
   app_size = size - SIGNATURE_SIZE;
   u8* app_data = buff;
   u8* sign = buff + app_size;
   FLASHER_DEBUG("data: 0x%x, sign: 0x%x!\r\n", buff, sign);
   
   if (!verify_app_buffer(app_data, app_size, sign)){
      UART_PRINT("failed to flash app, security violation!\r\n");
      return false;
   }
   
   UART_PRINT("app is verified!\r\n");
#endif 

   mpu_disable();

   flash_erase_app_slot(addr, size);
   flash_write_from_uart(addr, size);
 
   if (!update_app_desc(desc, size)) {
      UART_PRINT("app is corrupted, aborting...\r\n");
      flash_erase_app_slot(addr, size);
   } else {
      UART_PRINT("flashing is done!\r\n");
   }

   mpu_enable();

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

