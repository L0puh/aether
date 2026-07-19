#ifndef DEBUG_H
#define DEBUG_H

#include "target.h"
#include "core/uart.h"


#ifdef _DEBUG
   #define PRINT_MODULE(module, ...) \
      uart_writef("%s> ", module); \
      uart_writef(__VA_ARGS__); \
      uart_writef("\r\n") 

   #define DEBUG_PRINT(...)      PRINT_MODULE("*", __VA_ARGS__)
   #define ERROR_PRINT(...)      PRINT_MODULE("ERROR", __VA_ARGS__)
   #define PLAIN_PRINT(...)      uart_writef(__VA_ARGS__)
#else 
   #define DEBUG_PRINT(fmt, ...) ((void)0)
   #define ERROR_PRINT(fmt, ...) ((void)0)
#endif

#define DBG_UART_BAUDRATE 115200 
//#define DBG_UART_BAUDRATE 9600

void dump_memory(const void* addr, u32 size, void (*print)(const char*, ...));

#endif
