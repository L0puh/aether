#ifndef CONFIG_H
#define CONFIG_H


#include "core/hv.h"
#include "defs.h"

ret config_parse_mods(module_t *mods, u32 count);
u32 config_lookup_mods_count();

#endif 
