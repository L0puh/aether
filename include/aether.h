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

/* PLATFORM SPECIFIC */
#include "aether/device_vectors.h"
#include "aether/memory.h"

/* HYPERVISOR */
#include "boot/hv.h"
#include "core/scheduler.h"

#define APP_MAGIC (0xDEADBEEF)

#define ADD_ADDR    0x08002000   
#define HV_API_ADDR 0x20002400  

#endif

