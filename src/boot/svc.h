#ifndef SVC_H
#define SVC_H

#include <boot/hv.h>
#include "defs.h"
#include <core/gpio.h>
#include <hvapi.h>

typedef struct {
    periph_id_e id;
    u32 base;
    u32 size;      
} periph_map_entry_t;

static const periph_map_entry_t periph_map[] = {
    { PERIPH_USART1, USART1_BASE, sizeof(USART_t) },
    { PERIPH_USART2, USART2_BASE, sizeof(USART_t) },
    { PERIPH_GPIOA,  GPIOA_BASE, sizeof(GPIO_t) },
    { PERIPH_GPIOB,  GPIOB_BASE, sizeof(GPIO_t) },
    { PERIPH_GPIOC,  GPIOC_BASE, sizeof(GPIO_t) },

    /* TODO: */
    /* { PERIPH_SPI1,   0x40013000, 0x400 }, */ 
    /* { PERIPH_I2C1,   0x40005400, 0x400 }, */
};
#define PERIPHS_MAP_TOTAL ( sizeof(periph_map)/sizeof(periph_map[0]) )

ret svc_region_request(app_desc_t *app, u32 periph_id, u32 request);


#endif 
