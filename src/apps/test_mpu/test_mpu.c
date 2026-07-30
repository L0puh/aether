/******************************
 * this module is used to test 
 * MPU configuration and fuzz
 * with the permissions 
 ******************************/

#include <aether.h>


void test_readonly_write(void) 
{
   volatile u32 *region = (u32*) FLASH_HV_ORIGIN;
   *region= 0xDEADBEEF;
}

void test_non_executable(void) 
{
   void (*func)(void) = (void(*)(void)) RAM_HV_ORIGIN;
   func();
}

void test_priv_only(void)
{
   volatile u32 *region = (u32*)RAM_HV_ORIGIN;
   volatile u32 dummy = *region;
   (void)dummy;
}

void test_stack_overflow(void) 
{
   volatile u8 buffer[100], *ptr = &buffer[0];
   
   for (int i = 0; i < 200; i++){
      ptr[i] = 0xFF;
   }
}

void test_unaligned_access(void) 
{
   u8 buffer[10] = {0};
   volatile u32 *unaligned = (u32*)(buffer + 1);

   volatile u32 v = *unaligned;
   (void)v;
}

void test_divide_zero(void) 
{
   volatile int a = 10, b = 0;
   
   volatile int c = a / b;
   (void) c;
}

int main(void) 
{
   USART_t* usart = (USART_t*)USART1_BASE;

   if (hv_request_periph(PERIPH_USART1, AP_PRIV_RW_USER_RW) != 0) {
      return -1;
   }

   uart_writef_ex(usart, "MPU FUZZING: ");

   disable_irq();
   uart_writef_ex(usart, "read-only write\r\n");
   test_readonly_write();

   uart_writef_ex(usart, "non-exec\r\n");
   test_non_executable();

   uart_writef_ex(usart, "stack overflow\r\n");
   test_stack_overflow();

   uart_writef_ex(usart, "priv only\r\n");
   test_priv_only();

   uart_writef_ex(usart, "unaligned access\r\n");
   test_unaligned_access();
   
   uart_writef_ex(usart, "divide by zero\r\n");
   test_divide_zero();

   return 0;
}
