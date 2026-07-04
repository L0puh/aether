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
       mpu_set_static_region(i, &static_regions[i]);
       MPU_DEBUG("region id %d: base=0x%x attr=0x%x\r\n", i, static_regions[i].base, static_regions[i].attr_size);
    }

    mpu_enable();
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
   return true;
}

