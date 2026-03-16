#include "aether.h"

void main(void) {
    volatile u32 *RCC_AHB1ENR = (u32 *)0x40023830;
    *RCC_AHB1ENR |= (1 << 2);  
    
    volatile u32 *GPIOC_MODER = (u32 *)0x40020800;
    *GPIOC_MODER &= ~(3 << 26);
    *GPIOC_MODER |= (1 << 26);
    
    volatile u32 *GPIOC_ODR = (u32 *)0x40020814;
    
    while(1) {
        *GPIOC_ODR ^= (1 << 13);  // Toggle LED
        delay(1000000);
    }
}

static void delay(volatile u32 count) {
    while(count--) {
        __asm__("nop");
    }
}
