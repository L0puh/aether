#include <aether.h>

int app_entry(void) 
{
   ret status;
   uart_write("LOADING APP ... OK\r\n");
   char buffer[MAX_BUFFER_SIZE];
   while(1) 
   {

      status = uart_getline(buffer, MAX_BUFFER_SIZE);
      if (status == SUCCESS) 
      {
         uart_write("NEW MESSAGE!\r\n");
         uart_write(buffer);
      }
      systick_msec_delay(1000);
   }

   return 0;
}

