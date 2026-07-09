// THIS IS THE ONLY FILE AVAILABLE FOR APP

#ifndef HV_API
#define HV_API

#include <stdint.h>
#include <defs.h>

typedef enum {
   SVC_REG_REQ = 0,
  // ?? SVC_FLASH_WRITE,
   SVC_WDT_KICK,
   SVC_EXIT,


   SVC_COUNT
} svc_id_e;

typedef enum {
    PERIPH_NONE = 0,
    PERIPH_USART1,
    PERIPH_USART2,
    PERIPH_GPIOA,
    PERIPH_GPIOB,
    PERIPH_GPIOC,
    /* PERIPH_SPI1, */
    /* PERIPH_I2C1, */
    PERIPH_COUNT
} periph_id_e;

static inline int32_t hv_request_periph(periph_id_e id, u32 perms) {
    register u32 r0 asm("r0") = (u32)id;
    register u32 r1 asm("r1") = perms;
    asm volatile ("svc %[svc_id]"
        : "+r"(r0) : [svc_id] "I" (SVC_REG_REQ), "r"(r1) : "memory");
    return (int32_t)r0;
}

static inline int32_t hv_wdt_kick(void) {
    register u32 r0 asm("r0");
    asm volatile ("svc %[svc_id]"
        : "=r"(r0) : [svc_id] "I" (SVC_WDT_KICK) : "memory");
    return (int32_t)r0;
}

/* static inline void hv_yield(void) { */
/*     asm volatile ("svc %[svc_id]" :: [svc_id] "I" (SVC_YIELD) : "memory"); */
/* } */

static inline void hv_exit(int32_t code) {
    register u32 r0 asm("r0") = (u32)code;
    asm volatile ("svc %[svc_id]" :: [svc_id] "I" (SVC_EXIT), "r"(r0) : "memory");
    __builtin_unreachable();
}

#endif 
