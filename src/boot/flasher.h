#ifndef FLASHER_H
#define FLASHER_H

#include "defs.h"
#define FLASH_KEY1             0x45670123
#define FLASH_KEY2             0xCDEF89AB
#define FLASH_CR_LOCK          (1 << 7)
#define FLASH_SR_BUSY          (1 << 0)
#define FLASH_CR_PROGMODE      (1 << 0)
#define FLASH_PAGE_ERASE_MODE  (1 << 1)
#define FLASH_START_ERASE      (1 << 6)

void flash_app_by_uart(void);

void flash_unlock(void);
void flash_lock(void);

void flash_write_buffer(u32 addr, u8* data, u32 size);
void flash_erase_app_slot(u32 addr, u32 size);

//TODO: 
//void remove_app_by_addr(u32 addr, u32 size);
//void remove_app_by_id(u32 id);

#endif 
