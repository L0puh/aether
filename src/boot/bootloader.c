#include "aether.h"

extern void entry_point(u32 pc, u32 sp);

int main() {
   u32 *app_code = (u32 *)_app_rom_start;
   u32 app_sp = app_code[0];
   u32 app_start = app_code[1];

   entry_point(app_start, app_sp);


   BOOTLOADER_DEBUG("=== POINT NO REACHED ===\n");
   return 0;
}
