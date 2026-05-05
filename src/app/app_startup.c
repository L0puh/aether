#include <aether.h>

extern u32 _estack;

extern int pull_entry(void);
extern int broadcast_entry(void);

__attribute__((section(".app_desc"), used))
const app_desc_t app_vectors = {
   .p_stack = (u32)&_estack,
   .entry_point = (u32)app_entry,
   .id = 1,
   .flags = 0
};

