#include "../../src/hw.h"
#include "../../src/io.h"
#include "../../src/gfx.h"
#include "../../src/colors.h"
#include "../../src/rand.h"
#include "../../src/dma.h"
#include "../../src/printf.h"
#include "../../src/text.h"

#define NUM_DIMS  (7)
#define DIM_DELAY (10)
#define RAINSTR_COUNT_MAX (10)

// Assume a maximum of 200 slots
#define SLOTS_COUNT_MAX (200)


typedef struct {
    char data[64];
    uint32_t color;
    int len;
} rainstr_t;

typedef struct {
    rainstr_t *rst;
    int age;
    int x;
    int is_active;
} slot_t;

int disp_h, disp_w, font_h, font_w, rainstr_max_len, slot_count;

rainstr_t rsts[RAINSTR_COUNT_MAX] __attribute__((section(".bss.rainstrs")));
slot_t slots[SLOTS_COUNT_MAX] __attribute__((section(".bss.rslots")));

uint8_t alpha_arr[NUM_DIMS] = {
    255, 219, 183, 147, 111, 75, 39
};

void draw_loop(void);

void init_rainstrs(void) {
    // Set all RSTs
    int min = rainstr_max_len/2;
    for(int i = 0; i < RAINSTR_COUNT_MAX; i++) {
        rsts[i].len = min + (rand() % (rainstr_max_len - min));
        randstr(rsts[i].data, rsts[i].len);
        rsts[i].color = CSS_LIME;
    }
}

int random_binary_decision(void) {
    int x = rand() % 1001;
    if(x % 33 == 0) return 1;
    return 0;
}

void kernel_main() {
    uart_init();
    gfx_init();

    // Find out display parameters to setup the strings
    gfx_get_dimensions(&disp_w, &disp_h);
    // Get text dimensions
    text_get_font_dimensions(SHREE, &font_w, &font_h);
    
    // Initialize rainstrs
    rainstr_max_len = (disp_h/font_h);
    slot_count = (disp_w/font_w)-1;

    init_rainstrs();

    // Init all the slots, but don't make them active yet
    for(int i = 0; i < slot_count; i++) {
        slots[i].rst = NULL;
        slots[i].age = 0;
        slots[i].x = i * font_w;
        slots[i].is_active = 0;
    }

    // make some slots active
    slots[1].is_active = 1;

    draw_loop();
    // shouldn't reach here
    while(1);
}

void draw_loop(void) {
    while(1) {
        for(int i = 0; i < slot_count; i++) {
            if(slots[i].is_active == 1) {
                if(slots[i].rst == NULL) {
                    int idx = rand() % RAINSTR_COUNT_MAX;
                    slots[i].rst = &rsts[idx];
                    text_draw_char(slots[i].rst->data[0],
                                   slots[i].x, 0,
                                   SHREE, slots[i].rst->color);
                    slots[i].age = 1;
                } else {
                    rainstr_t *r = slots[i].rst;
                    if (slots[i].age < r->len) {
                        int y = 0;
                        for(int j = slots[i].age; j >= 0; j--) {
                            text_draw_char(slots[i].rst->data[j],
                                           slots[i].x, y, SHREE,
                                           slots[i].rst->color);
                            y += font_h;
                        }
                        slots[i].age++;
                    } else if (slots[i].age >= r->len) {
                        int x0 = slots[i].x;
                        int y0 = 0;
                        int x1 = slots[i].x + font_w;
                        int y1 = PD_HEIGHT;
                        if (slots[i].age >= (r->len + NUM_DIMS)) {
                            // end of life, clear the slot
                            slots[i].age = 0;
                            slots[i].rst = NULL;
                            slots[i].is_active = 0;
                            gfx_clear_rect(x0, y0, x1, y1);
                        } else {
                            int dim_idx = slots[i].age - r->len;
                            gfx_dim_rect(x0, y0, x1, y1, alpha_arr[dim_idx]);
                            slots[i].age++;
                        }
                    }
                }
            }
        }
        // randomly set some slots to be active
        for(int i = 0; i < slot_count; i++) {
            if((slots[i].is_active == 0) && random_binary_decision()) {
                slots[i].is_active = 1;
            }
        }
        gfx_update_display();
    }
}