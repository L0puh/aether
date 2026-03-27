#ifndef DEVICE_VECTORS_H
#define DEVICE_VECTORS_H

typedef struct _device_vectors {
   void *p_stack;                         // 0x00: initial stack pointer
   
   void (*pfn_reset_handler)(void);       // 0x04: reset handler
   void (*pfn_nmi_handler)(void);         // 0x08: nmi handler
   void (*pfn_hardfault_handler)(void);   // 0x0c: hard fault handler
   void (*pfn_memmanage_handler)(void);   // 0x10: memory management handler
   void (*pfn_busfault_handler)(void);    // 0x14: bus fault handler
   void (*pfn_usagefault_handler)(void);  // 0x18: usage fault handler
   void *reserved_0[4];                   // 0x1c-0x28: reserved
   void (*pfn_svc_handler)(void);         // 0x2c: svcall handler
   void (*pfn_debugmon_handler)(void);    // 0x30: debug monitor handler
   void *reserved_1;                      // 0x34: reserved
   void (*pfn_pendsv_handler)(void);      // 0x38: pendsv handler
   void (*pfn_systick_handler)(void);     // 0x3c: systick handler
   
   void (*pfn_irq_handlers[64])(void);   
} device_vectors_t;

#endif 
