#ifndef SYSTEM_H
#define SYSTEM_H


#include <defs.h>

typedef struct PACKED _app_manifest {
   u32 granted_periph_mask;
   u32 extra_ram;                
} app_manifest_t;

typedef struct PACKED _app_desc {
   u32 magic;
   u32 entry;
   u32 size;
   app_manifest_t manifset;
   u16 reserved;
   u8 version;
} app_desc_t;


void init_debug_led(void);
void toggle_debug_led(void);
ret  system_setup(void);
void led_blink(int count, int time);

#endif 
