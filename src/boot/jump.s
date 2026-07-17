.syntax unified
.thumb
.global enter_app
.thumb_func
enter_app:
    @ r0 = psp value
    @ r1 = entry point 
    
    @ save entry point
    push  {r4, lr}
    mov   r4, r1
    
    @ set psp
    msr   psp, r0
    isb
    
    @ clear exception masks
    mov   r3, #0
    msr   primask, r3
    msr   basepri, r3
    cpsie i
    
    @ switch to psp 
    mrs   r3, control
    orr   r3, r3, #2       
    msr   control, r3
    isb
    
	 @TODO: set pendSV/SVC check after app starts
    @ jump to app entry
    bx    r4
