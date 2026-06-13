#ifndef DEBUG_H
#define DEBUG_H

#include "target.h"
#include "core/uart.h"

#ifdef _DEBUG

   #define DEBUG_PRINT(fmt, ...) \
       uart_writef("[DEBUG %s (%d)] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

   #define BOOTLOADER_DEBUG(fmt, ...) \
       uart_writef("[BOOTLOADER:DEBUG %s (%d)] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
   #define BOOTLOADER_DEBUG(fmt, ...) \
       uart_writef("[BOOTLOADER:DEBUG %s (%d)] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
   #define UART_DEBUG(fmt, ...) \
       uart_writef("[UART:DEBUG %s (%d)] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
   #define UART_ERROR(fmt, ...) \
       uart_writef("[UART:ERROR %s (%d)] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
   #define CRYPTO_ERROR(fmt, ...) \
       uart_writef("[CRYPTO:ERROR %s (%d)] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
   #define CRYPTO_DEBUG(fmt, ...) \
       uart_writef("[CRYPTO:DEBUG %s (%d)] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
   #define FLASHER_ERROR(fmt, ...) \
       uart_writef("[FLASHER:ERROR %s (%d)] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
   #define FLASHER_DEBUG(fmt, ...) \
       uart_writef("[FLASHER:DEBUG%s (%d)] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else 
   #define DEBUG_PRINT(fmt, ...)
   #define BOOTLOADER_DEBUG(fmt, ...) 
   #define BOOTLOADER_DEBUG(fmt, ...) 
   #define UART_DEBUG(fmt, ...) 
   #define UART_ERROR(fmt, ...) 
   #define CRYPTO_ERROR(fmt, ...) 
   #define CRYPTO_DEBUG(fmt, ...) 
   #define FLASHER_ERROR(fmt, ...) 
   #define FLASHER_DEBUG(fmt, ...) 
#endif 

#define DBG_UART_BAUDRATE 115200 
//#define DBG_UART_BAUDRATE 9600


void dump_memory(const void* addr, u32 size, void (*print)(const char*, ...));
#endif
