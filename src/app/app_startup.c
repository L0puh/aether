#include <aether.h>

extern u32 _stext;
extern u32 _estack;
extern u32 _etext;
extern u32 _sdata;
extern u32 _edata;
extern u32 _sbss;
extern u32 _ebss;

extern int app_entry(void);

__attribute__((section(".app_desc"), used))
const app_desc_t app_vectors = {
   .p_stack = (u32)&_estack,
   .entry_point = (u32)app_entry,
   .id = 1,
   .flags = 0
};

