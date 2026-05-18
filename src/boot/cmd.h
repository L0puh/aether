#ifndef CMD_H
#define CMD_H

#include <stdbool.h>

typedef enum _cmd_e {
   CMD_UPDATE = 'F',
   CMD_RESET  = 'R',

   CMD_COUNT
} cmd_e;

bool cmd_update(void);

#endif 
