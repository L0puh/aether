#ifndef HV_H
#define HV_H

#include "defs.h"

#define MAX_MODULES     16
#define MODULE_TAG_LEN 8

typedef struct __attribute__((packed)) {
   u32 r4, r5, r6, r7, r8, r9, r10, r11;
   u32 psp;    // process stack pointer
   u32 lr;     // exec return
   u32 pc;     // program counter
   u32 xpsr;   // program status reg
} context_t;

typedef struct {
   u32 flash_start;
   u32 flash_size;

   u32 ram_start;
   u32 ram_size;

   //optional
   u32 periph_start;
   u32 periph_size;
} memory_t;

typedef struct _app_desc {
   uint32_t p_stack;
   uint32_t entry_point;
   uint32_t id;
   uint32_t flags;
} app_desc_t;

typedef enum {
   STATE_OFF,
   STATE_RUNNING,
   STATE_WAITING,
   STATE_TERMINATED
} state_e;

typedef struct {
   u8 id;
   char tag[MODULE_TAG_LEN];
   state_e state;
   memory_t memory;
   context_t context;
   u32 timeslice_remain;
   u32 time_total;
} module_t;

// flags
#define FLAG_UART    (1 << 0)
#define FLAG_TIMER   (1 << 1)
//...

#endif 
