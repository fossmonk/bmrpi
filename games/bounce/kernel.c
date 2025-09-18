#include "../../src/hw.h"
#include "../../src/io.h"
#include "../../src/gfx.h"
#include "../../src/rand.h"
#include "../../src/dma.h"
#include "../../src/printf.h"
#include "../../src/sprite.h"
#include "../../src/maths.h"

extern uint32_t rcube[];
sprite_t *s_table;

int ground_y = 0;
int anim_frame_count = 0;

void draw_loop(void);
int precompute_trajectory(double e, double g, double h);

void kernel_main() {
    uart_init();
    gfx_init();
    // register first ball
    sprite_register(0, 0, 30, 30, rcube);
    s_table = sprite_get_table();
    ground_y = (PD_HEIGHT - 30);
    anim_frame_count = precompute_trajectory(0.8, 
                                            (double)7, 
                                            (double)ground_y);
    draw_loop();
}

#define ANIM_NUM_FRAMES_MAX (200)

int bounce_trajectory[ANIM_NUM_FRAMES_MAX] = { 0 };

int precompute_trajectory(double e, double g, double h) {
    int frame_count = 0;
    double u0 = sqrt(2.0*g*h);
    double gby2 = g/2.0;
    int bounce_count = floor((1.0 - logn(u0))/logn(e));
    int ff_frames = floor(sqrt(2.0*h/g));
    
    for(int i = 0; i < ff_frames; i++) {
        bounce_trajectory[frame_count++] = floor(h - gby2*i*i);
    }

    double u = u0;

    for(int i = 0; i < bounce_count; i++) {
        u = u*e;
        int t = floor(2.0*u/g);
        for(int j = 0; j < (t+1); j++) {
            double k = (double)j;
            bounce_trajectory[frame_count++] = floor(u*k - gby2*k*k);
        }
    }
    return frame_count;
}

void draw_loop(void) {
    while(1) {
        int input = uart_getc_nb();
        if(input == 'a') {
            // register one more sprite
            int randx = rand() % (PD_WIDTH - 30);
            sprite_register(randx, 0, 30, 30, rcube);
        }
        int s_count = sprite_get_count();
        for(int i = 0; i < s_count; i++) {
            sprite_t *s = &s_table[i];
            s->ypos = ground_y - bounce_trajectory[s->age];
            gfx_clear_rect(s->prev_xpos, s->prev_ypos, s->prev_xpos + s->width, s->prev_ypos + s->height);
            sprite_put(s);
            s->prev_xpos = s->xpos;
            s->prev_ypos = s->ypos;
            s->age = (s->age + 1) % anim_frame_count;
        }
        gfx_update_display();
        wait_msec(16);
    }
}