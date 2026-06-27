#ifndef CMD_H
#define CMD_H

#include "defs.h"
#include <stdbool.h>

typedef enum _cmd_e {
   CMD_UPDATE = 'F',
   CMD_RESET  = 'R',
   CMD_SCAN   = 'S',
   CMD_WATCH  = 'W',
   CMD_STOP   = 'T',
   CMD_COUNT
} cmd_e;

bool cmd_update(void);
bool cmd_scan(void);

u8 recv_cmd(void);
bool execute_cmd(u8 cmd);


#endif 
