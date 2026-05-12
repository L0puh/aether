#include "core/scheduler.h"
#include "boot/hv.h"
#include "core/systick.h"
#include "debug.h"


void run_scheduler()
{
   u64 i;
   ret res;

   //FIXME:
   /* // get modules */ 
   /* module_t *mods; */
   /* u32 count = 0; */

   /* while(1) { */

   /*    u64 now = system_ticks_g; */

   /*    for(i = 0; i < count; i++) { */
   /*       DEBUG_PRINT("loading module...\r\n"); */ 
   /*       res = process_state(mods[i].state); */
   /*       if (res == SKIP_EXECUTION) { */
   /*          DEBUG_PRINT("skip execution...\r\n"); */ 
   /*          continue; */
   /*       } */
   /*       if (now >= mods[i].next_time) { */
   /*          DEBUG_PRINT("running module...\r\n"); */ 
   /*          load_from_desc(mods[i].desc); */
   /*          mods[i].next_time = now + mods[i].period_time; */
   /*       } */
   /*    } */
   /*    DEBUG_PRINT("cpu wait...\r\n"); */ 
   /*    cpu_wait_for_interrupt(); */

   /* } */

   return;
}
