#ifndef AETHER_H
#define AETHER_H

#include <stdint.h>
#include <stdbool.h>

#include "defs.h"
#include "utils/debug.h"

#include "core/gpio.h"
#include "core/systick.h"
#include "core/uart.h"
#include "core/clock.h"

#include "aether/device_vectors.h"
#include "aether/memory.h"

int bootloader_entry(void);
int app_entry(void);

#endif

