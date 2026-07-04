
INCLUDE memory_map.h

MEMORY
{
   app_flash (rx)  : ORIGIN = FLASH_APP_ORIGIN, LENGTH = FLASH_APP_LENGTH
   app_ram   (rwx) : ORIGIN = RAM_APP_ORIGIN,   LENGTH = RAM_APP_LENGTH
}

SECTIONS
{
   .app_vectors : { 
      KEEP(*(.app_vectors)) 
   } > FLASH_APP   

   . = FLASH_APP_ORIGIN + APP_DESC_OFFSET;

   .app_desc :
   {
      . = ALIGN(4);
      KEEP(*(.app_desc))
      . = ALIGN(4);
   } > FLASH_APP 

   .app_text : { 
      *(.text*) *(.rodata*) 
   } > FLASH_APP

   .app_data : { 
      *(.data*) 
   } > RAM_APP AT > FLASH_APP

   .app_bss : { 
      *(.bss*) 
   } > RAM_APP
}
