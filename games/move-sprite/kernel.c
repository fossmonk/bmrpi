#include "../../src/hw.h"
#include "../../src/io.h"
#include "../../src/gfx.h"
#include "../../src/colors.h"
#include "../../src/strops.h"
#include "../../src/rand.h"
#include "../../src/shell.h"
#include "../../src/dma.h"
#include "../../src/printf.h"

#include "../../src/sprite.h"

#define MARGIN (20)

extern uint32_t v_main[];
extern uint32_t v_magic[];
extern uint32_t v_side[];
extern uint32_t v_jump[];
extern uint32_t v_run[];

enum {
    V_MAIN,
    V_SIDE,
    V_JUMP,
    V_RUN,
    V_MAGIC,
    V_COUNT,
};

void draw_loop(void);

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
    // register sprites
    sprite_register(150, 200, v_main);
    sprite_register(150, 200, v_side);
    sprite_register(150, 200, v_jump);
    sprite_register(150, 200, v_run);
    sprite_register(150, 200, v_magic);
    // Init DMA_CH for sprite

    sch = dma_open_channel(CT_NORMAL);
    gfx_pitch = gfx_get_pitch();
    fb_addr = gfx_get_fb_bus_addr();
    draw_loop();
    // spin
    while(1);
}

uint32_t prev_s_w = 0;
uint32_t prev_s_h = 0;
uint32_t prev_s_x = 0;
uint32_t prev_s_y = 0;
uint32_t velocity = 10;

int xdir = 1;
int ydir = -1;

void draw_loop(void) {
    /* Init first sprite */
    int s_idx = V_MAIN;
    sprite_t *s = sprite_get(s_idx);
    prev_s_w = s->width;
    prev_s_h = s->height;
    int x = 0;
    int y = (PD_HEIGHT - (s->height + MARGIN));
    prev_s_x = x;
    prev_s_y = y;

    /* Draw sprite */
    sprite_draw(s, x, y);
    wait_msec(600);

    while(1) {
        // Get input, non-blocking
        int input = uart_getc_nb();

        if(input != -1) {
            // Update the sprite
            switch(input) {
                case ' ':
                    // jump
                    break;
                case 'p': // change sprite
                    // Get new sprite
                    s_idx = (s_idx + 1) % V_COUNT;
                    s = sprite_get(s_idx);
                    // Clear previous sprite
                    gfx_clear_rect(prev_s_x, prev_s_y, prev_s_x + prev_s_w, prev_s_y + prev_s_h);
                    sprite_draw(s, x, y);
                    break;
                case 'a': // move left
                    // Update x position
                    if(x > velocity) {
                        x -= velocity;
                        // Clear previous sprite
                        gfx_clear_rect(prev_s_x, prev_s_y, prev_s_x + prev_s_w, prev_s_y + prev_s_h);
                        sprite_draw(s, x, y);
                        prev_s_x = x;
                    }
                    break;
                case 'd': // move right
                    // Update x position
                    if((x + velocity) < (PD_WIDTH - s->width)) {
                        x += velocity;
                        // Clear previous sprite
                        gfx_clear_rect(prev_s_x, prev_s_y, prev_s_x + prev_s_w, prev_s_y + prev_s_h);
                        sprite_draw(s, x, y);
                        prev_s_x = x;
                    }
                    break;
                default:
                    break;
            }
        }
        
        wait_msec(100);
    }
}