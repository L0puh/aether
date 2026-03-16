#ifndef AETHER_H
#define AETHER_H

#include <inttypes.h>

typedef uint32_t u32;
typedef uint64_t u64;

#include "memory.h"
#include "debug.h"

typedef struct _device_vectors {
   
   void *p_stack;       

   void* pfn_reset_handler;
   void* pfn_nmi_handler;
   void* pfn_hardfault_handler;

} device_vectors_t;

#endif
