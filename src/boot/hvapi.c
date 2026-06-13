#include <aether.h>
#include <stdarg.h>

__attribute__((section(".hv_api"), used))
volatile hv_api_t hv_api;

void hv_api_init(void)
{
   hv_api.version = 0x1;
   hv_api.api_size = sizeof(hv_api_t);
   hv_api.delay_ms = delay_impl;
   hv_api.uart_read = uart_read_impl;
   hv_api.uart_write = uart_write_impl;
   hv_api.uart_writef = uart_writef_impl;
   hv_api.led_toggle = led_toggle_impl;
}

void delay_impl(u32 ms)
{
   u32 _lr;
   HV_GET_LR(_lr);
   if (!is_valid_app_call(_lr)){
      return;
   }

   systick_msec_delay(ms);
}


void uart_writef_impl(const char* str, ...){
   u32 _lr;
   HV_GET_LR(_lr);

   va_list args;
   va_start(args, str);
   
   if (!str) {
      return;
   }

   if (!is_valid_app_call(_lr)){
      return;
   }
   
   uart_writef(str, args);

   va_end(args);
}

void uart_write_impl(const char* str)
{
   u32 _lr;
   HV_GET_LR(_lr);
   
   if (!str) {
      return;
   }
   if (!is_valid_app_call(_lr)){
      return;
   }
   uart_write(str);
}

u8 uart_read_impl(void)
{
   //TODO
   
   u32 _lr;
   HV_GET_LR(_lr);
   if (!is_valid_app_call(_lr)){
      return 0;
   }
   return 0;
}

void led_toggle_impl(void) 
{
   u32 _lr;
   HV_GET_LR(_lr);
   if (!is_valid_app_call(_lr)){
      return;
   }
   toggle_debug_led();
}
