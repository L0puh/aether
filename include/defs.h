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
#define IS_ERROR(ret) (ret > SUCCESS)
#define CLEAR_BITS(n, pos) (~(((1UL << (n)) - 1) << (pos)))

typedef enum _ret_codes {
   SUCCESS,
   NOT_FOUND,
   VIOLATION,
   ACCESS_DENIED,
   INVALID_PARAMETER,
   TIMEOUT,
   BUSY,
   ERROR
} ret;

#endif 
