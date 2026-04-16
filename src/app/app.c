#include <aether.h>

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
   ret status;

   init_debug_led();
   status = rcc_init_uart_clock(USART1, GPIOA, 9, GPIOA, 10);
   if (IS_ERROR(status)) {
      toggle_degug_led();
      return;
   }
   uart_init(USART1, 0);

#ifdef SYSTEM_CLOCK_72Mhz
   status = set_system_clock_72MHz();
#else 
   status = set_system_clock_25MHz();
#endif 
   
   if (IS_ERROR(status)) {
      uart_write("error in setting up clock\n");
      return;
   }

   while(1) 
   {
      toggle_degug_led();
      uart_write("HELLO THERE\r\n");
      systick_msec_delay(1000);
   }
}

