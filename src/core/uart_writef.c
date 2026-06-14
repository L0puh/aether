#include "defs.h"
#include <aether.h>
#include <stdarg.h>

static void print_char(char c);
static void print_string(const char* s);
static void print_int(int val, int width, char pad);
static void print_uint(unsigned int val, int width, char pad);
static void print_hex(unsigned int val, int width, char pad, int uppercase);
static void print_ptr(void* ptr);

static void print_char(char c) {
    uart_putchar(c);
}

static void print_string(const char* s) {
    if (!s) s = "(null)";
    while (*s) {
        print_char(*s++);
    }
}

static void print_int(int val, int width, char pad) {
    if (val < 0) {
        print_char('-');
        val = -val;
    }
    print_uint((unsigned int)val, width, pad);
}

static void print_uint(unsigned int val, int width, char pad) {
    char digits[12];
    int pos = 0;
    
    if (val == 0) {
        digits[pos++] = '0';
    } else {
        while (val > 0) {
            digits[pos++] = '0' + (val % 10);
            val /= 10;
        }
    }
    
    while (pos < width) {
        print_char(pad);
    }
    
    while (pos > 0) {
        print_char(digits[--pos]);
    }
}

static void print_hex(unsigned int val, int width, char pad, int uppercase) {
    char digits[11];
    int pos = 0;
    char hex_char = uppercase ? 'A' : 'a';
    
    if (val == 0) {
        digits[pos++] = '0';
    } else {
        while (val > 0) {
            int digit = val & 0xF;
            digits[pos++] = (digit < 10) ? ('0' + digit) : (hex_char + digit - 10);
            val >>= 4;
        }
    }
    
    while (pos < width) {
        print_char(pad);
    }
    
    while (pos > 0) {
        print_char(digits[--pos]);
    }
}

static void print_ptr(void* ptr) {
    print_string("0x");
    print_hex((unsigned int)ptr, 8, '0', 0);
}

void uart_writef(const char* str, ...) {
    va_list args;
    va_start(args, str);
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '%' && str[i+1] != '\0') {
            i++;
            
            int width = 0;
            char pad = ' ';
            int is_long = 0;
            
            if (str[i] == '0') {
                pad = '0';
                i++;
            }
            
            while (str[i] >= '0' && str[i] <= '9') {
                width = width * 10 + (str[i] - '0');
                i++;
            }
            
            if (str[i] == 'l') {
                is_long = 1;
                i++;
                if (str[i] == 'l') {
                    i++;
                }
            }
            
            switch (str[i]) {
                case 's': {
                    char* s = va_arg(args, char*);
                    print_string(s);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    print_char(c);
                    break;
                }
                case 'd':
                case 'i': {
                    int val;
                    if (is_long) {
                        val = (int)va_arg(args, long);
                    } else {
                        val = va_arg(args, int);
                    }
                    print_int(val, width, pad);
                    break;
                }
                case 'u': {
                    unsigned int val;
                    if (is_long) {
                        val = (unsigned int)va_arg(args, unsigned long);
                    } else {
                        val = va_arg(args, unsigned int);
                    }
                    print_uint(val, width, pad);
                    break;
                }
                case 'x': {
                    unsigned int val;
                    if (is_long) {
                        val = (unsigned int)va_arg(args, unsigned long);
                    } else {
                        val = va_arg(args, unsigned int);
                    }
                    print_hex(val, width, pad, 0);
                    break;
                }
                case 'X': {
                    unsigned int val;
                    if (is_long) {
                        val = (unsigned int)va_arg(args, unsigned long);
                    } else {
                        val = va_arg(args, unsigned int);
                    }
                    print_hex(val, width, pad, 1);
                    break;
                }
                case 'p': {
                    void* ptr = va_arg(args, void*);
                    print_ptr(ptr);
                    break;
                }
                case '%': {
                    print_char('%');
                    break;
                }
                default: {
                    print_char('%');
                    print_char(str[i]);
                    break;
                }
            }
        } else {
            print_char(str[i]);
        }
    }
    
    va_end(args);
}

#ifdef _DEBUG
void dump_memory(const void* addr, u32 size, void (*print)(const char*, ...))
{
   const u8* ptr = (const u8*)addr;

   if (addr == NULL || size == 0) {
      print("(null or empty)\r\n");
      return;
   }

   u32 dump_size = size;
   if (dump_size > 256) {
      print("(dumping first 256 of %u bytes)\r\n", (unsigned int)size);
      dump_size = 256;
   }

   print("\r\n");

   for (u32 i = 0; i < dump_size; i += 16) {
      print("%x: ", (unsigned int)(ptr + i));

      for (u32 j = 0; j < 16; j++) {
         if (i + j < dump_size) {
            unsigned int byte = ptr[i + j];
            if (byte < 0x10) print("0");
            print("%x", byte);
         } else {
            print(" ");
         }

         if (j < 15) {
            print(" ");
         }

         if (j == 7) {
            print(" ");
         }
      }

      print("  |");
      for (u32 j = 0; j < 16 && i + j < dump_size; j++) {
         u8 c = ptr[i + j];
         if (c >= 0x20 && c < 0x7F) {
            print("%c", c);
         } else {
            print(".");
         }
      }
      print("|\r\n");
   }

   print("\r\n");

   if (size > dump_size) {
      print("(... %u more bytes)\r\n", (unsigned int)(size - dump_size));
   }
}
#else
void dump_memory(const void* addr, u32 size, void (*print)(const char*, ...))
{
   (void)addr;
   (void)size;
   (void)print;
}
#endif
