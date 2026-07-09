
INCLUDE memory_map.ld

MEMORY
{
   app_flash (rx)  : ORIGIN = FLASH_APP_ORIGIN, LENGTH = FLASH_APP_LENGTH
   app_ram   (rwx) : ORIGIN = RAM_APP_ORIGIN,   LENGTH = RAM_APP_LENGTH
}

_estack = ORIGIN(app_ram) + LENGTH(app_ram);

ENTRY(reset_handler)

SECTIONS
{
   .app_desc FLASH_APP_ORIGIN : {
      _sapp_desc = .;
      . = . + APP_DESC_OFFSET - 1;
      BYTE(0xFF)
      _eapp_desc = .;
   } > app_flash

   .app_text : { 
      _stext = .;
      *(.text*) *(.rodata*) 
      _etext = .;
   } > app_flash 

   .app_data : { 
      _sdata = .;
      *(.data*) 
      _edata = .;
   } > app_ram AT > app_flash 

   .app_bss : { 
      _sbss = .;
      *(.bss*) 
      _ebss = .;
   } > app_ram 
}
