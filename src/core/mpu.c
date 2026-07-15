#include "core/mpu.h"
#include <aether.h> 


void mpu_set_static_region(u8 num, const mpu_region_t *cfg)
{
   if (cfg == NULL) {
      return;
   }

   MPU->REGNUM  = num;
   MPU->REGBASE = REGION_BASE_ADDR(cfg->base);
   MPU->ATTR_AND_SIZE = cfg->attr_size | SUBREG_BITS(cfg->subreg_mask);

}

void mpu_init(void)
{
    MPU->CTRL = 0;
    data_sync_barrier();
    instr_sync_barrier();
  
    MPU_DEBUG("setting up regions...\r\n");
    for (u8 i = 0; i < NUM_STATIC_REGS; i++)
    {
       if (static_regions[i].base & (0x2000 - 1)) {
          MPU_DEBUG("WARNING: region %d is not 8KB aligned\r\n", i);
       }
       mpu_set_static_region(i, &static_regions[i]);
       MPU_DEBUG("region id %d: base=0x%x attr=0x%x\r\n", i, static_regions[i].base, static_regions[i].attr_size);
    }

    data_sync_barrier();
    instr_sync_barrier();
}

void mpu_disable(void)
{
   MPU->CTRL = 0;
   data_sync_barrier();
   instr_sync_barrier();
}


void mpu_enable(void)
{
   MPU->CTRL = MPU_ENABLE | PRIV_DEFAULT_ENABLE;
   data_sync_barrier();
   instr_sync_barrier();
}

bool mpu_program_dynamic_region(u32 base, u32 size_attr, u32 srd) 
{
   mpu_region_t cfg = {.base = base, .attr_size = size_attr, .subreg_mask = srd};
  
   mpu_set_static_region(REG_DYNAMIC, &cfg);
   data_sync_barrier();
   instr_sync_barrier();
   mpu_enable();

   return true;
}

u32 mpu_size_for_bytes(const u32 bytes)
{

   const u32 sizes[] = {32, 64, 256, 1024, 4096};
   const u32 region_sizes[] = {REGION_SIZE_32B, REGION_SIZE_64B,
                                REGION_SIZE_256B, REGION_SIZE_1KB,
                                REGION_SIZE_4KB};

   const u32 num_sizes = sizeof(sizes)/sizeof(sizes[0]);

   for (u32 i = 0; i < num_sizes; i++) {
      if (bytes <= sizes[i]) {
         MPU_DEBUG("mpu size for %d bytes: %d\r\n", bytes, sizes[i]);
         return region_sizes[i];
      }
   }

   return 0;
}
