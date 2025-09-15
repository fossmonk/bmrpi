#include <stdint.h>

#ifndef _SPRITE_H_
#define _SPRITE_H_

#define MAX_SPRITES (10)

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t* buffer;
} sprite_t;

int sprite_register(uint32_t w, uint32_t h, uint32_t *sprite_data);
sprite_t* sprite_get(int s_idx);
void sprite_draw(sprite_t *s, int x, int y);

#endif