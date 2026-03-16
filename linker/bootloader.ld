INCLUDE memory_map.ld

ENTRY(reset_handler)

SECTIONS 
{
   .text :
   {
      . = ALIGN(4);
      _stext = .;
      *(.isr_vectors)
         *(.text*)
         *(.rodata*)
         . = ALIGN(4);
         _etext = .;
   } > boot_rom

   .data :
   {
      . = ALIGN(4);
      _sdata = .;
      *(.data)
      . = ALIGN(4);
      _edata = .;
   } > ram AT> boot_rom

   .bss : 
   {
      . = ALIGN(4);
      _sbss = .;
      *(.bss)
      . = ALIGN(4);
      _ebss = .;
   } > ram
}
