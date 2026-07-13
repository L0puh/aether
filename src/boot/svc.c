#include "boot/hv.h"
#include "core/debug.h"
#include "core/mpu.h"
#include "defs.h"
#include <aether.h>


ret svc_region_request(app_desc_t *desc, u32 periph_id, u32 request)
{
   if (periph_id == PERIPH_NONE || periph_id >= PERIPH_COUNT) {
      DEBUG_PRINT("wrong periph id (%d), aborting memory request \r\n", periph_id);
      return NOT_FOUND;
   }

   UNUSED(desc);
   if (!(desc->manifset.granted_periph_mask & (1u << periph_id))) {
      DEBUG_PRINT("app doesn't have permission to request this region (%d)\r\n", periph_id);
      return VIOLATION;
   }

   const periph_map_entry_t *entry = NULL;
   for (u32 i = 0; i < PERIPHS_MAP_TOTAL; i++) {
      if (periph_map[i].id == periph_id) {
         entry = &periph_map[i];
         break;
      }
   }

   if (!entry) {
      DEBUG_PRINT("periph map is incomplete, error on hypervisor side\r\n");
      return ERROR;
   }

   u32 perms = request & ( AP_PRIV_RW_USER_RW | XN_ENABLE );
   if (perms == 0) perms = AP_PRIV_RW_USER_RW;

   u32 size_attr = mpu_size_for_bytes(entry->size) | perms | XN_ENABLE | REGION_ENABLE;
   

   bool ok = mpu_program_dynamic_region(entry->base, size_attr, 0);
   DEBUG_PRINT("%s dynamic region\r\n", ok ? "OK" : "ERROR");

   return ok ? SUCCESS: ACCESS_DENIED;
}

