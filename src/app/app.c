#include <aether.h>

#define CLEAR_BITS(n, pos) (~(((1UL << (n)) - 1) << (pos)))
#define GENERAL_PUSHPULL   (0b00 << 22)
#define ALTER_PUSHPULL     (0b01 << 22)
#define GENERAL_OPENDRAIN  (0b10 << 22)
#define ALTER_OPENDRAIN    (0b11 << 22) 
#define OUTPUT_50MHZ       (0b11 << 20)
#define OUTPUT_10MHZ       (0b01 << 20)
#define OUTPUT_2MHZ        (0b10 << 20)
#define INPUT_MODE         (0b00 << 20)

void init_debug_led()
{
   rcc_enable_clock_pin(GPIOC);
   GPIOC->CRH &= CLEAR_BITS(4, 20);
   GPIOC->CRH |= OUTPUT_50MHZ | GENERAL_PUSHPULL;
}

void toggle_degug_led() 
{
   GPIOC->ODR ^= (1 << 13);
}


void main(void) 
{
   set_system_clock(SYSTEM_CLOCK_25Mhz);
   rcc_init_uart_clock(USART1, GPIOA, NULL);
   init_debug_led();
   
   uart_init(USART1, 0);

   while(1) 
   {
      uart_write(msg);
      toggle_degug_led();
      systick_msec_delay(3000);
   }
}
