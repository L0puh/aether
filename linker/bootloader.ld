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
   } > boot_rom

   .data : AT(_etext)
   {
      . = ALIGN(4);
      _sdata = .;
      *(.data)
      . = ALIGN(4);
      _edata = .;
   } > boot_ram 

   
   .bss (NOLOAD) :
   {
      . = ALIGN(4);
      _sbss = .;
      *(.bss*)
      *(COMMON)
      . = ALIGN(4);
      _ebss = .;
   } > boot_ram

   .stack :
   {
      . = ALIGN(8);
      _sstack = .;
      . = . + STACK_SIZE;
      . = ALIGN(8);
      _estack = .;
   } > boot_ram
}
