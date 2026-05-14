#include <aether.h>

void flash_write_buffer(u32 addr, u8* data, u32 size) 
{
   mpu_disable();
   flash_unlock();
   
   for (u32 i = 0; i < size; i+=4){
      u32 word = 0;

      for (int j = 0; j < 4 && i + j < size; j++){
         word |= ((u32) data [i + j]) << (j * 8);
      }

      while(FLASH->SR & FLASH_SR_BUSY);

      FLASH->CR |= FLASH_CR_PROGMODE;
      *(volatile u32*)(addr+i) = word;

      while(FLASH->SR & FLASH_SR_BUSY);

      FLASH->CR &= ~FLASH_SR_BUSY;
   }

   flash_unlock();
   mpu_enable();
}


void flash_unlock(void)
{
   if (FLASH->CR & FLASH_CR_LOCK){
      FLASH->KEYR = FLASH_KEY1;
      FLASH->KEYR = FLASH_KEY2;
   }
}

void flash_lock(void)
{
   FLASH->CR |= FLASH_CR_LOCK;
}

static u32 recv_size(void)
{
   u32 size = 0;

   for (u8 i = 0; i < 25; i+=8){
      while(!uart_rx_ready());
      size = (uart_data() << i);
   }

   if (size == 0){
      FLASHER_DEBUG("size of app is zero\r\n");
   }
   
   return size;
}



void flash_app_by_uart(void)
{
   u8 buff[MAX_BUFFER_SIZE];
   u32 addr = 0x08002000; //TODO: transmit it via uart too
   u32 size; 

   FLASHER_DEBUG("waiting for uart binary via UART...\r\n");

   size = recv_size();

   // FIXME: erase only one app
   flash_erase_app_slot();

   for (u32 i = 0; i < size; i += 256){
      u32 chunk = (size - i < 256) ? (size - i) : 256;

      for (u32 i = 0; i < chunk; i++){
         while (!uart_rx_ready());
         buff[i] = uart_data();
      }

      flash_write_buffer(addr + i, buff, chunk);
   }

   FLASHER_DEBUG("FLASHING DONE!\r\n");
}

void flash_erase_app_slot()
{
   mpu_disable();
   flash_unlock();

   for (u32 addr = START_APP_SLOT; addr < END_APP_SLOT; addr += 0x4)
   {
      FLASH->CR |= FLASH_PAGE_ERASE_MODE;
      FLASH->AR = addr;
      FLASH->CR |= FLASH_START_ERASE;

      while(FLASH->SR & FLASH_SR_BUSY);

      FLASH->CR &= ~FLASH_PAGE_ERASE_MODE;
   }

   mpu_enable();
   flash_lock();
}
