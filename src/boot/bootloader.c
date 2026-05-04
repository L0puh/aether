#include <aether.h>

__attribute__((naked))
static void jump_to_app(uint32_t pc, uint32_t sp) {
    __asm volatile(
        "msr msp, r1\n"
        "dsb\n"
        "isb\n"
        "bx  r0\n"
    );
}

void load_from_flash(u32 addr, u32 id)
{
    app_desc_t *desc = (app_desc_t*)addr;
    
    uint32_t pc = desc->entry_point;
    uint32_t sp = desc->p_stack;
    
    if ((pc & 0xFFE00000) != 0x08000000) {
        while(1);  
    }
    if ((sp & 0xFFF00000) != 0x20000000) {
        while(1); 
    }
    if (desc->id != id) {
        while(1);
    }
    
    jump_to_app(pc, sp);
}

int bootloader_entry() 
{
   u32 app_addr = (u32)&_app_rom_start;
   load_from_flash(app_addr, 1);

   BOOTLOADER_DEBUG("=== POINT OF NO REACH ===\n");
   return 0;
}
