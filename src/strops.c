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