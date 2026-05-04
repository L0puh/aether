INCLUDE memory_map.ld

SECTIONS 
{
   .text :
   {
      . = ALIGN(4);
      _stext = .;
      KEEP(*(.app_desc))

      *(.text*)
      *(.rodata*)
      . = ALIGN(4);
      _etext = .;
    } > app_rom

    .data : AT (_etext)
    {
      . = ALIGN(4);
      _sdata = .;
      *(.data*)
      . = ALIGN(4);
      _edata = .;
    } > app_ram 

    .bss (NOLOAD) :
    {
      . = ALIGN(4);
      _sbss = .;
      *(.bss*)
      *(COMMON)
      . = ALIGN(4);
      _ebss = .;
    } > app_ram

    .stack :
    {
       . = ALIGN(8);
       _sstack = .;
       . = . + STACK_SIZE;
       . = ALIGN(8);
       _estack = .;
    } > app_ram
 }

