#ifndef FAULT_H
#define FAULT_H


// system handler control and state register (shcsr) 
#define SCB_SHCSR_MEMFAULTENA_Msk      (1UL << 16)  // memmanage enable
#define SCB_SHCSR_BUSFAULTENA_Msk      (1UL << 17)  // busfault enable
#define SCB_SHCSR_USGFAULTENA_Msk      (1UL << 18)  // usagefault enable

// status bits 
#define SCB_SHCSR_MEMFAULTACT_Msk      (1UL << 0)   // memmanage active
#define SCB_SHCSR_BUSFAULTACT_Msk      (1UL << 1)   // busfault active
#define SCB_SHCSR_USGFAULTACT_Msk      (1UL << 3)   // usagefault active
#define SCB_SHCSR_SVCALLACT_Msk        (1UL << 7)   // svc active
#define SCB_SHCSR_MONITORACT_Msk       (1UL << 8)   // monitor active
#define SCB_SHCSR_PENDSVACT_Msk        (1UL << 10)  // pendsv active
#define SCB_SHCSR_SYSTICKACT_Msk       (1UL << 11)  // systick active
#define SCB_SHCSR_USGFAULTPENDED_Msk   (1UL << 12)  // usagefault pending
#define SCB_SHCSR_MEMFAULTPENDED_Msk   (1UL << 13)  // memmanage pending
#define SCB_SHCSR_BUSFAULTPENDED_Msk   (1UL << 14)  // busfault pending
#define SCB_SHCSR_SVCALLPENDED_Msk     (1UL << 15)  // svc pending
                                                    
#define SCB_CCR_NONBASETHRDENA_Msk     (1UL << 0)   // non bade thread mode enable
#define SCB_CCR_USERSETMPEND_Msk       (1UL << 1)   // user set pending 
#define SCB_CCR_UNALIGN_TRP_Msk        (1UL << 3)   // trap unaligned access
#define SCB_CCR_DIV_0_TRP_Msk          (1UL << 4)   // trap div by 0
#define SCB_CCR_BFHFNMIGN_Msk          (1UL << 8)   // ignore faults
#define SCB_CCR_STKALIGN_Msk           (1UL << 9)   // stack alignment on exception entry
                                                    
void enable_fault_handlers(void);

#endif 
