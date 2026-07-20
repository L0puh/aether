#ifndef DEFS_H
#define DEFS_H

#include "target.h"
#include "hv/hvapi_defs.h"

#ifndef __INT32_C
#if defined(__LP64__) || defined(_LP64)
#define __INT32_C(c)  c   
#else
#define __INT32_C(c)  c ## L  
#endif
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef signed int  s32;
typedef int32_t     i32;
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

#ifndef NULL
#define NULL ((void*)0)
#endif 
#define UNUSED(x)((void)(x))
#define USED(x)((void)(x))
#define IS_ERROR(ret) (ret > SUCCESS)
#define CLEAR_BITS(n, pos) (~(((1UL << (n)) - 1) << (pos)))

#define HV_GET_LR(lr_var) \
   __asm volatile ("mov %0, lr\n" : "=r" (lr_var))

#define MAX_BUFFER_SIZE 256
#define SIGNATURE_SIZE  64

// TODO: refactor??
#ifdef FEATURE_SIGN_APP
#define MAX_APP_SIZE    (8192 + SIGNATURE_SIZE)
#else
#define MAX_APP_SIZE    8192
#endif 

#define FLASHER_WAIT_TIMEOUT 10000 //ms
#define TICK_HOOK_MS 30000 // 30 sec 
#define MAX_RUNTIME_MS 5000
#define FETCH_TIMEOUT_MS 60000
#define UART_TIMEOUT_MS 5000
#define APP_LIVENESS_TIMEOUT_MS 2000
#define WATCHDOG_RELOAD_TIMEOUT 5000 // 5 secs

#define PACKED         __attribute__((packed))
#define ALWAYS_INLINE  __attribute__((always_inline))
typedef enum _ret_codes {
   SUCCESS,
   NOT_FOUND,
   VIOLATION,
   ACCESS_DENIED,
   INVALID_PARAMETER,
   TIMEOUT,
   BUSY,
   ERROR,
   WRONG_DATA,
   SKIP_EXECUTION
} ret;

#define APP_MAGIC             0xDEADBEEF
#define PRIV_DROP_SIGNATURE   0x0DEAFBEE


#define APP_DESC_ADDR  (FLASH_APP_ORIGIN)
#define APP_STACK_ADDR (RAM_APP_ORIGIN + RAM_APP_LENGTH)

#define FLASH_CHUNK_SIZE (FLASH_HV_LENGTH/sizeof(u64))

typedef int (*app_entry_t)(void);
#define UART_PRINT(...)      uart_writef(__VA_ARGS__); uart_writef("\r\n") // used for flashing via UART 

typedef struct {
   u32 r0, r1, r2, r3, r12, lr, pc, xpsr;
} frame_t;

#endif 
