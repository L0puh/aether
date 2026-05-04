MEMORY 
{
   boot_rom  (rx)  : ORIGIN = 0x08000000, LENGTH = 32K
   app_rom   (rx)  : ORIGIN = 0x08008000, LENGTH = 480K
   boot_ram  (rwx) : ORIGIN = 0x20000000, LENGTH = 16K
   app_ram   (rwx) : ORIGIN = 0x20004000, LENGTH = 48K
}

_boot_rom_start = ORIGIN(boot_rom);
_boot_rom_size  = LENGTH(boot_rom);
_app_rom_start  = ORIGIN(app_rom);
_app_rom_size   = LENGTH(app_rom);


STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x800; /* 2K*/
