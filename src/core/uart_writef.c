#include <aether.h>
#include <stdarg.h>

void uart_writef(const char* str, ...)
{
   va_list args;
   va_start(args, str);

   char buffer[32]; 

   for (int i = 0; str[i] != '\0'; i++) {
      if (str[i] == '%' && str[i+1] != '\0') {
         i++; 

         int width = 0;
         char pad = ' ';
         if (str[i] == '0') {
            pad = '0';
            i++;
         }
         while (str[i] >= '0' && str[i] <= '9') {
            width = width * 10 + (str[i] - '0');
            i++;
         }

         switch (str[i]) {
            case 's': {
               char *s = va_arg(args, char*);
               uart_write(s ? s : "(null)");
               break;
            }
            case 'c': {
               char c = (char)va_arg(args, int);
               uart_putchar(c);
               break;
            }
            case 'd':
            case 'i': {
               int val = va_arg(args, int);
               int pos = 0;
               int neg = 0;

               if (val < 0) {
                  neg = 1;
                  val = -val;
               }

               if (val == 0) {
                  buffer[pos++] = '0';
               } else {
                  while (val > 0) {
                     buffer[pos++] = '0' + (val % 10);
                     val /= 10;
                  }
               }

               while (pos + neg < width) {
                  buffer[pos++] = pad;
               }

               if (neg) buffer[pos++] = '-';

               while (pos > 0) {
                  uart_putchar(buffer[--pos]);
               }
               break;
            }
            case 'u': {
               unsigned int val = va_arg(args, unsigned int);
               int pos = 0;

               if (val == 0) {
                  buffer[pos++] = '0';
               } else {
                  while (val > 0) {
                     buffer[pos++] = '0' + (val % 10);
                     val /= 10;
                  }
               }

               while (pos < width) {
                  buffer[pos++] = pad;
               }

               while (pos > 0) {
                  uart_putchar(buffer[--pos]);
               }
               break;
            }
            case 'x':
            case 'X': {
               unsigned int val = va_arg(args, unsigned int);
               int pos = 0;
               char hex_char = (str[i] == 'X') ? 'A' : 'a';

               if (val == 0) {
                  buffer[pos++] = '0';
               } else {
                  while (val > 0) {
                     int digit = val % 16;
                     buffer[pos++] = (digit < 10) ? ('0' + digit) : (hex_char + digit - 10);
                     val /= 16;
                  }
               }

               while (pos < width) {
                  buffer[pos++] = pad;
               }

               while (pos > 0) {
                  uart_putchar(buffer[--pos]);
               }
               break;
            }
            case 'p': {
               void *ptr = va_arg(args, void*);
               uart_write("0x");
               unsigned int val = (unsigned int)ptr;
               int pos = 0;

               if (val == 0) {
                  buffer[pos++] = '0';
               } else {
                  while (val > 0) {
                     int digit = val % 16;
                     buffer[pos++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
                     val /= 16;
                  }
               }

               while (pos > 0) {
                  uart_putchar(buffer[--pos]);
               }
               break;
            }
            case '%': {
               uart_putchar('%');
               break;
            }
            case 'l':
               i++;
               if (str[i] == 'l') i++; // Handle 'll'
               if (str[i] == 'u') {
                  unsigned int val = va_arg(args, unsigned int);
                  int pos = 0;

                  if (val == 0) {
                     buffer[pos++] = '0';
                  } else {
                     while (val > 0) {
                        buffer[pos++] = '0' + (val % 10);
                        val /= 10;
                     }
                  }

                  while (pos < width) {
                     buffer[pos++] = pad;
                  }

                  while (pos > 0) {
                     uart_putchar(buffer[--pos]);
                  }
               } else if (str[i] == 'x' || str[i] == 'X') {
                  unsigned int val = va_arg(args, unsigned int);
                  int pos = 0;
                  char hex_char = (str[i] == 'X') ? 'A' : 'a';

                  if (val == 0) {
                     buffer[pos++] = '0';
                  } else {
                     while (val > 0) {
                        int digit = val % 16;
                        buffer[pos++] = (digit < 10) ? ('0' + digit) : (hex_char + digit - 10);
                        val /= 16;
                     }
                  }

                  while (pos < width) {
                     buffer[pos++] = pad;
                  }

                  while (pos > 0) {
                     uart_putchar(buffer[--pos]);
                  }
               }
               break;
            default:
               uart_putchar('%');
               uart_putchar(str[i]);
               break;
         }
      } else {
         uart_putchar(str[i]);
      }
   }

   va_end(args);
}

