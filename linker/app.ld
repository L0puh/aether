
INCLUDE memory_map.ld

MEMORY
{
   app_flash (rx)  : ORIGIN = FLASH_APP_ORIGIN, LENGTH = FLASH_APP_LENGTH
   app_ram   (rwx) : ORIGIN = RAM_APP_ORIGIN,   LENGTH = RAM_APP_LENGTH
}

_estack = ORIGIN(app_ram) + LENGTH(app_ram);

SECTIONS
{
   .app_desc :
   {
      . = ALIGN(4);
      KEEP(*(.app_desc))
      . = ALIGN(4);
   } > app_flash

   .app_vectors : { 
      KEEP(*(.app_vectors)) 
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
