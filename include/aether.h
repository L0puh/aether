#ifndef AETHER_H
#define AETHER_H

#include "target.h"

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
#include "core/watchdog.h"
#include "core/crypto.h"

/* HYPERVISOR */
#include "hv/hvapi.h"
#include "hv/hvapi_defs.h"

/* PLATFORM SPECIFIC */
#include "device_vectors.h"
#include "memory_map.h"


/* BOOTLOADER */
#include "boot/flasher.h"
#include "boot/system.h"
#include "boot/svc.h"


#endif

