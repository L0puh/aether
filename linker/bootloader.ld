/*
 * bootloader linker script for stm32f103c6t6
 * applications are executed as functions that return control
 */

MEMORY
{
   rom (rx)  : ORIGIN = 0x08000000, LENGTH = 32K
   ram (rwx) : ORIGIN = 0x20000000, LENGTH = 9K
   hv  (rw)  : ORIGIN = 0x20002400, LENGTH = 256
   stack (rw): ORIGIN = 0x20002500, LENGTH = 768
}

_estack = ORIGIN(stack) + LENGTH(stack);

ENTRY(reset_handler)

SECTIONS
{

      .text :
   {
       . = ALIGN(4);

       _stext = .;

       KEEP(*(.vectors))

       *(.text*)
       *(.rodata*)

       . = ALIGN(4);

       _etext = .;
   } > rom


   .data : AT(_etext)
   {
       . = ALIGN(4);

       _sdata = .;

       *(.data*)

       . = ALIGN(4);

       _edata = .;
   } > ram


   .bss (NOLOAD) :
   {
       . = ALIGN(4);

       _sbss = .;

       *(.bss*)
       *(COMMON)

       . = ALIGN(4);

       _ebss = .;
   } > ram

   .hv_api (NOLOAD) :
   {
      . = ALIGN(4);

      KEEP(*(.hv_api))

      . = ALIGN(4);
   } > hv
}
