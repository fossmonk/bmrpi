#include <stdint.h>
#include "maths.h"
#include "io.h"
#include "gfx.h"
#include "led.h"

#define DISPLAY_WIDTH  (1920)
#define DISPLAY_HEIGHT (1080)

#define MBOX_VALID_RESP (0x80000000)

volatile uint32_t mbox[36] __attribute__((aligned(16)));

uint32_t width, height, pitch;
uint8_t *FB;

int32_t mbox_call(uint8_t ch) {
    // Get the physical address of the mailbox buffer
    uint32_t mbox_phys = (uint32_t)(uintptr_t)mbox;

    // message address + channel number
    uint32_t r = ((mbox_phys & 0x3FFFFFFF) | (ch & 0xF));

    // wait until mailbox is not full
    while (mmio_read(MBOX_STATUS) & MBOX_FULL);

    // Write message address to mailbox
    mmio_write(MBOX_WRITE, r);

    while(1) {
        // wait until mailbox is not empty
        while (mmio_read(MBOX_STATUS) & MBOX_EMPTY);

        // check if response is for our message
        if (r == mmio_read(MBOX_READ)) {
            return mbox[1] == MBOX_VALID_RESP;
        }
    }
    return 0;
}

void gfx_init() {
    mbox[0] = 35 * 4; // message size in bytes
    mbox[1] = 0;      // This is a request message

    // Tag 1: Set physical (display) width/height
    mbox[2] = 0x48003; 
    mbox[3] = 8;              // Value buffer size
    mbox[4] = 0;              // Request/response indicator
    mbox[5] = DISPLAY_WIDTH;  // Width
    mbox[6] = DISPLAY_HEIGHT; // Height

    // Tag 2: Set virtual (buffer) width/height
    mbox[7] = 0x48004;
    mbox[8] = 8;
    mbox[9] = 0;
    mbox[10] = DISPLAY_WIDTH;  // Width
    mbox[11] = DISPLAY_HEIGHT; // Height

    // Tag 3: Set color depth
    mbox[12] = 0x48005;
    mbox[13] = 4;
    mbox[14] = 0;
    mbox[15] = 32; // 32 bits per pixel (AARRGGBB)

    // Tag 4: Allocate framebuffer
    mbox[16] = 0x40001;
    mbox[17] = 8;
    mbox[18] = 0;
    mbox[19] = 16; // Alignment (will be returned as framebuffer address)
    mbox[20] = 0;

    // Tag 5: Get pitch (bytes per line)
    mbox[21] = 0x40008;
    mbox[22] = 4;
    mbox[23] = 0;
    mbox[24] = 0; // Pitch will be returned here

    mbox[25] = 0; // End of tags marker

    // Send the message and check for success
    if (mbox_call(MBOX_CH_PROP)) {
        pitch = mbox[24];
        width = mbox[5];
        height = mbox[6];
        uint32_t fb_bus_addr = mbox[19];
        // The framebuffer address is returned in the 'allocate buffer' tag response
        FB = (uint8_t *)(uintptr_t)(fb_bus_addr + 0x40000000);

        // for (int y = 0; y < height; y++) {
        //     for (int x = 0; x < width; x++) {
        //         ((uint32_t*)FB)[y * (pitch / 4) + x] = 0xFF00FFFF;  // CYAN, ARGB
        //     }
        // }
    } else {
        FB = NULL;
        while(1);
    }
}

void gfx_draw_pixel(int32_t x, int32_t y, uint32_t color) {
    uart_print("DRAWING PIXEL\r\n");
    int32_t offset = (y * pitch) + (x * 4);
    *((uint32_t *)(FB + offset)) = color;
}

void gfx_draw_line(int x1, int y1, int x2, int y2, uint32_t color)  
{  
    uart_print("Trying to draw line\r\n");
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
