#ifndef DEBUG_H
#define DEBUG_H


#ifdef _DEBUG
   #define BOOTLOADER_DEBUG(...) printf(__VA_ARGS__);
#else 
   #define BOOTLOADER_DEBUG(...)
#endif 

#endif
