#include <aether.h>

bool wait_flash_busy(u64 timeout)
{
   while (FLASH->SR & FLASH_SR_BUSY) {
      if (--timeout == 0) {
         UART_PRINT("busy timeout");
         return false;
      }
   }
   return true;
}

void flash_disable_progmode(void) {
   FLASH->CR &= ~FLASH_CR_PROGMODE;
   return;
}

void flash_enable_progmode(void) {
   FLASH->CR |= FLASH_CR_PROGMODE;
   return;
}

ret flash_write_from_buffer(u32 addr, const u8* data, u32 size)
{

   ret res = SUCCESS;

   flash_enable_progmode();
   for (u32 i = 0; i < size; i += 2){
      u16 half = data[i];
      if (i + 1 < size) {
         half |= (u16) data[i+1] << 8;
      } else {
         half |= 0xFF00;
      }

      if (!wait_flash_busy(FLASHER_WAIT_TIMEOUT)) {
         res = TIMEOUT;
         break;
      }
      
      *(volatile u16*)(addr + i) = half;
      
      if (!wait_flash_busy(FLASHER_WAIT_TIMEOUT)) {
         res = TIMEOUT;
         break;
      }

      if (FLASH->SR & (FLASH_SR_PGERR | FLASH_SR_WRPRTERR)) {
         UART_PRINT("flash error @ 0x%x, SR=0x%x", addr+i, FLASH->SR);
         FLASH->SR |= (FLASH_SR_PGERR | FLASH_SR_WRPRTERR);
         res = ERROR; 
         break; 
      }
   }
   
   flash_disable_progmode();
   return res;
}

ret flash_write_from_uart(u32 addr, u32 size)
{

   static u8 rx_chunk[FLASH_CHUNK_SIZE];
   u32 written = 0;
   ret res = SUCCESS;
   flash_unlock();

   while (written < size)
   {
      u32 rem = size - written;
      u32 chunk_len = (rem < FLASH_CHUNK_SIZE) ? rem : FLASH_CHUNK_SIZE;
      res = uart_read_buffer(rx_chunk, chunk_len);
      if (res != SUCCESS) {
         break;
      }

      res = flash_write_from_buffer(addr + written, rx_chunk, chunk_len);
      if (res != SUCCESS) {
         break;
      }
      
      written += chunk_len;
      UART_PRINT("write %lu/%lu @ 0x%x ok", written, size, (addr+written-chunk_len));
   }

   flash_lock();

   return res;
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
      UART_PRINT("invalid parameter, out of resources!");
      return;
   }

   u32 start_addr = addr;
   u32 end_addr = addr + size;

   for (u32 i = start_addr; i < end_addr; i += 0x400)
   {
      u32 timeout = FLASHER_WAIT_TIMEOUT;

      while((FLASH->SR & FLASH_SR_BUSY) && --timeout);
      if (timeout == 0) {
         UART_PRINT("erase busy timeout!");
         flash_lock();
         return;
      }

      FLASH->CR |= FLASH_PAGE_ERASE_MODE;
      FLASH->AR = i;
      FLASH->CR |= FLASH_START_ERASE;

      timeout = FLASHER_WAIT_TIMEOUT;
      while((FLASH->SR & FLASH_SR_BUSY) && --timeout);


      if (timeout == 0) {
         UART_PRINT("erase timeout at 0x%x!", i);
         FLASH->CR &= ~(FLASH_PAGE_ERASE_MODE | FLASH_START_ERASE);
         flash_lock();
         return;
      }

      if (FLASH->SR & FLASH_SR_PGERR) {
         UART_PRINT("erase programming error at 0x%x\r\n", i);
         FLASH->SR |= FLASH_SR_PGERR;
         FLASH->CR &= ~(FLASH_PAGE_ERASE_MODE | FLASH_START_ERASE);
         flash_lock();
         return;
      }

      UART_PRINT("erase at 0x%x!", i);
      FLASH->CR &= ~FLASH_PAGE_ERASE_MODE;
   }

   flash_lock();
   UART_PRINT("erase done!");
}
