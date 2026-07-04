/*
 * bootloader linker script for stm32f103c6t6
 * applications are flashed via UART only 
 *
 */

INCLUDE memory_map.ld

MEMORY
{
   hv_flash (rx)  : ORIGIN = FLASH_HV_ORIGIN, LENGTH = FLASH_HV_LENGTH 
   hv_ram   (rwx) : ORIGIN = RAM_HV_ORIGIN,   LENGTH = RAM_HV_LENGTH
}

ENTRY(reset_handler)

SECTIONS
{
  .hv_vectors : { 
     KEEP(*(.vectors)) 
  } > hv_flash 
  .hv_text    : { 
     _stext = .;
     *(.text*) *(.rodata*) 
     _etext = .;
  } > hv_flash 
  .hv_data    : { 
     _sdata = .;
     *(.data*) 
     _edata = .;
  } > hv_ram AT > hv_flash 
  .hv_bss     : { 
     _sbss = .;
     *(.bss*) 
     _ebss = .;
  } > hv_ram 
}

_estack = ORIGIN(hv_ram) + LENGTH(hv_ram);
