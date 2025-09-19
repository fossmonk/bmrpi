#include "rand.h"

static unsigned long int next = 1;

int rand(void) {
    // RAND_MAX assumed to be 32767 for this example
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % RAND_MAX;
}

int randstr(char *buf, int len) {
    char ascii_low = 32, ascii_high = 127;
    unsigned int seed = rand();
    srand(seed);
    int i = 0;
    for(i = 0; i < len + 1; i++) {
        buf[i] = (char)((rand() % (ascii_high - ascii_low)) + ascii_low);
    }
    buf[i] = '\0';
    return i;
}

void srand(unsigned int seed) {
    next = seed;
}