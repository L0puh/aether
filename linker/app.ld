INCLUDE memory_map.ld

SECTIONS 
{
   .text :
   {
      _stext = .;
      . = ALIGN(4);
      KEEP(*(.vectors .vectors.*))
      *(.text*)
      *(.rodata*)
      . = ALIGN(4);
      _etext = .;
    } > app_rom

    .data :
    {
      . = ALIGN(4);
      _sdata = .;
      *(.data*)
      . = ALIGN(4);
      _edata = .;
    } > ram AT> app_rom

    .bss :
    {
      . = ALIGN(4);
      _sbss = .;
      *(.bss*)
      *(COMMON)
      . = ALIGN(4);
      _ebss = .;
    } > ram
 }

