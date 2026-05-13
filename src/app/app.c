#include <aether.h>

extern u32 _etext, _sdata, _edata, _sbss, _ebss, _estack;
extern void _start(void);

#define HV_API_ADDR 0x200027E0 // TODO: fetch addr from linker

volatile hv_api_t* hv_g = (volatile hv_api_t*)HV_API_ADDR;

__attribute__((section(".app_desc"), used)) 
const app_desc_t desc = {
   .magic = APP_MAGIC,
   .version = 0x0100,
   .p_stack = &_estack,
   .entry = _start,
   .crc = 0
};

void app_main(void)
{

   hv_g->led_toggle();
   hv_g->uart_write("HELLO FROM APP!\r\n");
}

void _start(void)
{
   u32 data_sz = (u32)&_edata - (u32)&_sdata;
   u8 *src = (u8*)&_etext;
   u8 *dst = (u8*)&_sdata;
   for (u32 i = 0; i < data_sz; i++) {
      *dst++ = *src++;
   }

   u32 bss_sz = (u32)&_ebss - (u32)&_sbss;
   dst = (u8*)&_sbss;
   for (u32 i = 0; i < bss_sz; i++) {
      *dst++ = 0;
   }

   app_main();
   while(1);
}
