#include "hv/hvapi_defs.h"
#include <aether.h>

extern u32 _stext;
extern u32 _estack;
extern u32 _etext;
extern u32 _sdata;
extern u32 _edata;
extern u32 _sbss;
extern u32 _ebss;

extern int bootloader_entry(void);

void reset_handler(void);
void default_handler(void);
void nmi_handler(void) __attribute__((weak, alias("default_handler")));
void debugmon_handler(void) __attribute__((weak, alias("default_handler")));
void pendsv_handler(void) __attribute__((weak, alias("default_handler")));
void systick_handler(void);

extern void usagefault_handler(void);
extern void hardfault_handler(void);
extern void memmanage_handler(void);
extern void busfault_handler(void);
extern void svc_handler(void);

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

/* in: r0 = id, r1 = permissions */
u32 svc_handler_c(frame_t *frame, u32 exc_ret)
{
   u8 *ret_addr = (u8*)frame->pc;
   u8 svc_num  = *(ret_addr-2);

   app_desc_t *desc = (app_desc_t*)APP_DESC_ADDR;
   if (desc->magic != APP_MAGIC) {
      ERROR_PRINT("no app found");
      //TODO: return other value
      return 0;
   }

   if (svc_num >= SVC_COUNT) {
      ERROR_PRINT("wrong svc number");
      frame->r0 = (u32)-1;
      return 0;
   }

   DEBUG_PRINT("stack: 0x%x, svc num: %d, ret addr: 0x%x, control=0x%x", frame, svc_num, ret_addr, get_control());
   
   if (frame->lr == 0xFFFFFFF9){
      ERROR_PRINT("SECURITY VIOLATION: app is privileged!");
      system_reset();
   }
   
   mark_app_alive();

   ret res = ERROR;
   switch(svc_num) {
      case SVC_REG_REQ:
         DEBUG_PRINT("calling region request: id=%d perms=0x%x", frame->r0, frame->r1);
         res = svc_region_request(desc, frame->r0, frame->r1); 
         break;
      case SVC_EXIT:
         return svc_app_exit(desc, frame, exc_ret);
      case SVC_WDT_KICK:
         watchdog_kick();
         break;
      default:
         DEBUG_PRINT("not implemented yet");
         break;
   }

   frame->r0 = res == SUCCESS ? 0 : -1;
   return 0;
}

void systick_handler(void)
{
   increment_system_ticks();
   tick_hook();
}

void default_handler(void)
{

   DEBUG_PRINT("default handler!!!");
   while(1) {
      cpu_wait_for_interrupt();
   }
}

__attribute__((noreturn))
void reset_handler(void)
{
   u32 data_sz = (u32)&_edata - (u32)&_sdata;
   u32 bss_sz = (u32)&_ebss  - (u32)&_sbss;

   if (data_sz > 0) {
       u8 *p_src  = (u8*)&_etext;
       u8 *p_dest = (u8*)&_sdata;
       for (u32 i = 0; i < data_sz; i++) {
           *p_dest++ = *p_src++;
       }
   }

   if (bss_sz > 0) {
       u8 *p_dest = (u8*)&_sbss;
       for (u32 i = 0; i < bss_sz; i++) {
           *p_dest++ = 0;
       }
   }

   bootloader_entry();
}
