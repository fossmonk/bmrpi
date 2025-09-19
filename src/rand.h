#ifndef _RAND_H_
#define _RAND_H_

#define RAND_MAX (32768)

int rand(void);
int randstr(char *buf, int len);
void srand(unsigned int seed);

#endif /* _RAND_H_ */