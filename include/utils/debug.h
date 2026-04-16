#ifndef DEBUG_H
#define DEBUG_H

#include <core/uart.h>
#define _DEBUG
#ifdef _DEBUG
   #define BOOTLOADER_DEBUG(...) uart_write(__VA_ARGS__);
   #define BOOTLOADER_ERROR(...) uart_write(__VA_ARGS__);
   #define UART_ERROR(...)       uart_write(__VA_ARGS__);
   #define UART_DEBUG(...)       uart_write(__VA_ARGS__);
#else 
   #define BOOTLOADER_DEBUG(...)
   #define BOOTLOADER_ERROR(...)
   #define UART_ERROR(...)
   #define UART_DEBUG(...)
#endif 

#define DBG_UART_BAUDRATE 115200

#endif
