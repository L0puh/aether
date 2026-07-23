.syntax unified 
.thumb 

.macro FAULT_ENTRY name
.global \name
.thumb_func 
\name:
	 tst   lr, #4
	 ite   eq
	 mrseq r0, msp
	 mrsne r0, psp
	 mov   r1, lr          
	 push  {lr}
	 bl	 \name\()_c 
	 pop   {lr}
	 cmp   r0, #0
	 it    ne
	 movne lr, r0          
	 bx    lr
.endm

FAULT_ENTRY memmanage_handler
FAULT_ENTRY hardfault_handler
FAULT_ENTRY busfault_handler
FAULT_ENTRY usagefault_handler
FAULT_ENTRY svc_handler
