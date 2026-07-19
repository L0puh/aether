#ifndef WATCHDOG_H
#define WATCHDOG_H


#include "defs.h"

#define IWDG_KEY_ENABLE  0xCCCC
#define IWDG_KEY_UNLOCK  0x5555
#define IWDG_KEY_REFRESH 0xAAAA

void watchdog_init(u32 timeout);
void watchdog_kick(void);
bool is_app_healthy(void);
bool is_iwdg_enabled(void);
void mark_app_alive(void);

#endif 
