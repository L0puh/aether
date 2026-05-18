#include <aether.h>


void flash_write_buffer(u32 addr, u8* data, u32 size)
{
    flash_unlock();

    for (u32 i = 0; i < size; i += 2)
    {
        uint16_t halfword = data[i];

        if (i + 1 < size)
            halfword |= ((uint16_t)data[i + 1] << 8);

        u32 timeout = FLASHER_WAIT_TIMEOUT;

        while (FLASH->SR & FLASH_SR_BUSY) {
            if (--timeout == 0) {
                FLASHER_ERROR("BUSY timeout\r\n");
                flash_lock();
                return;
            }
        }

        FLASH->CR |= FLASH_CR_PROGMODE;

        *(volatile uint16_t*)(addr + i) = halfword;

        timeout = FLASHER_WAIT_TIMEOUT;

        while (FLASH->SR & FLASH_SR_BUSY) {
            if (--timeout == 0) {
                FLASHER_ERROR("WRITE timeout\r\n");
                flash_lock();
                return;
            }
        }

        FLASH->CR &= ~FLASH_CR_PROGMODE;

        FLASHER_DEBUG("write %lu ok\r\n", i / 2);
    }

    flash_lock();
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
   bool res;

   for (u8 i = 0; i < 4; i++){
      res = uart_wait_rx_ready(FLASHER_WAIT_TIMEOUT);
      if (!res) {
         FLASHER_DEBUG("recv_size timeout\r\n");
         return 0;
      }
      size |= (uart_data() << (i*8));
   }

   if (size == 0){
      FLASHER_DEBUG("size of app is zero\r\n");
   }
   
   return size;
}

void flash_app_by_uart(void)
{
   mpu_disable();
   static u8 buff[MAX_APP_SIZE];
   u32 addr = 0x08002000; //TODO: transmit addr via uart too
   u32 size;

   uart_flush_rx();
   FLASHER_DEBUG("waiting for uart binary via UART...\r\n");

   size = recv_size();

   if (size == 0 || size > MAX_APP_SIZE) {
      FLASHER_DEBUG("invalid size: %lu\r\n", size);
      return;
   }

   FLASHER_DEBUG("size received: %lu\r\n", size);

   for (u32 i = 0; i < size; i++) {
      if (!uart_wait_rx_ready(FLASHER_WAIT_TIMEOUT)){
         FLASHER_DEBUG("receive data timeout!\r\n");
         flash_erase_app_slot();
         mpu_enable();
         return;
      }
         
      buff[i] = uart_data();
   }
  
   FLASHER_DEBUG("data received! flashing...\r\n");
   
   flash_erase_app_slot();
   flash_write_buffer(addr, buff, size);

   FLASHER_DEBUG("flashing done!\r\n");
   mpu_enable();
   return;
}


void flash_erase_app_slot()
{
   flash_unlock();

   for (u32 addr = START_APP_SLOT; addr < END_APP_SLOT; addr += 0x400)
   {
      u32 timeout = FLASHER_WAIT_TIMEOUT;

      while((FLASH->SR & FLASH_SR_BUSY) && --timeout);
      if (timeout == 0) {
         FLASHER_ERROR("erase busy timeout!\r\n");
         flash_lock();
         return;
      }

      FLASH->CR |= FLASH_PAGE_ERASE_MODE;
      FLASH->AR = addr;
      FLASH->CR |= FLASH_START_ERASE;

      timeout = FLASHER_WAIT_TIMEOUT;
      while((FLASH->SR & FLASH_SR_BUSY) && --timeout);

      if (timeout == 0) {
         FLASHER_ERROR("erase timeout at 0x%08lX!\r\n", addr);
         FLASH->CR &= ~(FLASH_PAGE_ERASE_MODE | FLASH_START_ERASE);
         flash_lock();
         return;
      }

      FLASHER_ERROR("erase at 0x%08lX!\r\n", addr);
      FLASH->CR &= ~FLASH_PAGE_ERASE_MODE;
   }

   flash_lock();
}
