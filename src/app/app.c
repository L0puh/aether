#include <aether.h>

void main(void) {

   RCC->AHBENR |= (1 << 4);       // enable clock for pin C
   GPIOC->CRH &= ~(0xF << 20);    // clean bits
   GPIOC->CRH &=  (0x2 << 20);    // set CNF[00]MODE[10]

   while(1) {
      GPIOC->ODR ^= (1 << 13);
      systick_msec_delay(5000);
   }
}

void delay(volatile u32 count) {
   while(count--) {
      __asm__("nop");
   }
}
