#include "core/gpio.h"
#include <aether.h>


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
   
static void run_app(app_desc_t* desc)
{
   if (desc == NULL) {
      ERROR_PRINT("app is null, something went wrong");
      return;
   }
   
   volatile u32 entry = (u32)desc->entry | 1;

   if (!verify_privilege_dropped(entry)) {
      ERROR_PRINT("privilege is not dropped, erasing this app!");
#ifndef _DEBUG
      flash_erase_app_slot(FLASH_APP_ORIGIN, FLASH_APP_LENGTH);
#endif 
      return;
   }

   DEBUG_PRINT("RUNNING APP (0x%x entry)", entry);

   disable_irq();
   enter_app(APP_STACK_ADDR-8, entry);
   
}

u32 recv_size(void) 
{
   u32 size;

   size = uart_read_word();

#ifdef FEATURE_SIGN_APP
   if (size < SIGNATURE_SIZE || size > MAX_APP_SIZE + SIGNATURE_SIZE) {
      FLASHER_DEBUG("invalid size: %lu", size);
      return 0;
   }
#else 
   if (size > FLASH_APP_LENGTH || size == 0) {
      ERROR_PRINT("invalid size: %lu", size);
      return 0;
   }
#endif 

   UART_PRINT("size received: %lu", size);
   return size;
}

bool fetch_app(void) 
{
   if (!uart_wait_rx_ready(FETCH_TIMEOUT_MS)) {
      UART_PRINT("timeout to recv sync byte");
      return false;
   }
   else if ((uart_data() != 'F')){
      UART_PRINT("failed to recv sync byte, wrong byte");
      return false;
   }

   UART_PRINT("flash command received, chunk size: %d!", FLASH_CHUNK_SIZE);

   uart_flush_rx();
   u32 size, addr;

   addr = FLASH_APP_ORIGIN;
   size = recv_size();
   if (size == 0) return false;
 

#ifdef FEATURE_SIGN_APP 
   //FIXME: should recv signature separately? 
   //i.e. sign = recv_sign(SIGNATURE_SIZE);
   //without buffer allocation (since memory unwise 
   //to do that lol)
   app_size = size - SIGNATURE_SIZE;
   u8* app_data = buff;
   u8* sign = buff + app_size;
   FLASHER_DEBUG("data: 0x%x, sign: 0x%x!", buff, sign);
   
   if (!verify_app_buffer(app_data, app_size, sign)){
      UART_PRINT("failed to flash app, security violation!");
      return false;
   }
   
   UART_PRINT("app is verified!");
#endif 

   flash_erase_app_slot(addr, size);
   flash_write_from_uart(addr, size);
 
   dump_memory((const void*) addr, size, uart_writef);

   app_desc_t *desc = (app_desc_t*) APP_DESC_ADDR;
   if (desc->size != size || desc->magic != APP_MAGIC){
      UART_PRINT("corrupted app (%d bytes & 0x%x magic), aborting", desc->size, desc->magic);
      flash_erase_app_slot(addr, size);
   }
   
   UART_PRINT("flashing is done!");

   system_reset();

   return true;
}

void bootloader_exit_hook(i32 code)
{
   DEBUG_PRINT("APP EXITED WITH CODE: %d", code);

   u32 msp = get_msp();
   u32 psp = get_psp();
   u32 ctrl = get_control();

   DEBUG_PRINT("msp = 0x%x psp=0x%x ctrl=0x%x", msp, psp, ctrl);
}

__attribute__((noreturn))
int bootloader_entry()
{
   int ret;
   if (!IS_ERROR(system_setup())) {
      led_blink(3, 100);
   }

   DEBUG_PRINT("BOOTLOADER START");
   check_reset_cause();

   app_desc_t* desc = NULL;

   while (!(ret = is_app_exists(&desc))) {
      ret = fetch_app();
      if (ret) { break; }
      DEBUG_PRINT("sleeping for 60ms before trying again...");
      systick_msec_delay(60000);
   }

   if (desc == NULL) {
      ERROR_PRINT("failed to fetch app (NULL)");
   } else { 
      ret = preinit_periph(desc->manifset);
      if (IS_ERROR(ret)){
         ERROR_PRINT("failed to preinit peripherals, aborting");
      } else {
         run_app(desc);
      }
   }

   DEBUG_PRINT("POINT OF NO RETURN");
}

