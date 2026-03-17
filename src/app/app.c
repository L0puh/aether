#include "aether.h"

void main(void) {
    volatile u32 *RCC_APB2ENR = (u32 *)0x40021018;  
    volatile u32 *GPIOC_CRH = (u32 *)0x40011004;   
    volatile u32 *GPIOC_ODR = (u32 *)0x4001100C;  
    
    *RCC_APB2ENR |= (1 << 4);
    
    *GPIOC_CRH &= ~(0xF << 20);     
    *GPIOC_CRH |= (0x2 << 20);     
    
    while(1) {
        *GPIOC_ODR ^= (1 << 13);    
        delay(1000000);
    }
}

void delay(volatile u32 count) {
    while(count--) {
        __asm__("nop");
    }
}
