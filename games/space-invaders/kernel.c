#include "../../src/hw.h"
#include "../../src/io.h"
#include "../../src/gfx.h"
#include "../../src/colors.h"
#include "../../src/strops.h"
#include "../../src/rand.h"
#include "../../src/dma.h"
#include "../../src/printf.h"
#include "../../src/text.h"
#include "../../src/sprite.h"

#include "sprite/sprite-data.h"

#define ALIGN32 __attribute__((aligned(32)))

#define ALIENS_ROWS (5)
#define ALIENS_COLS (6)
#define ALIENS_GAP_W (10) // 10 pixels gap between aliens
#define RBULLET_MAX_COUNT (3)
#define ABULLET_MAX_COUNT (5)

#define SCORE_TEXT "SCORE: "

typedef struct {
    sprite_t s;
    int is_active;
} bullet_t;

/* Sprite data and structs */
uint32_t rocket_data[] = ROCKET_DATA;
uint32_t alien_data[] = ALIEN_DATA;
uint32_t bullet_data[] = BULLET_DATA;

/* GLOBALS */
/* We have one rocket, 6 x 5 = 30 aliens, and keep a max of 32 bullets at a time */
sprite_t g_rocket ALIGN32;
sprite_t g_aliens[ALIENS_ROWS*ALIENS_COLS] ALIGN32;

bullet_t g_rbs[RBULLET_MAX_COUNT] ALIGN32;
bullet_t g_abs[ABULLET_MAX_COUNT] ALIGN32;

int disp_w, disp_h;
int score_x, score_y, score_w, score_h;
int alien_fleet_x, alien_fleet_y, alien_fleet_max_x, fleet_lx, fleet_rx, fleet_dir, fleet_y_inc;
const int alien_fleet_w = ALIENS_COLS*ALIEN_W + (ALIENS_COLS-1)*(ALIENS_GAP_W);
const int alien_fleet_h = ALIENS_ROWS*ALIEN_H;
const int alien_count = ALIENS_ROWS*ALIENS_COLS;
const int rocket_velocity = 4;
int g_score = 0;
int g_rb_count = 0;

/* Function declarations */
void init_sprites(void);
void init_score(void);
void init_lives(void);
void draw_loop(void);

void kernel_main() {
    uart_init();
    gfx_init();

    gfx_get_dimensions(&disp_w, &disp_h);
    alien_fleet_x = disp_w/2 - (2*ALIENS_GAP_W + (ALIENS_GAP_W/2) + 3*ALIEN_W);
    alien_fleet_y = disp_h/3 - (2*ALIEN_H + (ALIEN_H/2));
    alien_fleet_max_x = alien_fleet_x + alien_fleet_w;

    init_sprites();
    init_score();
    init_lives();

    draw_loop();

    // spin
    while(1);
}

void init_sprites(void) {
    /* Init rocket */
    g_rocket.buffer = rocket_data;
    g_rocket.width = ROCKET_W;
    g_rocket.height = ROCKET_H;
    g_rocket.xpos = disp_w/2 - ROCKET_W;
    g_rocket.ypos = disp_h - ROCKET_H;
    g_rocket.age = 0;
    g_rocket.prev_xpos = g_rocket.xpos;
    g_rocket.prev_ypos = g_rocket.ypos;

    // draw rocket
    sprite_put(&g_rocket);

    /* Init bullets with buffer, but don't set as active or other params */
    for(int i = 0; i < RBULLET_MAX_COUNT; i++) {
        g_rbs[i].s.buffer = bullet_data;
        g_rbs[i].s.height = BULLET_H;
        g_rbs[i].s.width = BULLET_W;
        g_rbs[i].is_active = 0;
    }
    for(int i = 0; i < ABULLET_MAX_COUNT; i++) {
        g_abs[i].s.buffer = bullet_data;
        g_abs[i].s.height = BULLET_H;
        g_abs[i].s.width = BULLET_W;
        g_abs[i].is_active = 0;
    }

    /* Init aliens */
    for(int i = 0; i < alien_count; i++) {
        int k = (i % ALIENS_COLS);
        g_aliens[i].buffer = alien_data;
        g_aliens[i].width = ALIEN_W;
        g_aliens[i].height = ALIEN_H;
        g_aliens[i].xpos = ((alien_fleet_x + k*ALIEN_W) % alien_fleet_max_x) + k*ALIENS_GAP_W;
        g_aliens[i].ypos = (alien_fleet_y + (i / ALIENS_COLS)*ALIEN_H);
        g_aliens[i].age = 0;
        g_aliens[i].prev_xpos = g_aliens[i].xpos;
        g_aliens[i].prev_ypos = g_aliens[i].ypos;

        sprite_put(&g_aliens[i]);
    }
    fleet_lx = g_aliens[0].xpos;
    fleet_rx = g_aliens[alien_count-1].xpos + ALIEN_W + ALIENS_GAP_W;
    fleet_dir = 1;
    fleet_y_inc = 0;
}

void init_score(void) {
    // score on top left
    text_get_dimensions(SCORE_TEXT, ROBOTO_MONO, &score_w, &score_h);
    score_x = score_w;
    score_y = 0;
    text_draw_str(0, 0, SCORE_TEXT, ROBOTO_MONO, 0, CSS_CYAN);
    text_draw_str(score_x, score_y, "0", ROBOTO_MONO, 0, CSS_CYAN);
}

void init_lives(void) {

}

void print_game_over(int won) {
    int t_w, t_h;
    char* won_str = "WON  !!";
    char* lost_str = "LOST !!";
    text_get_dimensions("GAME OVER !! YOU XXXX !!", ROBOTO_MONO, &t_w, &t_h);
    char s[30];
    strops_copy(s, "GAME OVER !! YOU ");
    if(won) {
        strops_copy(s+17, won_str);
    } else {
        strops_copy(s+17, lost_str);
    }
    int x = (disp_w - t_w)/2;
    int y = (disp_h - t_h)/2;
    text_draw_str(x, y, s, ROBOTO_MONO, 0, CSS_CYAN);
    gfx_update_display();
}

void update_score(int score) {
    char temp[10];
    g_score += score;
    text_draw_str(score_x, score_y, "      ", ROBOTO_MONO, 0, CSS_CYAN);
    strops_num2str(g_score, temp);
    text_draw_str(score_x, score_y, temp, ROBOTO_MONO, 0, CSS_CYAN);
}

void draw_loop(void) {
    while(1) {
        // Check for input
        int input = uart_getc_nb();
        if(input != -1) {
            // Update rocket
            if(input == 'a') {
                // move rocket left
                g_rocket.xpos -= rocket_velocity;
                if(g_rocket.xpos <= 5) { 
                    g_rocket.xpos = 5;
                } else {
                    gfx_clear_rect(g_rocket.prev_xpos,
                                   g_rocket.prev_ypos, 
                                   g_rocket.prev_xpos + g_rocket.width,
                                   g_rocket.prev_ypos + g_rocket.height);
                }
                g_rocket.prev_xpos = g_rocket.xpos;
                sprite_put(&g_rocket);
            }
            if(input == 'd') {
                // move rocket right
                g_rocket.xpos += rocket_velocity;
                if(g_rocket.xpos >= (disp_w - g_rocket.width)) { 
                    g_rocket.xpos = (disp_w - g_rocket.width);
                } else {
                    gfx_clear_rect(g_rocket.prev_xpos,
                                   g_rocket.prev_ypos, 
                                   g_rocket.prev_xpos + g_rocket.width,
                                   g_rocket.prev_ypos + g_rocket.height);
                }
                g_rocket.prev_xpos = g_rocket.xpos;
                sprite_put(&g_rocket);
            }
            if(input == 'l') {
                // Check if there is a space for new rbullet
                int idx = -1;
                for(int i = 0; i < RBULLET_MAX_COUNT; i++) {
                    if(g_rbs[i].is_active == 0) {
                        idx = i;
                        break;
                    }
                }
                if(idx >= 0) {
                    g_rbs[idx].s.xpos = g_rocket.xpos + g_rocket.width/2;
                    g_rbs[idx].s.ypos = g_rocket.ypos - g_rbs[idx].s.height;
                    g_rbs[idx].s.prev_xpos = g_rbs[idx].s.xpos;
                    g_rbs[idx].s.prev_ypos = g_rbs[idx].s.ypos;
                    g_rbs[idx].is_active = 1;
                }
            }
        }
        // Move all the active bullets up or down
        for(int i = 0; i < RBULLET_MAX_COUNT; i++) {
            if(g_rbs[i].is_active == 1) {
                gfx_clear_rect(g_rbs[i].s.xpos, g_rbs[i].s.ypos, 
                               g_rbs[i].s.xpos + g_rbs[i].s.width, g_rbs[i].s.ypos + g_rbs[i].s.height);
                g_rbs[i].s.ypos -= 8;
                if(g_rbs[i].s.ypos != 0) {
                    sprite_put(&g_rbs[i].s);
                } else {
                    // bullet reached end of screen
                    g_rbs[i].is_active = 0;
                }
            }
        }
        // Move the alien fleet, checking if they've hit boundaries
        // If they have, come 5 pixels down, also move in other direction
        // If bullet has hit an alien, remove alien from fleet, set bullet to inactive
        if(fleet_rx >= disp_w - (ALIEN_W + ALIENS_GAP_W)) {
            fleet_dir = -1;
            fleet_y_inc = 1;
        } else if(fleet_lx <= 0) {
            fleet_dir = 1;
            fleet_y_inc = 1;
        }
        int active_alien_count = 0;
        int max_active_alien_y = 0;
        for(int i = 0; i < alien_count; i++) {
            if(g_aliens[i].buffer != NULL) {
                active_alien_count++;
                g_aliens[i].xpos += (fleet_dir * 4);
                if(fleet_y_inc) {
                    g_aliens[i].ypos += 5;
                }
                if(g_aliens[i].ypos > max_active_alien_y) {
                    max_active_alien_y = g_aliens[i].ypos;
                }
                gfx_clear_rect(g_aliens[i].prev_xpos, g_aliens[i].prev_ypos, 
                            g_aliens[i].prev_xpos + g_aliens[i].width,
                            g_aliens[i].prev_ypos + g_aliens[i].height);
                g_aliens[i].prev_xpos = g_aliens[i].xpos;
                g_aliens[i].prev_ypos = g_aliens[i].ypos;
                
                // check for bullet hit
                int hitzone_lx = g_aliens[i].xpos;
                int hitzone_rx = g_aliens[i].xpos + g_aliens[i].width;
                int hitzone_y = g_aliens[i].ypos + g_aliens[i].height;
                for(int b = 0; b < RBULLET_MAX_COUNT; b++) {
                    if((g_rbs[b].is_active == 1) &&
                       (g_rbs[b].s.xpos >= hitzone_lx) && 
                       (g_rbs[b].s.xpos <= hitzone_rx) && 
                       (g_rbs[b].s.ypos <= hitzone_y)) {
                        // hit, remove alien and bullet and update score
                        g_aliens[i].buffer = NULL;
                        g_rbs[b].is_active = 0;
                        gfx_clear_rect(g_rbs[b].s.xpos, g_rbs[b].s.ypos, 
                                    g_rbs[b].s.xpos + g_rbs[b].s.width,
                                    g_rbs[b].s.ypos + g_rbs[b].s.height);
                        update_score(10);
                    }
                }
                sprite_put(&g_aliens[i]);
            }
        }

        if(max_active_alien_y >= g_rocket.ypos) {
            print_game_over(0);
            break;
        }
        
        if(active_alien_count == 0) {
            print_game_over(1);
            break;
        }
        
        if(fleet_y_inc) fleet_y_inc = 0;

        fleet_lx += (fleet_dir * 4);
        fleet_rx += (fleet_dir * 4);

        // After all the updates, push to screen
        gfx_update_display();
    }
}
