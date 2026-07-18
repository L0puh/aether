.syntax unified
.cpu cortex-m3
.thumb
.section .hv_text, "ax"
.global exit_landing
.thumb_func
exit_landing:
    mrs   r0, control
    bic.w r0, r0, #2    
    msr   control, r0
    isb
    bl    bootloader_exit_hook
    b     .
