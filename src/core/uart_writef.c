#include "defs.h"
#include <aether.h>
#include <stdarg.h>
#include <stdint.h>


static void out_char(char c) {
    uart_putchar(c);
}

static void out_str(const char *s) {
    if (!s) s = "(null)";
    while (*s) out_char(*s++);
}

static void out_repeat(char c, int count) {
    while (count-- > 0) out_char(c);
}

static int format_digits(char *buf, int buf_size, unsigned long val, int base, int uppercase) {
    const char *alphabet = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    char tmp[32];
    int n = 0;

    if (val == 0) {
        tmp[n++] = '0';
    } else {
        while (val > 0 && n < (int)sizeof(tmp)) {
            tmp[n++] = alphabet[val % base];
            val /= base;
        }
    }

    int len = (n < buf_size) ? n : buf_size;
    for (int i = 0; i < len; i++) {
        buf[i] = tmp[len - 1 - i];
    }
    return len;
}

static void emit_number(unsigned long val, int is_negative, int base,
                         int uppercase, int width, char pad) {
    char digits[32];
    int len = format_digits(digits, sizeof(digits), val, base, uppercase);
    int sign_len = is_negative ? 1 : 0;
    int total_len = len + sign_len;
    int pad_count = width - total_len;
    if (pad_count < 0) pad_count = 0;

    if (pad == '0') {
        if (is_negative) out_char('-');
        out_repeat('0', pad_count);
    } else {
        out_repeat(' ', pad_count);
        if (is_negative) out_char('-');
    }

    for (int i = 0; i < len; i++) out_char(digits[i]);
}

static void emit_ptr(void *ptr) {
    out_str("0x");
    emit_number((unsigned long)(uintptr_t)ptr, 0, 16, 0, 8, '0');
}

typedef struct {
    int width;
    char pad;
    int is_long;
} fmt_spec_t;

static fmt_spec_t parse_spec(const char *str, int *i) {
    fmt_spec_t spec = { .width = 0, .pad = ' ', .is_long = 0 };

    if (str[*i] == '0') {
        spec.pad = '0';
        (*i)++;
    }

    while (str[*i] >= '0' && str[*i] <= '9') {
        spec.width = spec.width * 10 + (str[*i] - '0');
        (*i)++;
    }

    if (str[*i] == 'l') {
        spec.is_long = 1;
        (*i)++;
        if (str[*i] == 'l') (*i)++; 
    }

    return spec;
}

void uart_writef(const char *str, ...) {
    va_list args;
    va_start(args, str);

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != '%' || str[i + 1] == '\0') {
            out_char(str[i]);
            continue;
        }

        i++;
        fmt_spec_t spec = parse_spec(str, &i);
        char conv = str[i];

        switch (conv) {
            case 's':
                out_str(va_arg(args, char*));
                break;

            case 'c':
                out_char((char)va_arg(args, int));
                break;

            case 'd':
            case 'i': {
                long val = spec.is_long ? va_arg(args, long) : (long)va_arg(args, int);
                int neg = val < 0;
                unsigned long uval = neg ? (unsigned long)(-val) : (unsigned long)val;
                emit_number(uval, neg, 10, 0, spec.width, spec.pad);
                break;
            }

            case 'u': {
                unsigned long val = spec.is_long ? va_arg(args, unsigned long)
                                                  : (unsigned long)va_arg(args, unsigned int);
                emit_number(val, 0, 10, 0, spec.width, spec.pad);
                break;
            }

            case 'x':
            case 'X': {
                unsigned long val = spec.is_long ? va_arg(args, unsigned long)
                                                  : (unsigned long)va_arg(args, unsigned int);
                emit_number(val, 0, 16, conv == 'X', spec.width, spec.pad);
                break;
            }

            case 'p':
                emit_ptr(va_arg(args, void*));
                break;

            case '%':
                out_char('%');
                break;

            default:
                out_char('%');
                out_char(conv);
                break;
        }
    }

    va_end(args);
}

#ifdef _DEBUG
void dump_memory(const void *addr, u32 size, void (*print)(const char *, ...)) {
    const u8 *ptr = (const u8*)addr;

    if (addr == NULL || size == 0) {
        print("(null or empty)\r\n");
        return;
    }

    u32 dump_size = size > 256 ? 256 : size;
    if (dump_size < size) {
        print("(dumping first 256 of %u bytes)\r\n", (unsigned int)size);
    }

    print("\r\n");
    for (u32 i = 0; i < dump_size; i += 16) {
        print("%x: ", (unsigned int)(uintptr_t)(ptr + i));

        for (u32 j = 0; j < 16; j++) {
            if (i + j < dump_size) {
                print("%02x", ptr[i + j]);
            } else {
                print("  ");
            }
            if (j < 15) print(" ");
            if (j == 7) print(" ");
        }

        print("  |");
        for (u32 j = 0; j < 16 && i + j < dump_size; j++) {
            u8 c = ptr[i + j];
            print("%c", (c >= 0x20 && c < 0x7F) ? c : '.');
        }
        print("|\r\n");
    }
    print("\r\n");

    if (size > dump_size) {
        print("(... %u more bytes)\r\n", (unsigned int)(size - dump_size));
    }
}
#else
void dump_memory(const void *addr, u32 size, void (*print)(const char *, ...)) {
    (void)addr; (void)size; (void)print;
}
#endif
