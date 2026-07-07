#ifndef HV_H
#define HV_H

#include "core/gpio.h"
#include "defs.h"
#include <stdbool.h>


typedef struct PACKED _app_manifest {
   u32 granted_periph_mask;
   u32 extra_ram;                //TODO: request for extra RAM
} app_manifest_t;

typedef struct PACKED _app_desc {
   u32 magic;
   u32 version;
   u32 entry;
   u32 size;
   u32 crc;
   app_manifest_t manifest;
   u32 reserved[2];
} app_desc_t;

typedef void (*hv_delay_ms_t)(u32 ms);
typedef void (*hv_uart_write_t)(const char* str);
typedef void (*hv_uart_writef_t)(const char* str, ...);
typedef void (*hv_led_toggle_t)(void);
typedef u8 (*hv_uart_read_t)(void);

typedef struct _hv_api {
   u32 version;
   u32 api_size;

   hv_delay_ms_t delay_ms;
   hv_uart_write_t uart_write;
   hv_uart_writef_t uart_writef;
   hv_uart_read_t uart_read;
   hv_led_toggle_t led_toggle;

} hv_api_t;


extern volatile hv_api_t hv_api;

bool is_valid_app_call(u32 lr);
void hv_tick_hook(void);
void init_debug_led(void);
void toggle_debug_led(void);
ret  system_setup(void);
void led_blink(int count, int time);


void dump_regs(void);

#endif 
