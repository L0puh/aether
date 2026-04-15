#include <aether.h>

void delay(volatile u32 count);

void main(void) {
    RCC->APB2ENR |= (1 << 4);
    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |= (0x2 << 20);

    while(1) {
        GPIOC->ODR ^= (1 << 13);
        systick_msec_delay(100);
    }
}

void delay(volatile u32 count) {
   while(count--) {
      __asm__("nop");
   }
}
