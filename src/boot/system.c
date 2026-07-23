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

   enable_fault_handlers();
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

bool is_granted(u32 mask, u32 perm) {
   return mask & (1u << perm);
}

ret preinit_periph(const app_manifest_t manifest)
{
   ret status = SUCCESS;
   PLAIN_PRINT("preiniting peripherals: ");


   if (is_granted(manifest.granted_periph_mask, PERIPH_GPIOA)){
      PLAIN_PRINT("GPIOA ");
      status = rcc_enable_clock_pin(GPIOA);
      if (IS_ERROR(status)){
         return status;
      }
   }

   if (is_granted(manifest.granted_periph_mask, PERIPH_GPIOB)){
      PLAIN_PRINT("GPIOB ");
      status = rcc_enable_clock_pin(GPIOB);
      if (IS_ERROR(status)){
         return status;
      }
   }

   if (is_granted(manifest.granted_periph_mask, PERIPH_GPIOC)){
      PLAIN_PRINT("GPIOC ");
      status = rcc_enable_clock_pin(GPIOC);
      if (IS_ERROR(status)){
         return status;
      }
   }

   if (is_granted(manifest.granted_periph_mask, PERIPH_USART1)){
      PLAIN_PRINT("USART1 ");
      status = rcc_init_uart_clock(USART1, GPIOA, 9, GPIOA, 10);

      if (IS_ERROR(status)){
         return status;
      }

      uart_init(USART1, 0);
   }

   if (is_granted(manifest.granted_periph_mask, PERIPH_USART2)){
      PLAIN_PRINT("USART2 ");
      status = rcc_init_uart_clock(USART2, GPIOA, 2, GPIOA, 3);

      if (IS_ERROR(status)){
         return status;
      }

      uart_init(USART2, 0);
   }

   PLAIN_PRINT("\r\n");
   return status;
}
