.syntax unified
.cpu cortex-m4
.thumb
.global svc_handler
.thumb_func
svc_handler:
	tst   lr, #4
	ite   eq
	mrseq r0, msp
	mrsne r0, psp
	push  {lr}
	bl    svc_handler_c
	pop   {lr}
	bx    lr
