#ifndef MPU_H
#define MPU_H

#include "core/gpio.h"
#include "defs.h"
#include "memory_map.h"
#include <stdbool.h>

#define SUBREG_BITS(mask)   ((mask) << 8)
#define MPU_ENABLE          (1 << 0)
#define PRIV_DEFAULT_ENABLE (1 << 2)
#define REGION_NUM(n)       (n)
#define REGION_VALID        (1 << 4)
#define REGION_BASE_ADDR(a) ((a) & 0xFFFFFFE0)

#define REGION_ENABLE       (1 << 0)
#define REGION_SIZE_256B    (0x7 << 1)    // 7
#define REGION_SIZE_512B    (0x8 << 1)    // 8
#define REGION_SIZE_1KB     (0x9 << 1)    // 9 
#define REGION_SIZE_2KB     (0xA << 1)    // 10
#define REGION_SIZE_4KB     (0xB << 1)    // 11
#define REGION_SIZE_8KB     (0xC << 1)    // 12
#define REGION_SIZE_16KB    (0xD << 1)    // 13
#define REGION_SIZE_32KB    (0xE << 1)    // 14
#define REGION_SIZE_64KB    (0xF << 1)    // 15
#define REGION_SIZE_128KB   (0x10 << 1)   // 16
#define REGION_SIZE_256KB   (0x11 << 1)   // 17
#define REGION_SIZE_512KB   (0x12 << 1)   // 18
#define REGION_SIZE_1MB     (0x13 << 1)   // 19
#define REGION_SIZE_2MB     (0x14 << 1)   // 20
#define REGION_SIZE_4MB     (0x15 << 1)   // 21
#define REGION_SIZE_8MB     (0x16 << 1)   // 22
#define REGION_SIZE_16MB    (0x17 << 1)   // 23
#define REGION_SIZE_32MB    (0x18 << 1)   // 24
#define REGION_SIZE_64MB    (0x19 << 1)   // 25
#define REGION_SIZE_128MB   (0x1A << 1)   // 26
#define REGION_SIZE_256MB   (0x1B << 1)   // 27
#define REGION_SIZE_512MB   (0x1C << 1)   // 28
#define REGION_SIZE_1GB     (0x1D << 1)   // 29
#define REGION_SIZE_2GB     (0x1E << 1)   // 30
#define REGION_SIZE_4GB     (0x1F << 1)   // 31

#define AP_NO_ACCESS        (0x0 << 24)   /* no access */
#define AP_PRIV_RW          (0x1 << 24)   /* privileged read/write */
#define AP_PRIV_RW_USER_RO  (0x2 << 24)   /* rw, user ro */ 
#define AP_PRIV_RW_USER_RW  (0x3 << 24)   /* full read/write */
#define AP_PRIV_RO          (0x5 << 24)   /* privileged read-only */
#define AP_RO               (0x6 << 24)   /* read-only */

#define XN_ENABLE           (1 << 28)     /* execute never */
#define XN_DISABLE          (0 << 28)     /* execute allowed */



typedef struct _mpu_region_t {
   u32 base;
   u32 attr_size;
   u32 subreg_mask; // subregion disable bits 
} mpu_region_t;

typedef enum _region_type {
   REG_NULL_GUARD = 0,
   REG_HV_FLASH,
   REG_HV_RAM,
   REG_APP_FLASH,
   REG_APP_RAM,
   REG_RAM_GUARD,
   REG_DYNAMIC = 6 // reserved
} region_type_e;


// probably later it will be done with
// .cfg file and generated automatically...maybe
static const mpu_region_t static_regions[] = 
{
   [REG_NULL_GUARD] = {
    .base = 0x00000000,
    .attr_size = REGION_SIZE_256B | AP_PRIV_RO | XN_ENABLE | REGION_ENABLE,  
    .subreg_mask = 0b11111110,  
   },

   [REG_HV_FLASH] = {
      .base = FLASH_HV_ORIGIN,
      .attr_size = REGION_SIZE_16KB | AP_PRIV_RO | XN_DISABLE | REGION_ENABLE,
      .subreg_mask = 0,  
   },
   [REG_HV_RAM] = {
      .base = RAM_HV_ORIGIN,
      .attr_size = REGION_SIZE_4KB | AP_PRIV_RW | XN_ENABLE | REGION_ENABLE,
      .subreg_mask = 0,   
   },
   [REG_APP_FLASH] = {
      .base = FLASH_APP_ORIGIN,
      .attr_size = REGION_SIZE_64KB | AP_PRIV_RW_USER_RO | XN_DISABLE | REGION_ENABLE,
      .subreg_mask = 0b11000000, 
   },
   [REG_APP_RAM] = {
      .base = RAM_APP_ORIGIN,
      .attr_size = REGION_SIZE_16KB | AP_PRIV_RW_USER_RW | XN_ENABLE | REGION_ENABLE,
      .subreg_mask = 0b11000000,  
   },
   [REG_RAM_GUARD] = {
      .base = RAM_GUARD_ORIGIN,
      .attr_size = REGION_SIZE_1KB | AP_NO_ACCESS | XN_ENABLE | REGION_ENABLE,
      .subreg_mask = 0,  
   },
};

#define NUM_STATIC_REGS (sizeof(static_regions) / sizeof(static_regions[0]))

void mpu_init();
void mpu_disable(void);
void mpu_enable(void);

void mpu_set_static_region(u8 num, const mpu_region_t *cfg);
bool mpu_program_dynamic_region(u32 base, u32 size_attr, u32 srd);


#endif 
