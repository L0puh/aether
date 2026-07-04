/*
 * bootloader linker script for stm32f103c6t6
 * applications are flashed via UART only 
 *
 */

INCLUDE memory_map.h

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
  } > FLASH_HV
  .hv_text    : { 
     *(.text*) *(.rodata*) 
  } > FLASH_HV
  .hv_data    : { 
     *(.data*) 
  } > RAM_HV AT > FLASH_HV
  .hv_bss     : { 
     *(.bss*) 
  } > RAM_HV
}

_hv_stack_top   = ORIGIN(RAM_HV) + LENGTH(RAM_HV);
_app_stack_top  = ORIGIN(RAM_APP) + LENGTH(RAM_APP);
