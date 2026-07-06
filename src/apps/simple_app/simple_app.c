#include <aether.h>

int main(void)
{
   volatile int i = 0;
   for (; i < 1000000; i++){
      __asm volatile ("nop");
   }

   return 0;
}
