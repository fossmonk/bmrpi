#include <stdint.h>

#ifndef _SPRITE_H_
#define _SPRITE_H_

#define MAX_SPRITES (100)

typedef struct {
    uint16_t width;
    uint16_t height;
    uint32_t* buffer;
    uint16_t xpos;
    uint16_t ypos;
    uint16_t prev_xpos;
    uint16_t prev_ypos;
    uint32_t age;
} sprite_t;

int sprite_register(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t *sprite_data);
sprite_t* sprite_get(int s_idx);
void sprite_draw(sprite_t *s, int x, int y);
int sprite_get_count(void);
sprite_t* sprite_get_table(void);
void sprite_put(sprite_t *s);
void sprite_put_direct(sprite_t *s);

#endif