#include <aether.h>

extern void entry_point(void* pc, void* sp);

int main() {
    device_vectors_t *vectors;
    vectors = (device_vectors_t*) &_app_rom_start;
    
    entry_point(vectors->pfn_reset_handler, vectors->p_stack);
    
    BOOTLOADER_DEBUG("=== POINT OF NO REACH ===\n");
    return 0;
}
