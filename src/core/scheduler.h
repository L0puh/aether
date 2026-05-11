/**************************************************************************

  just a simple scheduler for show-case purpose, must be rewritten later

 **************************************************************************/

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <core/hv.h>

ret process_state(state_e state) ;
void run_scheduler(module_t mods[], u64 count);

#endif 
