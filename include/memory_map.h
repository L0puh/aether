
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
#define FLASH_HV_LENGTH   0x4000UL      /* 16K */ 

#define FLASH_APP_ORIGIN  0x08008000UL
#define FLASH_APP_LENGTH  0x8000UL      /* 32K */

#define APP_DESC_OFFSET   0x20UL        /* 32B */

#define RAM_HV_ORIGIN     0x20000000UL
#define RAM_HV_LENGTH     0x1000UL      /* 4K */

#define RAM_GUARD_ORIGIN  0x20001000UL
#define RAM_GUARD_LENGTH  0x400UL       /* 1K */ 

#define RAM_APP_ORIGIN    0x20002000UL
#define RAM_APP_LENGTH    0x2000UL      /* 8K */ 

#endif
