#include "strops.h"
#include <limits.h>

int strops_cmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strops_ncmp(const char* s1, const char* s2, int n) {
    int flag = 0;
    for(int i = 0; i < n; i++) {
        if(s1[i] != s2[i]) {
            flag = -1;
            break;
        }
    }
    return flag;
}

int strops_copy(char *dst, const char *src) {
    while(*src) *dst++ = *src++;
}

int strops_len(const char *s) {
    int count = 0;
    while(*s++ != '\0')count++;
    return count;
}

void strops_u2hex(uint32_t value, char *out) {
    static const char hex[] = "0123456789ABCDEF";

    out[0] = '0';
    out[1] = 'x';
    for (int i = 0; i < 8; i++) {
        // Extract nibble from most significant to least
        uint32_t nibble = (value >> ((7 - i) * 4)) & 0xF;
        out[2 + i] = hex[nibble];
    }
    out[10] = '\0';
}

void strops_num2str(int value, char *out) {
    // assume out has enough size

    int i = value;
    if(value < 0)i = -value;
    int digit = 0;
    while(i) {
        out[digit++] = (i % 10) + '0';
        i /= 10;
    }
    if(value < 0)out[digit++] = '-';
    for(int k = 0; k < (digit+1)/2; k++) {
        char temp = out[k];
        out[k] = out[digit-1-k];
        out[digit-1-k] = temp;
    }
    out[digit] = '\0';
}

static int8_t hex_char_to_int(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1; // Indicates an error
}

uint32_t strops_htoi(char *hex_string) {
    uint32_t result = 0;

    // Skip the "0x" prefix if it exists
    if (hex_string[0] == '0' && (hex_string[1] == 'x' || hex_string[1] == 'X')) {
        hex_string += 2;
    }

    // Process each character in the string
    while (*hex_string != '\0') {
        int8_t value = hex_char_to_int(*hex_string);
        if (value == -1) {
            // Stop on the first non-hexadecimal character
            break;
        }
        
        // Left-shift the current result by 4 bits and add the new digit
        result = (result << 4) | value;
        hex_string++;
    }

    return result;
}

int strops_atoi(const char *str) {
    int result = 0;
    int sign = 1;
    int i = 0;

    // 1. Skip leading whitespace characters
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' ||
           str[i] == '\r' || str[i] == '\f' || str[i] == '\v') {
        i++;
    }

    // 2. Handle optional sign
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    // 3. Convert digit characters to integer
    while (str[i] >= '0' && str[i] <= '9') {
        int digit = str[i] - '0';

        // Check for overflow before adding the new digit
        if (sign == 1 && (result > INT_MAX / 10 || (result == INT_MAX / 10 && digit > INT_MAX % 10))) {
            return INT_MAX;
        }
        if (sign == -1 && (result < INT_MIN / 10 || (result == INT_MIN / 10 && digit > -(INT_MIN % 10)))) {
            return INT_MIN;
        }

        result = result * 10 + digit;
        i++;
    }

    return sign * result;
}