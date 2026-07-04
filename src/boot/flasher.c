#include <aether.h>


bool flash_write_from_uart(u32 addr, u32 size)
{
   u8 data;
   u32 timeout;

   flash_unlock();
   for (u32 i = 0; i < size; i++) 
   {
      if (!uart_wait_rx_ready(FLASHER_WAIT_TIMEOUT)) {
         UART_PRINT("recv data timeout\r\n");
         flash_erase_app_slot(addr, size);
         return false;
      }

      data = uart_data();
      timeout = FLASHER_WAIT_TIMEOUT;

      while (FLASH->SR & FLASH_SR_BUSY) {
         if (--timeout == 0) {
            FLASHER_ERROR("busy timeout\r\n");
            flash_lock();
            return false;
         }
      }

      FLASH->CR |= FLASH_CR_PROGMODE;

      *(volatile uint16_t*)(addr + i) = data;

      timeout = FLASHER_WAIT_TIMEOUT;

      while (FLASH->SR & FLASH_SR_BUSY) {
         if (--timeout == 0) {
            FLASHER_ERROR("write timeout\r\n");
            flash_lock();
            return false;
         }
      }

      FLASH->CR &= ~FLASH_CR_PROGMODE;

      FLASHER_DEBUG("write %lu/%lu at 0x%x ok\r\n", i, size, (addr+i));

   }
   flash_lock();

   return true;
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

void flash_erase_app_slot(u32 addr, u32 size)
{
   flash_unlock();

   if (addr < FLASH_APP_ORIGIN || addr > FLASH_APP_ORIGIN + FLASH_APP_LENGTH || size > FLASH_APP_LENGTH) {
      FLASHER_ERROR("invalid parameter, out of resources!\r\n");
      return;
   }

   u32 start_addr = addr;
   u32 end_addr = addr + size;

   for (u32 i = start_addr; i < end_addr; i += 0x400)
   {
      u32 timeout = FLASHER_WAIT_TIMEOUT;

      while((FLASH->SR & FLASH_SR_BUSY) && --timeout);
      if (timeout == 0) {
         FLASHER_ERROR("erase busy timeout!\r\n");
         flash_lock();
         return;
      }

      FLASH->CR |= FLASH_PAGE_ERASE_MODE;
      FLASH->AR = i;
      FLASH->CR |= FLASH_START_ERASE;

      timeout = FLASHER_WAIT_TIMEOUT;
      while((FLASH->SR & FLASH_SR_BUSY) && --timeout);

      //TODO: check errors 

      if (timeout == 0) {
         FLASHER_ERROR("erase timeout at 0x%x!\r\n", i);
         FLASH->CR &= ~(FLASH_PAGE_ERASE_MODE | FLASH_START_ERASE);
         flash_lock();
         return;
      }

      CMD_PRINT("erase at 0x%x!\r\n", i);
      FLASH->CR &= ~FLASH_PAGE_ERASE_MODE;
   }

   flash_lock();
}
