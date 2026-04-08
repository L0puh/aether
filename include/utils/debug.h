#ifndef DEBUG_H
#define DEBUG_H


#ifdef _DEBUG
   #define BOOTLOADER_DEBUG(...) printf(__VA_ARGS__);
   #define BOOTLOADER_ERROR(...) printf(__VA_ARGS__);
   #define UART_ERROR(...) printf(__VA_ARGS__);
   #define UART_DEBUG(...) printf(__VA_ARGS__);
#else 
   #define BOOTLOADER_DEBUG(...)
   #define BOOTLOADER_ERROR(...)
   #define UART_ERROR(...)
   #define UART_DEBUG(...)
#endif 

#endif
