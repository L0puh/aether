#ifndef MEMORY_H
#define MEMORY_H

#include <defs.h>

extern const u32 _boot_rom_start;
extern const u32 _boot_rom_size;

extern const u32 _puller_rom_start;
extern const u32 _puller_rom_size;
extern const u32 _puller_ram_start;
extern const u32 _puller_ram_size;
extern const u32 _puller_estack;

extern const u32 _broadcast_rom_start;
extern const u32 _broadcast_rom_size;
extern const u32 _broadcast_ram_start;
extern const u32 _broadcast_ram_size;
extern const u32 _broadcast_estack;

extern const u32 _flasher_rom_start;
extern const u32 _flasher_rom_size;
extern const u32 _flasher_ram_start;
extern const u32 _flasher_ram_size;
extern const u32 _flasher_estack;

#endif
