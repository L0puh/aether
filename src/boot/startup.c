#include <stdint.h>

typedef uint32_t u32;

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


__attribute__((section(".isr_vectors")))  
u32 vector_tbl[] = {
   (u32)&_estack,           
   (u32)reset_handler,      
   (u32)nmi_handler,        
   (u32)hardfault_handler,  
   (u32)memmanage_handler,  
   (u32)busfault_handler,   
   (u32)usagefault_handler, 
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

   main();
}
