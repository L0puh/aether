INCLUDE memory_map.ld


ENTRY(reset_handler)

SECTIONS 
{
   /DISCARD/ : {
       *(.init_array)
       *(.fini_array)
       *(.eh_frame)
       *(.ARM.exidx)
       *(.ARM.extab)
   }
   .text :
   {
      _stext = .;
      . = ALIGN(4);
      KEEP(*(.vectors .vectors.*))
      *(.text*)
      *(.rodata*)
      . = ALIGN(4);
      _etext = .;
   } > rom

   .data : AT(_etext)
   {
      . = ALIGN(4);
      _sdata = .;
      *(.data)
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
   
   .heap (NOLOAD) : 
   {
      . = ALIGN(8);
      _sheap = .;
      . = . + HEAP_SIZE;
      . = ALIGN(8);
      _eheap = .;
   } > ram

   .stack (NOLOAD) :
   {
      . = ALIGN(8);
      . = ORIGIN(ram) + LENGTH(ram) - STACK_SIZE;
      _sstack = .;
      . = . + STACK_SIZE;
      . = ALIGN(8);
      _estack = .;
   } > ram

   .hv_api (NOLOAD) :
   {
      . = ALIGN(4);
      KEEP(*(.hv_api))
      . = ALIGN(4);
   } > hv 

   PROVIDE(hv_api_addr = ADDR(.hv_api));

}
