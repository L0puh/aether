/*
 * bootloader linker script for stm32f103c6t6
 * applications are executed as functions that return control
 */

MEMORY
{
   rom   (rx)  : ORIGIN = 0x08000000,  LENGTH = 32K
   ram   (rwx) : ORIGIN = 0x20000000,  LENGTH = 10208 /*10K - 32bytes */
   hv    (rw)  : ORIGIN = 0x200027E0,  LENGTH = 32 
}

_rom_start = ORIGIN(rom);
_rom_size  = LENGTH(rom);
_ram_start = ORIGIN(ram);
_ram_size  = LENGTH(ram);

STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x800;
HEAP_SIZE  = DEFINED(__heap_size__)  ? __heap_size__  : 0x400;

_estack = ORIGIN(ram) + LENGTH(ram);
