#include "core/scheduler.h"

#include "core/systick.h"
#include "debug.h"

ret process_state(state_e state) 
{
   switch(state){
      case STATE_OFF:
      case STATE_TERMINATED:
         return SKIP_EXECUTION;
      case STATE_RUNNING:
      case STATE_WAITING:
         return SUCCESS; //FIXME: handle properly 
   }

   return SKIP_EXECUTION;
}


void run_scheduler(module_t mods[], u64 count)
{
   u64 i;
   ret res;

   while(1) {

      u64 now = system_ticks_g;

      for(i = 0; i < count; i++) {
         DEBUG_PRINT("loading module...\r\n"); 
         res = process_state(mods[i].state);
         if (res == SKIP_EXECUTION) {
            DEBUG_PRINT("skip execution...\r\n"); 
            continue;
         }
         if (now >= mods[i].next_time) {
            DEBUG_PRINT("running module...\r\n"); 
            load_from_desc(mods[i].desc);
            mods[i].next_time = now + mods[i].period_time;
         }
      }
      DEBUG_PRINT("cpu wait...\r\n"); 
      cpu_wait_for_interrupt();

   }

   return;
}
