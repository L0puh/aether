#ifndef HV_H
#define HV_H

#include "defs.h"
#include <stdbool.h>

typedef struct PACKED _app_desc {
   u32 magic;
   u32 version;
   void *p_stack;
   void (*entry)(void);
   u32 size;
} app_desc_t;

typedef void (*hv_delay_ms_t)(u32 ms);
typedef void (*hv_uart_write_t)(const char* str);
typedef void (*hv_led_toggle_t)(void);
typedef u8 (*hv_uart_read_t)(void);

typedef struct _hv_api {
   u32 version;
   u32 api_size;

   hv_delay_ms_t delay_ms;
   hv_uart_write_t uart_write;
   hv_uart_read_t uart_read;
   hv_led_toggle_t led_toggle;

} hv_api_t;

extern volatile hv_api_t hv_api;

bool is_valid_app_call(void);
void init_debug_led(void);
void toggle_debug_led(void);
ret setup_system(void);
void led_blink(int count, int time);

void delay_impl(u32 ms);
void uart_write_impl(const char* str);
u8   uart_read_impl(void);
void led_toggle_impl(void);
void hv_api_init(void);


#endif 
