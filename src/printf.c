#include <stdarg.h>
#include "printf.h"
#include "io.h"

// Function to print a signed integer.
// It uses a buffer and handles negative numbers.
void print_int(int num) {
    char buffer[12]; // Enough for a 32-bit integer and a sign
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        uart_putc('0');
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    // Convert digits in reverse order
    while (num != 0) {
        buffer[i++] = (num % 10) + '0';
        num = num / 10;
    }

    // Add sign if negative
    if (is_negative) {
        buffer[i++] = '-';
    }

    // Print digits in the correct order
    while (i > 0) {
        uart_putc(buffer[--i]);
    }
}

// Function to print an unsigned integer as a hexadecimal number.
// It uses a lookup table for hex digits.
void print_hex(unsigned int num) {
    char buffer[9]; // Enough for 8 hex digits and null terminator
    const char hex_digits[] = "0123456789ABCDEF";
    int i = 0;

    for(int k = 0; k < 9; k++) buffer[k] = '0';

    if (num == 0) {
        uart_print("00000000");
        return;
    }

    // Convert digits in reverse order
    while (num != 0 && i < 8) {
        buffer[i++] = hex_digits[num % 16];
        num /= 16;
    }

    // Print digits in the correct order
    while (i > 0) {
        uart_putc(buffer[--i]);
    }
}

void print_float(double num) {
    long integer_part = (long)num;
    double fractional_part = num - (double)integer_part;

    if (num < 0) {
        uart_putc('-');
        num = -num;
        integer_part = (long)num;
        fractional_part = num - (double)integer_part;
    }

    // Print integer part
    print_int(integer_part);
    uart_putc('.');

    // Print fractional part
    for (int i = 0; i < 6; i++) {
        fractional_part *= 10;
        int digit = (int)fractional_part;
        uart_putc(digit + '0');
        fractional_part -= digit;
    }
}

void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd':
                    print_int(va_arg(args, int));
                    break;
                case 'f':
                    print_float(va_arg(args, double));
                    break;
                case 'c':
                    uart_putc((char)va_arg(args, int));
                    break;
                case 's':
                    uart_print(va_arg(args, char *));
                    break;
                case 'x':
                    print_hex(va_arg(args, unsigned int));
                    break;
                case '%':
                    uart_putc('%');
                    break;
                default:
                    // Print an unknown specifier literally.
                    uart_putc('%');
                    uart_putc(*format);
                    break;
            }
        } else {
            uart_putc(*format);
        }
        format++;
    }

    va_end(args);
}
