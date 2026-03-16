#include "aether.h"

static void delay(volatile uint32_t count) {
    while(count--) {
        __asm__("nop");
    }
}

__attribute__((used))
void app_main(void) {
    volatile uint32_t *RCC_AHB1ENR = (uint32_t *)0x40023830;
    *RCC_AHB1ENR |= (1 << 2);  
    
    volatile uint32_t *GPIOC_MODER = (uint32_t *)0x40020800;
    *GPIOC_MODER &= ~(3 << 26);
    *GPIOC_MODER |= (1 << 26);
    
    volatile uint32_t *GPIOC_ODR = (uint32_t *)0x40020814;
    
    while(1) {
        *GPIOC_ODR ^= (1 << 13);  // Toggle LED
        delay(1000000);
    }
}
