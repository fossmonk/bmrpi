#include <stdint.h>

#ifndef _SPRITE_H_
#define _SPRITE_H_

typedef struct {
    uint32_t width;
    uint32_t height;
    const uint32_t* buffer;
} sprite_t;

typedef enum {
    V_MAIN,
    V_SIDE,
    V_RUN,
    V_JUMP,
    V_MAGIC,
    V_COUNT
} SPRITE;

const sprite_t* sprite_get(SPRITE s);

#endif