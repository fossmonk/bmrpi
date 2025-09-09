#include "src/hw.h"
#include "src/mmio.h"
#include "src/gfx.h"
#include "src/led.h"

void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3) {
    led_init();
    led_blink_test(5);
    gfx_init();
    while(1) gfx_draw_circle(-1, -1, 200);
}