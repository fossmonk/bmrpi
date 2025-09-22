#include <stdint.h>

#ifndef _STROPS_H_
#define _STROPS_H_

int strops_len(const char *s);
int strops_cmp(const char* s1, const char* s2);
int strops_ncmp(const char* s1, const char* s2, int n);
int strops_copy(char *dst, const char *src);
int strops_atoi(const char *str);
void strops_u2hex(uint32_t value, char *out);
void strops_num2str(int value, char *out);
uint32_t strops_htoi(char *hex_string);

#endif /* _STROPS_H_ */