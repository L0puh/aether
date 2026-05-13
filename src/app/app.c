#include <aether.h>

#define HV_API_ADDR 0x20002400
volatile hv_api_t* hv = (volatile hv_api_t*)HV_API_ADDR;

void app_main(void);

__attribute__((section(".app_desc"), used))
const app_desc_t app_desc =
{
   .magic  = APP_MAGIC,
   .version = 0x0100,
   .entry = app_main,
   .crc = 0
};

void app_main(void)
{
   hv->uart_write( "HELLO FROM APP\r\n");

   hv->led_toggle();
   hv->delay_ms(100);


   hv->uart_write( "APP DONE\r\n");
}
