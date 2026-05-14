#include <aether.h> 


void mpu_init(void)
{
    MPU->CTRL = 0;
    data_sync_barrier();
    instr_sync_barrier();

    MPU->REGNUM = 0;                                
    MPU->REGBASE = REGION_BASE_ADDR(ADD_ADDR);       
    MPU->ATTR_AND_SIZE = REGION_SIZE_8KB               
                     | AP_PRIV_RO                    
                     | XN_DISABLE                    
                     | REGION_ENABLE;                

    MPU->REGNUM = 1;                                 
    MPU->REGBASE = REGION_BASE_ADDR(HV_API_ADDR);    
    MPU->ATTR_AND_SIZE = REGION_SIZE_256B            
               | AP_PRIV_RW_USER_RW                  
               | XN_DISABLE                          
               | REGION_ENABLE;                      
    
    MPU->REGNUM = 2;                                 
    MPU->REGBASE = REGION_BASE_ADDR(0x00000000);     
    MPU->ATTR_AND_SIZE = REGION_SIZE_4KB             
               | AP_NO_ACCESS                        
               | XN_ENABLE                           
               | REGION_ENABLE;                      

    MPU->CTRL = MPU_ENABLE | PRIV_DEFAULT_ENABLE;
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

