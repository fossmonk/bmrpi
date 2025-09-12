#include "src/hw.h"
#include "src/io.h"
#include "src/gfx.h"
#include "src/colors.h"

uint32_t colors[] = {
    CSS_ORANGE,
    CSS_GOLD,
    CSS_INDIGO,
    CSS_VIOLET,
    CSS_TURQUOISE,
    CSS_CYAN,
    CSS_SALMON,
    CSS_CORAL,
    CSS_TOMATO,
    CSS_ORCHID,
    CSS_HOTPINK,
    CSS_CRIMSON,
    CSS_CHOCOLATE,
    CSS_SIENNA,
    CSS_PEACHPUFF,
    CSS_LAVENDER,
    CSS_BLACK,
    CSS_SILVER,
    CSS_GRAY,
    CSS_WHITE,
    CSS_MAROON,
    CSS_RED,
    CSS_PURPLE,
    CSS_FUCHSIA,
    CSS_GREEN,
    CSS_LIME,
    CSS_OLIVE,
    CSS_YELLOW,
    CSS_NAVY,
    CSS_BLUE,
    CSS_TEAL,
    CSS_AQUA,
};

void color_text_test() {
    for(int i = 0; i < 32; i++) {
        term_draw_str(20, 20*(i + 1), "HELLO RASPI 4B", colors[i]);
    }
}

void kernel_main() {
    uart_init();
    uart_print("HELLO FROM RASPI 4B\r\n");
    gfx_init();
    color_text_test();
    while(1) {
    }
}