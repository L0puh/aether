#include "defs.h"
#include <aether.h>

typedef void (*app_entry_t)(void);

void init_debug_led()
{
   rcc_enable_clock_pin(GPIOC);

   GPIOC->CRH &= CLEAR_BITS(4, 20);
   GPIOC->CRH |= OUTPUT_50MHZ | GENERAL_PUSHPULL;
}

void toggle_debug_led()
{
   GPIOC->ODR ^= (1 << 13);
}

void led_blink(int count, int time)
{
   while (count--)
   {
      toggle_debug_led();

      for (volatile int i = 0; i < time; i++);

      toggle_debug_led();

      for (volatile int i = 0; i < time; i++);
   }
}

ret setup_system()
{
   ret status;

#ifdef SYSTEM_CLOCK_72Mhz
   status = set_system_clock_72Mhz();
#else
   status = set_system_clock_25Mhz();
#endif

   systick_init();
   init_debug_led();

   status = rcc_init_uart_clock(
         USART1,
         GPIOA, 9,
         GPIOA, 10
         );

   if (IS_ERROR(status))
      return status;

   uart_init(USART1, 0);

   hv_api_init();

   BOOTLOADER_DEBUG("SYSTEM OK\r\n");

   return SUCCESS;
}

static void run_app(app_desc_t* desc)
{
   app_entry_t entry;
   entry = (app_entry_t)((u32)desc->entry | 1);
   BOOTLOADER_DEBUG("RUNNING APP\r\n");
   entry();
   BOOTLOADER_DEBUG("APP RETURNED\r\n");
}

void scan_for_apps()
{
   u32 start = 0x08002000;
   u32 end   = 0x08008000;

   for (u32 addr = start; addr < end; addr += 4)
   {
      app_desc_t* desc = (app_desc_t*)addr;

      if (desc->magic != APP_MAGIC)
         continue;

      BOOTLOADER_DEBUG(
            "APP FOUND @ 0x%08x\r\n",
            addr
            );

      BOOTLOADER_DEBUG(
            "ENTRY = 0x%08x\r\n",
            desc->entry
            );

      run_app(desc);
      wait_interrupt();
   }

   BOOTLOADER_DEBUG("NO MORE APPS\r\n");
   wait_interrupt();
}

   __attribute__((noreturn))
int bootloader_entry()
{
   enable_irq();
   if (IS_ERROR(setup_system())) {
      toggle_debug_led();
   }

   BOOTLOADER_DEBUG("BOOTLOADER START\r\n");
   while(true) {
      scan_for_apps();
   }
}
