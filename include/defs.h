#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define NULL 0
#define UNUSED(x)((void)(x))
#define IS_ERROR(ret) (ret > SUCCESS)

typedef enum _ret_codes {
   SUCCESS,
   NOT_FOUND,
   VIOLATION,
   ACCESS_DENIED,
   INVALID_PARAMETR,
} ret;

#endif 
