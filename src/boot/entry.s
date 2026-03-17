.syntax unified
.cpu cortex-m3
.thumb

.section .text
.global entry_point
.thumb_func
entry_point:
    @ r0 = pc					 @ app reset address	
    @ r1 = sp					 @ stack pointer
    msr  msp, r1            @ load app stack pointer into MSP
    bx   r0                 @ jump to app — no return

