#include "src/hw.h"
#include "src/io.h"
#include "src/gfx.h"

void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3) {
    uart_init();
    int prints = 100;
    while(prints--) uart_print("HELLO RASPI4\r\n");
    gfx_init();
    while(1) gfx_draw_circle(-1, -1, 200);
}