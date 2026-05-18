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

bool cmd_update(void)
{
   u8 cmd = uart_data();
   if (cmd != CMD_UPDATE)
   {
      BOOTLOADER_DEBUG("recieved %c\r\n", cmd);
      return false;
   }

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
   flash_erase_app_slot(addr, app_size);
   flash_write_buffer(addr, app_data, app_size);
   mpu_enable();
   
   FLASHER_DEBUG("flashing done!\r\n");

   return true;
}
