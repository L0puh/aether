#include <aether.h>


ret svc_region_request(app_desc_t *desc, u32 periph_id, u32 request)
{
   if (periph_id == PERIPH_NONE || periph_id >= PERIPH_COUNT) {
      DEBUG_PRINT("wrong periph id (%d), aborting memory request ", periph_id);
      return NOT_FOUND;
   }

   UNUSED(desc);
   if (!is_granted(desc->manifset.granted_periph_mask, periph_id)) {
      DEBUG_PRINT("app doesn't have permission to request this region (%d)", periph_id);
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
      DEBUG_PRINT("periph map is incomplete, error on hypervisor side");
      return ERROR;
   }

   u32 perms = ( AP_PRIV_RW_USER_RW );
   UNUSED(request);

   u32 size_attr = mpu_size_for_bytes(entry->size) | perms | XN_ENABLE | REGION_ENABLE;

   bool ok = mpu_program_dynamic_region(entry->base, size_attr, 0);
   DEBUG_PRINT("%s dynamic region", ok ? "granted" : "not granted");

   return ok ? SUCCESS: ACCESS_DENIED;
}

