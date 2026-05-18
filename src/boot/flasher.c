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

        FLASHER_DEBUG("write %lu/%lu at 0x%x ok\r\n", i/2, size/i, (addr+i));
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

void flash_erase_app_slot(u32 addr, u32 size)
{
   flash_unlock();

   if (addr < START_APP_SLOT || addr > END_APP_SLOT || addr + size > END_APP_SLOT) {
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

      if (timeout == 0) {
         FLASHER_ERROR("erase timeout at 0x%08lX!\r\n", i);
         FLASH->CR &= ~(FLASH_PAGE_ERASE_MODE | FLASH_START_ERASE);
         flash_lock();
         return;
      }

      FLASHER_ERROR("erase at 0x%08lX!\r\n", i);
      FLASH->CR &= ~FLASH_PAGE_ERASE_MODE;
   }

   flash_lock();
}
