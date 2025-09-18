#include <stdint.h>
#include "maths.h"
#include "io.h"
#include "gfx.h"
#include "led.h"
#include "strops.h"
#include "colors.h"
#include "dma.h"
#include "printf.h"
#include "mbox.h"

#define BUS_ADDR(addr) (((uint64_t)addr | 0x40000000) & ~0xC0000000)

uint8_t gfx_buffer[PD_HEIGHT*PD_WIDTH*4] __attribute__((section(".bss.gfx_buffer"))) __attribute__((aligned(32)));

uint32_t width, height, pitch, pitch_by_4;
uint8_t *FB0;
#ifdef DOUBLE_BUFFER
uint8_t *FB1;
uint8_t *frontbuffer;
#endif
uint8_t *backbuffer;
dma_channel *gfx_dma_ch;

void gfx_init() {
    volatile uint32_t *mbox = mbox_get_buffer();
    mbox[0] = 31 * 4; // message size in bytes
    mbox[1] = 0;      // This is a request message

    // Tag 1: Set physical (display) width/height
    mbox[2] = 0x48003; 
    mbox[3] = 8;              // Value buffer size
    mbox[4] = 0;              // Request/response indicator
    mbox[5] = PD_WIDTH;  // Width
    mbox[6] = PD_HEIGHT; // Height

    // Tag 2: Set virtual (buffer) width/height
    mbox[7] = 0x48004;
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = VD_WIDTH;  // Width
    mbox[11] = VD_HEIGHT; // Height

     // Tag 2: Set virtual (buffer) offset
    mbox[12] = 0x48009;
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0;  // x offset
    mbox[16] = 0; // y offset

    // Tag 3: Set color depth
    mbox[17] = 0x48005;
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32; // 32 bits per pixel (AARRGGBB)

    // Tag 4: Allocate framebuffer
    mbox[21] = 0x40001;
    mbox[22] = 8;
    mbox[23] = 8;
    mbox[24] = 4096; // Alignment (will be returned as framebuffer address)
    mbox[25] = 0;

    // Tag 5: Get pitch (bytes per line)
    mbox[26] = 0x40008;
    mbox[27] = 4;
    mbox[28] = 4;
    mbox[29] = 0; // Pitch will be returned here

    mbox[30] = 0; // End of tags marker

    // Send the message and check for success
    if (mbox_call(MBOX_CH_PROP)) {
        pitch = mbox[29];
        pitch_by_4 = pitch/4;
        width = mbox[5];
        height = mbox[6];
        uint32_t fb_bus_addr = mbox[24];
        // The framebuffer address is returned in the 'allocate buffer' tag response
        FB0 = (uint8_t *)(uintptr_t)(fb_bus_addr + 0x40000000);
        #ifdef DOUBLE_BUFFER
        FB1 = FB0 + (PD_HEIGHT * PD_WIDTH * 4);
        backbuffer = FB1;
        frontbuffer = FB0;
        #else
        backbuffer = FB0;
        #endif
        // Initialize DMA for buffer copy
        gfx_dma_ch = dma_open_channel(CT_NORMAL);
    } else {
        while(1);
    }
}

uint32_t gfx_get_pitch() {
    return pitch;
}

dma_channel* gfx_get_dma_ch() {
    return gfx_dma_ch;
}

uint32_t gfx_get_buffer(void) {
    return (uint32_t)(uintptr_t)(gfx_buffer);
}

uint32_t gfx_get_fb_bus_addr(void) {
    return (uint32_t)(uintptr_t)(backbuffer);
}

void gfx_set_virtual_offset(uint32_t offset) {
    volatile uint32_t *mbox = mbox_get_buffer();
    mbox[0] = 8 * 4; // buffer size in bytes
    mbox[1] = 0;     // request/response

    // Set virtual offset
    mbox[2] = 0x00048009; // Set virtual offset
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 0; // X offset
    mbox[6] = offset; // Y offset

    mbox[7] = 0; // End tag
    mbox_call(MBOX_CH_PROP);
}

void gfx_wait_for_vsync() {
    volatile uint32_t *mbox = mbox_get_buffer();
    mbox[0] = 7 * 4; // buffer size in bytes
    mbox[1] = 0;     // request/response

    mbox[2] = 0x0004800E; // VSYNC
    mbox[3] = 4;
    mbox[4] = 0;
    mbox[5] = 0;
    mbox[6] = 0; // End tag
    mbox_call(MBOX_CH_PROP);
}

void gfx_update_display() {
#ifdef DOUBLE_BUFFER
    // Wait for VSYNC
    gfx_wait_for_vsync();
#endif
    // Push internal buffer to backbuffer
    gfx_push_to_screen();
    
#ifdef DOUBLE_BUFFER
    static int swap = 1;
    // Swap the back and frontbuffer
    if(swap) {
        gfx_set_virtual_offset(PD_HEIGHT);
        backbuffer = FB0;
        frontbuffer = FB1;
        swap = 0;
    } else {
        gfx_set_virtual_offset(0);
        backbuffer = FB1;
        frontbuffer = FB0;
        swap = 1;
    }
#endif
}

/* Actual Graphics Functions */
/* Color Name Table */
char *colornames[] = {
    "black", "silver", "gray", "white", "maroon", "red", "purple", "fuchsia",
    "green", "lime", "olive", "yellow", "navy", "blue", "teal", "aqua", 
    "orange", "gold", "indigo", "violet", "turquoise", "cyan", "salmon", "coral",
    "tomato", "orchid", "hotpink", "crimson", "chocolate", "sienna", "peachpuff","lavender",
};

uint32_t colorvalues[] = {
    CSS_BLACK, CSS_SILVER, CSS_GRAY, CSS_WHITE, CSS_MAROON, CSS_RED, CSS_PURPLE, CSS_FUCHSIA,
    CSS_GREEN, CSS_LIME, CSS_OLIVE, CSS_YELLOW, CSS_NAVY, CSS_BLUE, CSS_TEAL, CSS_AQUA,
    CSS_ORANGE, CSS_GOLD, CSS_INDIGO, CSS_VIOLET, CSS_TURQUOISE, CSS_CYAN, CSS_SALMON, CSS_CORAL,
    CSS_TOMATO, CSS_ORCHID, CSS_HOTPINK, CSS_CRIMSON, CSS_CHOCOLATE, CSS_SIENNA, CSS_PEACHPUFF, CSS_LAVENDER,
};

void gfx_get_dimensions(int *w, int *h) {
    *w = width;
    *h = height;
}

void gfx_print_color_list() {
    for(int i = 0; i < NUM_COLORS; i++) {
        printf("%s ", colornames[i]);
        if(i && i % 8 == 0) printf("\n");
    }
    printf("\n");
}

uint32_t gfx_get_color_by_idx(int i) {
    return colorvalues[i];
}

uint32_t gfx_get_color_from_str(char *cname) {
    uint32_t color_idx = NUM_COLORS;
    for(int i = 0; i < NUM_COLORS; i++) {
        if(strops_cmp(colornames[i], cname) == 0) {
            color_idx = i;
            break;
        }
    }
    
    if(color_idx < 32) {
        return colorvalues[color_idx];
    }

    // return CYAN by default
    return CSS_CYAN;
}

void gfx_push_to_screen() {
    dma_setup_2dmem_copy(gfx_dma_ch, backbuffer, gfx_buffer, PD_WIDTH*4, PD_HEIGHT, PD_WIDTH*4, 12);
    dma_start(gfx_dma_ch);
    dma_wait(gfx_dma_ch);
}

static uint32_t blackarr = CSS_WHITE;
void gfx_clearscreen() {
    dma_setup_mem_copy(gfx_dma_ch, gfx_buffer, &blackarr, PD_HEIGHT*PD_WIDTH*4, 1);
    gfx_dma_ch->block->transfer_info = ((1 << TI_BURST_LENGTH_SHIFT) | (TI_DEST_INC));
    dma_start(gfx_dma_ch);
    dma_wait(gfx_dma_ch);
}

void gfx_clear_rect(int x0, int y0, int x1, int y1) {
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            ((uint32_t*)gfx_buffer)[y * (pitch_by_4) + x] = 0xFF000000;  // BLACK, ARGB
        }
    }
}

void gfx_push_rectblock(int x0, int y0, int x1, int y1) {
    /* Assumption is that something is drawn on draw buffer. x0, y0, x1, y1
       define the bounding box rectangle for the figure drawn
    */
    int offset0 = (y0 * pitch) + (x0 * 4);
    dma_setup_2dmem_copy(gfx_dma_ch, 
                        (void *)((uintptr_t)(backbuffer) + offset0),
                        (void *)((uintptr_t)gfx_buffer + offset0),
                        (x1-x0)*4, (y1-y0), PD_WIDTH*4, 2);
    dma_start(gfx_dma_ch);
    dma_wait(gfx_dma_ch);
}

void gfx_clearscreen_direct() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            ((uint32_t*)backbuffer)[y * (pitch_by_4) + x] = 0xFF000000;  // BLACK, ARGB
            #ifdef DOUBLE_BUFFER
            ((uint32_t*)frontbuffer)[y * (pitch_by_4) + x] = 0xFF000000;  // BLACK, ARGB
            #endif
        }
    }
}

void gfx_draw_pixel(int32_t x, int32_t y, uint32_t color) {
    int32_t offset = (y * pitch) + (x * 4);
    *((uint32_t *)(gfx_buffer + offset)) = color;
}

void gfx_draw_pixel_direct(int32_t x, int32_t y, uint32_t color) {
    int32_t offset = (y * pitch) + (x * 4);
    *((uint32_t *)(backbuffer + offset)) = color;
}

void gfx_draw_rect(int x1, int y1, int x2, int y2, uint32_t color, int fill) {
    int y = y1;

    while (y <= y2) {
        int x = x1;
        while (x <= x2) {
            if ((x == x1 || x == x2) || (y == y1 || y == y2)) gfx_draw_pixel(x, y, color);
            else if (fill) gfx_draw_pixel(x, y, color);
            x++;
        }
        y++;
    }
}

void gfx_draw_rect_imm(int x1, int y1, int x2, int y2, uint32_t color, int fill) {
    gfx_draw_rect(x1, y1, x2, y2, color, fill);
    gfx_push_rectblock(x1, y1, x2, y2);
}


void gfx_draw_square(int x, int y, int a, uint32_t color, int fill) {
    gfx_draw_rect(x - a/2, y - a/2, x + a/2, y + a/2, color, fill);
}

void gfx_draw_square_imm(int x, int y, int a, uint32_t color, int fill) {
    gfx_draw_rect_imm(x-a/2, y-a/2, x+a/2, y+a/2, color, fill);
}

void gfx_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {  
    int dx, dy, p, x, y;

    dx = x2-x1;
    dy = y2-y1;
    x = x1;
    y = y1;
    p = 2*dy-dx;

    while (x < x2) {
       if (p >= 0) {
          gfx_draw_pixel(x, y, color);
          y++;
          p = p+2*dy-2*dx;
       } else {
          gfx_draw_pixel(x, y, color);
          p = p + 2*dy;
       }
       x++;
    }
}

void gfx_draw_line_imm(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx, dy, p, x, y;

    dx = x2-x1;
    dy = y2-y1;
    x = x1;
    y = y1;
    p = 2*dy-dx;

    while (x < x2) {
       if (p >= 0) {
          gfx_draw_pixel_direct(x, y, color);
          y++;
          p = p+2*dy-2*dx;
       } else {
          gfx_draw_pixel_direct(x, y, color);
          p = p + 2*dy;
       }
       x++;
    }
}

void gfx_draw_circle(int x0, int y0, int radius, uint32_t color, int fill) {
    int x = radius;
    int y = 0;
    int err = 0;
 
    while (x >= y) {
        if (fill) {
            gfx_draw_line(x0 - y, y0 + x, x0 + y, y0 + x, color);
            gfx_draw_line(x0 - x, y0 + y, x0 + x, y0 + y, color);
            gfx_draw_line(x0 - x, y0 - y, x0 + x, y0 - y, color);
            gfx_draw_line(x0 - y, y0 - x, x0 + y, y0 - x, color);
        }
        gfx_draw_pixel(x0 - y, y0 + x, color);
        gfx_draw_pixel(x0 + y, y0 + x, color);
        gfx_draw_pixel(x0 - x, y0 + y, color);
        gfx_draw_pixel(x0 + x, y0 + y, color);
        gfx_draw_pixel(x0 - x, y0 - y, color);
        gfx_draw_pixel(x0 + x, y0 - y, color);
        gfx_draw_pixel(x0 - y, y0 - x, color);
        gfx_draw_pixel(x0 + y, y0 - x, color);

        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }
    
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void gfx_draw_circle_imm(int x0, int y0, int radius, uint32_t color, int fill) {
    gfx_draw_circle(x0, y0, radius, color, fill);
    gfx_push_rectblock(x0-radius, y0-radius, x0+radius, y0+radius);
}
