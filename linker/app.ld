/* memory configuration - application is offset from bootloader */

MEMORY
{
   ROM (rx) : ORIGIN = 0x08002000, LENGTH = 8K
}

SECTIONS
{
   .app_desc :
   {
      . = ALIGN(4);

      KEEP(*(.app_desc))

      . = ALIGN(4);
   } > ROM

   .text :
   {
      . = ALIGN(4);

      *(.text*)
      *(.rodata*)

      . = ALIGN(4);
   } > ROM
}
