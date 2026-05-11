MEMORY 
{
   boot_rom  (rx)  : ORIGIN = 0x08000000, LENGTH = 32K
   boot_ram  (rwx) : ORIGIN = 0x20000000, LENGTH = 16K
}

INCLUDE module_memory_map.ld

_boot_rom_start = ORIGIN(boot_rom);
_boot_rom_size  = LENGTH(boot_rom);


