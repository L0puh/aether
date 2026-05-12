/* memory configuration - application is offset from bootloader */

MEMORY
{
   ROM (rx) : ORIGIN = 0x08002000, LENGTH = 8K
   RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 10208
}

_estack = ORIGIN(RAM) + LENGTH(RAM);
hv_api = 0x200027E0;

ENTRY(_start)

SECTIONS
{
    .app_desc : ALIGN(4)
    {
        KEEP(*(.app_desc))
    } > ROM

    .text : ALIGN(4)
    {
        _stext = .;
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > ROM

    .data : AT(_etext)
    {
        _sdata = .;
        *(.data)
        _edata = .;
    } > RAM

    .bss (NOLOAD) : ALIGN(4)
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM

    .heap (NOLOAD) : ALIGN(8)
    {
        _sheap = .;
        . = . + 0x200;
        _eheap = .;
    } > RAM

    .stack (NOLOAD) : ALIGN(8)
    {
        _sstack = .;
        . = . + 0x400;
        _estack = .;
    } > RAM
}

ASSERT((_etext - ORIGIN(ROM)) <= LENGTH(ROM), "application too large for slot!")
