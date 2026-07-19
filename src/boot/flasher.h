#ifndef FLASHER_H
#define FLASHER_H

#include "defs.h"

#define FLASH_KEY1             0x45670123
#define FLASH_KEY2             0xCDEF89AB
#define FLASH_CR_LOCK          (1 << 7)
#define FLASH_CR_PROGMODE      (1 << 0)
#define FLASH_PAGE_ERASE_MODE  (1 << 1)
#define FLASH_START_ERASE      (1 << 6)

#define FLASH_SR_BUSY          (1 << 0)
#define FLASH_SR_PGERR         (1 << 2)   //programming error
#define FLASH_SR_WRPRTERR      (1 << 4)   //write protection error

void flash_unlock(void);
void flash_lock(void);

void flash_erase_app_slot(u32 addr, u32 size);
ret flash_write_from_uart(u32 addr, u32 size);

void flash_disable_progmode(void);
void flash_enable_progmode(void);

#endif 
