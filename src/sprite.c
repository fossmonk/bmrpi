#include <stdint.h>
#include "sprite.h"
#include "gfx.h"
#include "dma.h"
#include "printf.h"

/* Pre Allocate Memory for Sprite Table */
sprite_t g_sprite_table[MAX_SPRITES] __attribute__((section(".bss.sp_table")));
uint32_t g_sprite_count = 0;

int sprite_register(uint32_t w, uint32_t h, uint32_t *sprite_data) {
    if(g_sprite_count < MAX_SPRITES) {
        int i = g_sprite_count;
        g_sprite_table[i].width = w;
        g_sprite_table[i].height = h;
        g_sprite_table[i].buffer = sprite_data;
        g_sprite_count++;
        return 0;
    }
    return -1;
}

sprite_t* sprite_get(int s_idx) {
    if(s_idx < g_sprite_count) {
        return &g_sprite_table[s_idx];
    }
    return NULL;
}

void sprite_draw(sprite_t *s, int x, int y) {
    int offset = (y * gfx_get_pitch() + x*4);
    dma_channel* sch = dma_open_channel(CT_NORMAL);
    uint32_t gbuf = gfx_get_buffer();
    dma_setup_2dmem_copy(sch,
                         (void *)(uintptr_t)(gbuf + offset),
                         (void *)((uintptr_t)s->buffer),
                         (s->width)*4, (s->height), PD_WIDTH*4, 2);
    // source stride is 0, this is lower 15 bits of stride reg. Zero it out. Setup API assumes same stride.
    sch->block->stride &= 0xFFFF0000;
    dma_start(sch);
    dma_wait(sch);
    dma_close_channel(sch);
}