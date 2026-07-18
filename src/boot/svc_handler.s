.syntax unified
.thumb
.global svc_handler
.thumb_func
svc_handler:
    tst   lr, #4
    ite   eq
    mrseq r0, msp
    mrsne r0, psp
    mov   r1, lr          
    push  {lr}
    bl    svc_handler_c  
    pop   {lr}
    cmp   r0, #0
    it    ne
    movne lr, r0          
    bx    lr
