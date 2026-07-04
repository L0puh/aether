#ifndef DEBUG_H
#define DEBUG_H

#include "target.h"
#include "core/uart.h"


#ifdef _DEBUG
   #define DEBUG_PRINT(...)      uart_writef(__VA_ARGS__)
   #define BOOTLOADER_DEBUG(...) uart_writef(__VA_ARGS__)
   #define BOOTLOADER_ERROR(...) uart_writef(__VA_ARGS__)
   #define UART_ERROR(...)       uart_writef(__VA_ARGS__)
   #define UART_DEBUG(...)       uart_writef(__VA_ARGS__)
   #define FLASHER_DEBUG(...)    uart_writef(__VA_ARGS__)
   #define FLASHER_ERROR(...)    uart_writef(__VA_ARGS__)
   #define CRYPTO_DEBUG(...)     uart_writef(__VA_ARGS__)
   #define CRYPTO_ERROR(...)     uart_writef(__VA_ARGS__)
   #define MPU_DEBUG(...)        uart_writef(__VA_ARGS__)

#else 
   #define DEBUG_PRINT(fmt, ...) ((void)0)
   #define BOOTLOADER_DEBUG(fmt, ...) ((void)0)
   #define UART_DEBUG(fmt, ...) ((void)0)
   #define UART_ERROR(fmt, ...) ((void)0)
   #define CRYPTO_ERROR(fmt, ...) ((void)0)
   #define CRYPTO_DEBUG(fmt, ...) ((void)0)
   #define FLASHER_ERROR(fmt, ...) ((void)0)
   #define FLASHER_DEBUG(fmt, ...) ((void)0)
   #define MPU_DEBUG(...)          ((void)0) 
#endif 

#define DBG_UART_BAUDRATE 115200 
//#define DBG_UART_BAUDRATE 9600

void dump_memory(const void* addr, u32 size, void (*print)(const char*, ...));

#endif
