#
# transit point from 
# bootloader ---> app 
# to drop privileges 
#

.syntax unified
.cpu cortex-m3
.thumb

.section .app_signatures, "a"
.global app_signature
app_signature:
    .word 0x0DEAFBEE

.section .app_start, "ax"
.global app_start
.thumb_func
app_start:
    mrs   r0, control
    orr   r0, r0, #1   
    msr   control, r0
    isb
    
    bl    main
    
    movs  r0, #0
    svc   #2
    b     .
