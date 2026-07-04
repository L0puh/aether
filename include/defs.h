#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>
#include <target.h>

typedef signed int  s32;
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

#define NULL 0
#define UNUSED(x)((void)(x))
#define USED(x)((void)(x))
#define IS_ERROR(ret) (ret > SUCCESS)
#define CLEAR_BITS(n, pos) (~(((1UL << (n)) - 1) << (pos)))

#define HV_GET_LR(lr_var) \
   __asm volatile ("mov %0, lr\n" : "=r" (lr_var))

#define MAX_BUFFER_SIZE 1024
#define SIGNATURE_SIZE  64
#ifdef FEATURE_SIGN_APP
#define MAX_APP_SIZE    (8192 + SIGNATURE_SIZE)
#else
#define MAX_APP_SIZE    8192
#endif 
#define MAX_HV_API_SIZE 256
#define START_APP_SLOT 0x08002000
#define END_APP_SLOT 0x08008000
#define FLASHER_WAIT_TIMEOUT 10000 //ms
#define TICK_HOOK_MS 100 
#define MAX_RUNTIME_MS 5000


#define CMD_PRINT(...)  uart_writef(__VA_ARGS__);

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

#define UART_PRINT(...)      uart_writef(__VA_ARGS__); // used for flashing via UART 

#endif 
