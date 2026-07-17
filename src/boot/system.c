#include <aether.h>

void init_debug_led(void)
{
   rcc_enable_clock_pin(GPIOC);
   gpio_set_mode(GPIOC, 13, OUTPUT_PP_2MHz);
}

void toggle_debug_led(void)
{
   GPIOC->ODR ^= (1 << 13);
}

void led_blink(int count, int time)
{
   while (count--)
   {
      toggle_debug_led();
      systick_msec_delay(time);
   }
}

ret system_setup(void)
{
   ret status;

#ifdef SYSTEM_CLOCK_72Mhz
   status = set_system_clock_72Mhz();
#endif

   systick_init();
   init_debug_led();

   status = rcc_init_uart_clock(USART1, GPIOA, 9, GPIOA, 10);

   if (IS_ERROR(status))
      return status;

   uart_init(USART1, 0);
   uart_flush_tx();
   uart_flush_rx();

   flash_lock();

   DEBUG_PRINT("SYSTEM OK");
   
   mpu_init();
   mpu_enable();

   return SUCCESS;
}


