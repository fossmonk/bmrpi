#include "src/hw.h"
#include "src/io.h"
#include "src/gfx.h"

void kernel_main() {
    uart_init();
    uart_print("HELLO RASPI4\r\n");
    gfx_init();
    gfx_draw_line(100,500,350,700,0xFF00FFFF);
    term_draw_str(20, 20, "HELLO RASPBERRY PI 4B", 0xFF00FFFF);
}