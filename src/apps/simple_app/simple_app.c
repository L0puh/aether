#include <aether.h>

void test_memmanage_fault(void) {
    volatile uint32_t *invalid_ptr = (uint32_t*)0x60000000; 
    volatile uint32_t value = *invalid_ptr;  
    (void)value;
}

void test_instruction_fetch_fault(void) {
    void (*invalid_func)(void) = (void(*)(void))0x0800FFFF;
    invalid_func(); 
}

void test_mpu_access_fault(void) {
    volatile uint32_t *protected_ptr = (uint32_t*)0x20001000; 
    *protected_ptr = 0xDEADBEEF; 
}

int main(void) {
    USART_t *usart = (USART_t*)USART1_BASE;
    
    if (hv_request_periph(PERIPH_USART1, AP_PRIV_RW_USER_RW) != 0) {
        return -1;
    }
    
    uart_write_ex(usart, "HELLO WORLD FROM APP! I JUST GOT FREEDOM OF SPEECH\r\n");
    
    uart_write_ex(usart, "triggering memmanage fault...\r\n");
   
    /* test_memmanage_fault();            // data access fault */
    test_instruction_fetch_fault(); // instruction fetch fault
    // test_mpu_access_fault();        // MPU protection fault
    
    uart_write_ex(usart, "fault didn't work!\r\n");
    
    return 0;
}
