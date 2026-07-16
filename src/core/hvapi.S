.syntax unified
.thumb
.section .text
.align 2

#include "hvapi_defs.h"

.global hv_request_periph
.global hv_wdt_kick
.global hv_exit

@ in:  r0 = id, r1 = perms
@ out: status in r0
hv_request_periph:
    svc SVC_REG_REQ    
    bx  lr

@ out: status in r0
hv_wdt_kick:
    svc SVC_WDT_KICK  
    bx  lr

@ out: r0 = exit code
hv_exit:
    svc SVC_EXIT     
    bkpt 0          
