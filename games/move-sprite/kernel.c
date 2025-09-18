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
#define JUMP_VELOCITY (40)

#define MOVE_LEFT  (1 << 0)
#define MOVE_RIGHT (1 << 1)
#define FIRE       (1 << 2)
#define JUMP       (1 << 3)

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

void kernel_main() {
    uart_init();
    gfx_init();
    // register sprites
    sprite_register(150, 200, v_main);
    sprite_register(150, 200, v_side);
    sprite_register(150, 200, v_jump);
    sprite_register(150, 200, v_run);
    sprite_register(150, 200, v_magic);
    draw_loop();
    // spin
    while(1);
}

uint32_t prev_s_w = 0;
uint32_t prev_s_h = 0;
uint32_t prev_s_x = 0;
uint32_t prev_s_y = 0;
uint32_t velocity = 10;
uint32_t acc = 5;
uint8_t in_jump = 0;

int xdir = 1;
int ydir = -1;

int jump_get_inst_pos(int pos) {
    // motion 1d, with max jump height = 160
    int v0 = JUMP_VELOCITY;
    int r = 2*v0*pos - acc*pos*pos;
    return r/2;
}

void draw_loop(void) {
    /* Init first sprite */
    int s_idx = V_SIDE;
    sprite_t *s = sprite_get(s_idx);
    prev_s_w = s->width;
    prev_s_h = s->height;
    const int x0 = 0;
    const int y0 = (PD_HEIGHT - (s->height + MARGIN));
    int x = x0, y = y0;
    prev_s_x = x;
    prev_s_y = y;
    int jump_pos = 0;
    uint32_t idle_cnt = 0;

    sprite_draw(s, x, y);
    gfx_update_display();
    
    wait_msec(600);

    while(1) {
        // Get input, non-blocking
        int input = uart_getc_nb();

        if((input != -1) && (input & 0xD0 == 0xD0)) {
            int update_display = 0;
            // Update the sprite
            if(input & JUMP) {
                // jump
                if(in_jump == 0) {
                    in_jump = 1;
                    jump_pos = 1;
                    y -= (velocity*acc);
                    // Clear previous sprite
                    gfx_clear_rect(prev_s_x, prev_s_y, prev_s_x + prev_s_w, prev_s_y + prev_s_h);
                    sprite_draw(s, x, y);
                    update_display = 1;
                    prev_s_y = y;
                }
                idle_cnt = 0;
            }

            if(input & MOVE_LEFT) {
                // Update x position
                if(x > velocity) {
                    x -= velocity;
                    // Clear previous sprite
                    gfx_clear_rect(prev_s_x, prev_s_y, prev_s_x + prev_s_w, prev_s_y + prev_s_h);
                    s = sprite_get(V_JUMP);
                    sprite_draw(s, x, y);
                    update_display = 1;
                    prev_s_x = x;
                }
                idle_cnt = 0;
            }

            if(input & MOVE_RIGHT) {
                // Update x position
                if((x + velocity) < (PD_WIDTH - s->width)) {
                    x += velocity;
                    // Clear previous sprite
                    gfx_clear_rect(prev_s_x, prev_s_y, prev_s_x + prev_s_w, prev_s_y + prev_s_h);
                    s = sprite_get(V_JUMP);
                    sprite_draw(s, x, y);
                    update_display = 1;
                    prev_s_x = x;
                }
                idle_cnt = 0;
            }

            if(input & FIRE) {
                s = sprite_get(V_MAGIC);
                // Clear previous sprite
                gfx_clear_rect(prev_s_x, prev_s_y, prev_s_x + prev_s_w, prev_s_y + prev_s_h);
                sprite_draw(s, x, y);
                update_display = 1;
                idle_cnt = 0;
            }

            if(update_display) {
                gfx_update_display();
                update_display = 0;
            }
        }
        idle_cnt = (idle_cnt + 1) % 10000000;
        if(in_jump) {
            y = y0 - jump_get_inst_pos(jump_pos);
            jump_pos++;
            if(y == y0) {
                in_jump = 0;
            }
            // Clear previous sprite
            gfx_clear_rect(prev_s_x, prev_s_y, prev_s_x + prev_s_w, prev_s_y + prev_s_h);
            sprite_draw(s, x, y);
            gfx_update_display();
            prev_s_y = y;
        } else {
            if(idle_cnt >= 9000) {
                s = sprite_get(V_SIDE);
                // Clear previous sprite
                gfx_clear_rect(prev_s_x, prev_s_y, prev_s_x + prev_s_w, prev_s_y + prev_s_h);
                sprite_draw(s, x, y);
                gfx_update_display();
            }
        }
        wait_msec(16);
    }
}