.syntax unified
.cpu cortex-m3
.thumb

.section .app_start, "ax"
.global app_start
.thumb_func
app_start:
    @ drop to unprivileged
    mrs   r0, control
    orr   r0, r0, #1   
    msr   control, r0
    isb
    
    @ unprivileged
    bl    main
    
    @ exit 
    movs  r0, #0
    svc   #2
    b     .
