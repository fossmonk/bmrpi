#include "../../src/hw.h"
#include "../../src/io.h"
#include "../../src/gfx.h"
#include "../../src/colors.h"
#include "../../src/strops.h"
#include "../../src/rand.h"
#include "../../src/shell.h"
#include "../../src/dma.h"
#include "../../src/printf.h"

#include "sprite/sprite.h"

#define MARGIN (20)

void sprite_loop(void);

dma_channel *sch;
uint32_t gfx_pitch, fb_addr;

void naive_delay(int i) {
    while(i--) {
        int k = 500000;
        while(k--);
    }
}

void kernel_main() {
    uart_init();
    gfx_init();
    sch = dma_open_channel(CT_NORMAL);
    gfx_pitch = gfx_get_pitch();
    fb_addr = gfx_get_fb_bus_addr();
    sprite_loop();
    // spin
    while(1);
}

uint32_t prev_s_w = 0;
uint32_t prev_s_h = 0;
uint32_t prev_s_x = 0;
uint32_t prev_s_y = 0;
uint32_t velocity = 2;

void draw_sprite(const sprite_t *s, int x, int y) {
    int offset = (y * gfx_pitch + x*4);
    dma_setup_2dmem_copy(sch, (void *)(uintptr_t)(fb_addr + offset),
                        (void *)((uintptr_t)s->buffer),
                        (s->width)*4, (s->height), PD_WIDTH*4, 2);
    // source stride is 0, this is lower 15 bits of stride reg
    sch->block->stride &= 0xFFFF0000;
    dma_start(sch);
    dma_wait(sch);
}

int xdir = 1;
int ydir = -1;

void sprite_loop(void) {
    /* Init first sprite */
    int s_idx = V_MAGIC;
    const sprite_t *s = sprite_get(s_idx);
    prev_s_w = s->width;
    prev_s_h = s->height;
    int x = 0;
    int y = (PD_HEIGHT - (s->height + MARGIN));
    prev_s_x = x;
    prev_s_y = y;

    /* Draw sprite */
    draw_sprite(s, x, y);
    wait_msec(600);

    while(1) {
        // Clear previous sprite
        gfx_clear_rect(prev_s_x, prev_s_y, prev_s_x + prev_s_w, prev_s_y + prev_s_h);

        // Init next sprite
        s = sprite_get(s_idx);
        prev_s_w = s->width;
        prev_s_h = s->height;
        x += xdir*velocity;
        y += ydir*velocity;

        // whenever there is a collision, switch the sprite
        if(x >= (PD_WIDTH - s->width)) {
            x -= 2*velocity;
            xdir = -1;
            s_idx = (s_idx + 1) % V_COUNT;
        }

        if(x <= 0) {
            x += 2*velocity;
            xdir = 1;
            s_idx = (s_idx + 1) % V_COUNT;
        }

        if(y >= (PD_HEIGHT - s->height)) {
            y -= 2*velocity;
            ydir = -1;
            s_idx = (s_idx + 1) % V_COUNT;
        }

        if(y <= 0) {
            y += 2*velocity;
            ydir = 1;
            s_idx = (s_idx + 1) % V_COUNT;
        }

        prev_s_x = x;
        prev_s_y = y;

        draw_sprite(s, x, y);
        wait_msec(600);
    }
}