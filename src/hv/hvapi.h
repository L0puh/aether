#ifndef HV_API
#define HV_API

#include <defs.h>

extern int32_t hv_request_periph(u32 id, u32 perms);
extern int32_t hv_wdt_kick(void);
extern void    hv_exit(int32_t code) __attribute__((noreturn));
extern int     app_start(void);

#endif 
