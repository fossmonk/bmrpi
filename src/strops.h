#include <stdint.h>

#ifndef _STROPS_H_
#define _STROPS_H_

int strops_cmp(const char* s1, const char* s2);
int strops_ncmp(const char* s1, const char* s2, int n);
int strops_copy(char *dst, const char *src);
int strops_atoi(const char *str);

#endif /* _STROPS_H_ */