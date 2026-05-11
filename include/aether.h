#ifndef AETHER_H
#define AETHER_H

#define __INT32_C(c)  c ## L
#define __UINT32_C(c) c ## UL

#include <stdint.h>

#include <stdbool.h>

/* UTILS */
#include "defs.h"
#include "core/debug.h"
#include "core/config.h"

/* CORE */
#include "core/gpio.h"
#include "core/systick.h"
#include "core/uart.h"
#include "core/clock.h"

/* PLATFORM SPECIFIC */
#include "aether/device_vectors.h"
#include "aether/memory.h"

/* HYPERVISOR */
#include "core/hv.h"
#include "core/scheduler.h"

#endif

