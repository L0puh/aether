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

#define MAX_BUFFER_SIZE 1024
#define MAX_APP_SIZE    8192
#define SIGNATURE_SIZE  64
#define START_APP_SLOT 0x08002000
#define END_APP_SLOT 0x08008000
#define FLASHER_WAIT_TIMEOUT 10000 //ms

#define PACKED __attribute__((packed))
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

#endif 
