#include <aether.h>

__attribute__((section(".hv_api"), used))
volatile hv_api_t hv_api;

void hv_api_init(void)
{
   hv_api.version = 0x1;
   hv_api.api_size = sizeof(hv_api_t);
   hv_api.delay_ms = delay_impl;
   hv_api.uart_read = uart_read_impl;
   hv_api.uart_write = uart_write_impl;
   hv_api.led_toggle = led_toggle_impl;
}

void delay_impl(u32 ms)
{
   systick_msec_delay(ms);
}

void uart_write_impl(const char* str)
{
   if (!str) {
      return;
   }
   uart_write(str);
}

u8 uart_read_impl(void)
{
   return 0;
}

void led_toggle_impl(void) 
{
   toggle_degug_led();
}
