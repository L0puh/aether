MEMORY 
{
   boot_rom  (rx)  : ORIGIN = 0x08000000, LENGTH = 0x00004000  
   app_rom   (rx)  : ORIGIN = 0x08004000, LENGTH = 0x0003C000  
   ram       (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00008000 
}

/* MEMORY LAYOUT:
 * booloader   - 16KB
 * application - 240KB
 * ram         - 32KB
 */

_boot_rom_start = ORIGIN(boot_rom);
_boot_rom_size  = LENGTH(boot_rom);
_app_rom_start  = ORIGIN(app_rom);
_app_rom_size   = LENGTH(app_rom);

_estack = ORIGIN(ram) + LENGTH(ram);

