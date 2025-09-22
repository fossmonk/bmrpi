#include <stdint.h>
#include "sprite.h"
#include "gfx.h"
#include "dma.h"
#include "printf.h"

/* Pre Allocate Memory for Sprite Table */
sprite_t g_sprite_table[MAX_SPRITES] __attribute__((section(".bss.sp_table"))) __attribute__((aligned(32)));
uint32_t g_sprite_count __attribute__((aligned(32))) = 0;

int sprite_register(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t *sprite_data) {
    if(g_sprite_count < MAX_SPRITES) {
        int i = g_sprite_count;
        g_sprite_table[i].width = w;
        g_sprite_table[i].height = h;
        g_sprite_table[i].xpos = x;
        g_sprite_table[i].ypos = y;
        g_sprite_table[i].prev_xpos = x;
        g_sprite_table[i].prev_ypos = y;
        g_sprite_table[i].age = 0;
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

int sprite_get_count(void) {
    return g_sprite_count;
}

sprite_t* sprite_get_table(void) {
    return g_sprite_table;
}

void sprite_draw(sprite_t *s, int x, int y) {
    int offset = (y * gfx_get_pitch() + x*4);
    dma_channel* sch = dma_open_channel(CT_NORMAL);
    uint32_t gbuf = gfx_get_buffer();
    dma_setup_2dmem_copy(sch,
                         (void *)(uintptr_t)(gbuf + offset),
                         (void *)((uintptr_t)s->buffer),
                         (s->width)*4, (s->height), PD_WIDTH*4, 8);
    // source stride is 0, this is lower 15 bits of stride reg. Zero it out. Setup API assumes same stride.
    sch->block->stride &= 0xFFFF0000;
    dma_start(sch);
    dma_wait(sch);
    dma_close_channel(sch);
}

void sprite_put(sprite_t *s) {
    if(s->buffer != NULL) {
        int x = s->xpos;
        int y = s->ypos;
        int offset = (y*gfx_get_pitch() + x*4);
        dma_channel* sch = dma_open_channel(CT_NORMAL);
        uint32_t gbuf = gfx_get_buffer();
        dma_setup_2dmem_copy(sch,
                             (void *)(uintptr_t)(gbuf + offset),
                             (void *)((uintptr_t)s->buffer),
                             (s->width)*4, (s->height), PD_WIDTH*4, 8);
        // source stride is 0, this is lower 15 bits of stride reg. Zero it out. Setup API assumes same stride.
        sch->block->stride &= 0xFFFF0000;
        dma_start(sch);
        dma_wait(sch);
        dma_close_channel(sch);
    }
}

void sprite_put_direct(sprite_t *s) {
    int x = s->xpos;
    int y = s->ypos;
    int offset = (y*gfx_get_pitch() + x*4);
    dma_channel* sch = dma_open_channel(CT_NORMAL);
    uint32_t fb = gfx_get_fb_bus_addr();
    dma_setup_2dmem_copy(sch,
                         (void *)(uintptr_t)(fb + offset),
                         (void *)((uintptr_t)s->buffer),
                         (s->width)*4, (s->height), PD_WIDTH*4, 8);
    // source stride is 0, this is lower 15 bits of stride reg. Zero it out. Setup API assumes same stride.
    sch->block->stride &= 0xFFFF0000;
    dma_start(sch);
    dma_wait(sch);
    dma_close_channel(sch);
}