#ifndef MPU_H
#define MPU_H

#define MPU_ENABLE          (1 << 0)
#define PRIV_DEFAULT_ENABLE (1 << 2)
#define REGION_NUM(n)       (n)
#define REGION_VALID        (1 << 4)
#define REGION_BASE_ADDR(a) ((a) & 0xFFFFFFE0)

#define REGION_ENABLE       (1 << 0)
#define REGION_SIZE_256B    (0x7 << 1)   
#define REGION_SIZE_4KB     (0xB << 1)   
#define REGION_SIZE_8KB     (0xC << 1)   
#define REGION_SIZE_32KB    (0xE << 1)   
#define REGION_SIZE_4GB     (31 << 1)   

#define AP_NO_ACCESS        (0x0 << 24)   /* no access */
#define AP_PRIV_RW          (0x1 << 24)   /* privileged read/write */
#define AP_PRIV_RW_USER_RW  (0x3 << 24)   /* full read/write */
#define AP_PRIV_RO          (0x5 << 24)   /* privileged read-only */
#define AP_RO               (0x6 << 24)   /* read-only */

#define XN_ENABLE           (1 << 28)     /* execute never */
#define XN_DISABLE          (0 << 28)     /* execute allowed */



void mpu_init();
void mpu_disable(void);
void mpu_enable(void);


#endif 
