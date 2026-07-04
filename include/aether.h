#ifndef AETHER_H
#define AETHER_H

#define __INT32_C(c)  c ## L
#define __UINT32_C(c) c ## UL

#include "target.h"

#include <stdint.h>
#include <stdbool.h>

/* UTILS */
#include "defs.h"
#include "core/debug.h"

/* CORE */
#include "core/gpio.h"
#include "core/systick.h"
#include "core/uart.h"
#include "core/clock.h"
#include "core/mpu.h"
#include "core/utils.h"
#include "core/crypto.h"

/* PLATFORM SPECIFIC */
#include "device_vectors.h"
#include "memory_map.h"

/* HYPERVISOR */
#include "boot/hv.h"
#include "core/scheduler.h"
#include "boot/flasher.h"

#define APP_MAGIC (0xDEADBEEF)
#define APP_DESC_ADDR (FLASH_APP_ORIGIN + APP_DESC_OFFSET)

typedef void (*app_entry_t)(void);


#endif

