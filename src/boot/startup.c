#include <aether.h>

extern u32 _stext;
extern u32 _estack;
extern u32 _etext;
extern u32 _sdata;
extern u32 _edata;
extern u32 _sbss;
extern u32 _ebss;

int  main(void);
void reset_handler(void);
void default_handler(void);

void nmi_handler(void) __attribute__((weak, alias("default_handler")));
void hardfault_handler(void) __attribute__((weak, alias("default_handler")));
void memmanage_handler(void) __attribute__((weak, alias("default_handler")));
void busfault_handler(void) __attribute__((weak, alias("default_handler")));
void usagefault_handler(void) __attribute__((weak, alias("default_handler")));
void svc_handler(void) __attribute__((weak, alias("default_handler")));
void debugmon_handler(void) __attribute__((weak, alias("default_handler")));
void pendsv_handler(void) __attribute__((weak, alias("default_handler")));
void systick_handler(void) __attribute__((weak, alias("default_handler")));

__attribute__((section(".vectors"), used))
const device_vectors_t vector_table = {
    .p_stack = (void*)&_estack,
    
    .pfn_reset_handler = reset_handler,
    .pfn_nmi_handler = nmi_handler,
    .pfn_hardfault_handler = hardfault_handler,
    .pfn_memmanage_handler = memmanage_handler,
    .pfn_busfault_handler = busfault_handler,
    .pfn_usagefault_handler = usagefault_handler,
    
    .reserved_0 = {0, 0, 0, 0},
    
    .pfn_svc_handler = svc_handler,
    .pfn_debugmon_handler = debugmon_handler,
    .reserved_1 = 0,
    .pfn_pendsv_handler = pendsv_handler,
    .pfn_systick_handler = systick_handler,
    
    .pfn_irq_handlers = { [0 ... 63] = default_handler }
};


void default_handler(void)
{
   while(1);
}

void reset_handler(void)
{

   u32 data_sz = (u32)&_edata - (u32)&_sdata;
   u32 bss_sz  = (u32)&_ebss  - (u32)&_sbss;
 
   /* copy data from rom to ram */
   u32 *p_src  = (u32*)&_etext;
   u32 *p_dest = (u32*)&_sdata;
   for (u32 i = 0; i < data_sz; i++) {
      *p_dest++ = *p_src++;
   }

   p_dest = (u32*)&_sbss;
   for (u32 i = 0; i < bss_sz; i++) {
      *p_dest++ = 0;
   }

   p_src = (u32*) &_stext;

   main();
}
