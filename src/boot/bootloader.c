#include <aether.h>

#ifdef FEATURE_CRC_APP
ret verify_crc(app_desc_t* desc, u32 size)
{
    if (desc == NULL) {
        return NOT_FOUND;
    }

    uint16_t calc_crc = crc_calculate((const uint8_t*)FLASH_APP_ORIGIN, size, 
                                                offsetof(app_desc_t, crc16), 2);

    DEBUG_PRINT("CRC orig: 0x%04x and calculated: 0x%04x!", desc->crc16, calc_crc);
    return (calc_crc == desc->crc16) ? SUCCESS: ERROR;
}
#endif 

bool is_app_exists(app_desc_t** desc) {

   app_desc_t* ptr = (app_desc_t*) APP_DESC_ADDR;

   PLAIN_PRINT("APP: ");
   if (ptr->magic == APP_MAGIC) {

      PLAIN_PRINT( "FOUND @ 0x%x [%d bytes]\r\n", APP_DESC_ADDR, ptr->size);
      dump_memory((const void*) APP_DESC_ADDR, ptr->size, &uart_writef);
      DEBUG_PRINT( "ENTRY = 0x%xn", ((u32)ptr->entry | 1));

      *desc = ptr;
      return true;
      
   }

   PLAIN_PRINT("NOT FOUND\r\n");
   DEBUG_PRINT("[magic=0x%x size=%d]", ptr->magic, ptr->size);
   return false;
}

bool verify_privilege_dropped(const u32 entry_app)
{
   u32 addr = entry_app & ~1;
   u32 *sig = (u32*)(addr - 4);

   dump_memory((const void*)sig, 32, uart_writef);
   if (sig[0] != PRIV_DROP_SIGNATURE)
   {
      ERROR_PRINT("app does not have drop privilege!");
      DEBUG_PRINT("expected: 0x%x; got: 0x%x", PRIV_DROP_SIGNATURE, sig[0]);
      return false;
   }

   DEBUG_PRINT("drop privilege signature found");
   return true;
}
   
static ret run_app(app_desc_t* desc)
{
   if (desc == NULL) {
      ERROR_PRINT("app is null, something went wrong");
      return NOT_FOUND;
   }
   
   volatile u32 entry = (u32)desc->entry | 1;

   if (!verify_privilege_dropped(entry)) {
      ERROR_PRINT("privilege is not dropped, erasing this app!");
      flash_erase_app_slot(FLASH_APP_ORIGIN, FLASH_APP_LENGTH);
      return VIOLATION;
   }

   DEBUG_PRINT("enabling watchdog before running");
   watchdog_init(WATCHDOG_RELOAD_TIMEOUT);
   DEBUG_PRINT("RUNNING APP (0x%x entry)", entry);

   disable_irq();
   enter_app(APP_STACK_ADDR-8, entry);

   return SUCCESS; 
}

u32 recv_size(void) 
{
   u32 size;

   size = uart_read_word();

   if (size > FLASH_APP_LENGTH || size == 0) {
      ERROR_PRINT("invalid size: %lu", size);
      return 0;
   }

   UART_PRINT("size received: %lu", size);
   return size;
}

#ifdef FEATURE_CRC_APP
#endif 

ret fetch_app(void) 
{
   if (!uart_wait_rx_ready(FETCH_TIMEOUT_MS)) {
      UART_PRINT("timeout to recv sync byte");
      return TIMEOUT;
   }
   else if ((uart_data() != 'F')){
      UART_PRINT("failed to recv sync byte, wrong byte");
      return WRONG_DATA;
   }

   UART_PRINT("flash command received, chunk size: %d!", FLASH_CHUNK_SIZE);

   uart_flush_rx();
   u32 size, addr;

   addr = FLASH_APP_ORIGIN;
   size = recv_size();
   if (size == 0) return false;
 
   flash_erase_app_slot(addr, size);
   flash_write_from_uart(addr, size);
 
   dump_memory((const void*) addr, size, uart_writef);

   app_desc_t *desc = (app_desc_t*) APP_DESC_ADDR;
   if (desc->size != size || desc->magic != APP_MAGIC){
      UART_PRINT("corrupted app (%d bytes & 0x%x magic), aborting", desc->size, desc->magic);
      flash_erase_app_slot(addr, size);
      return WRONG_DATA;
   }
   

#ifdef FEATURE_CRC_APP
   if (IS_ERROR(verify_crc(desc, size))){
      ERROR_PRINT("failed to verify crc, aborting flashing");
      flash_erase_app_slot(addr, size);
      return VIOLATION;
   } 

   DEBUG_PRINT("crc is verified!");
#endif 
   
   UART_PRINT("flashing is done!");
   system_reset();

   return SUCCESS;
}

void bootloader_exit_hook(i32 code)
{
   DEBUG_PRINT("APP EXITED WITH CODE: %d", code);
   DEBUG_PRINT("msp = 0x%x psp=0x%x ctrl=0x%x", get_msp(), get_psp(), get_control());
   DEBUG_PRINT("sleeping for %d ms", FETCH_TIMEOUT_MS);
   systick_msec_delay(FETCH_TIMEOUT_MS);
}

__attribute__((noreturn))
int bootloader_entry()
{
   ret res; 
   if (!IS_ERROR(system_setup())) {
      led_blink(3, 100);
   }

   DEBUG_PRINT("BOOTLOADER START");

   check_reset_cause();

   app_desc_t* desc = NULL;

   while (!is_app_exists(&desc) && !fetch_app()) {
      DEBUG_PRINT("sleeping for 60ms before trying again...");
      systick_msec_delay(60000);
   }

   if (desc != NULL) {
      res = preinit_periph(desc->manifset);
      if (!IS_ERROR(res)){
         run_app(desc);
      } 
      
      ERROR_PRINT("failed to preinit peripherals");
   }

   DEBUG_PRINT("POINT OF NO RETURN");
}

