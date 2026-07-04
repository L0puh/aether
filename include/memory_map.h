
/*************************************
 *
 * this file includes settings for 
 * memory for both bootloader and 
 * apps regions 
 * 
 * see bootloader.ld and app.ld 
 *
 * NB: addr must be properly aligned 
 *
 ************************************/


#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#define FLASH_HV_ORIGIN   0x08000000UL
#define FLASH_HV_LENGTH   0x4000UL     /* 16K */

#define FLASH_APP_ORIGIN  0x08004000UL
#define FLASH_APP_LENGTH  0xC000UL     /* 48K */

#define APP_DESC_OFFSET   0x100UL      /* after vectors */

#define RAM_HV_ORIGIN     0x20000000UL
#define RAM_HV_LENGTH     0x1000UL     /* 4K */

#define RAM_GUARD_ORIGIN  0x20001000UL
#define RAM_GUARD_LENGTH  0x400UL      /* 1K */

#define RAM_APP_ORIGIN    0x20001400UL
#define RAM_APP_LENGTH    0x3C00UL     /* 15K */


#define APP_DESC_ADDR (FLASH_APP_ORIGIN + APP_DESC_OFFSET)

#endif
