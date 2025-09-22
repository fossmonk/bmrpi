#include <stdint.h>

#ifndef _TEXT_H_
#define _TEXT_H_

typedef enum {
    ROBOTO_MONO,
    SHREE,
    FONT_COUNT
} font_e;

void text_print_fontlist(void);
void text_get_dimensions(const char *str, font_e f, int* w, int* h);
void text_get_font_dimensions(font_e f, int* glyph_w, int* glyph_h);
void text_draw_char(uint8_t ch, int x, int y, font_e f, uint32_t color);
void text_draw_str(int x, int y, char *s, font_e f, int dir, uint32_t color);
void text_draw_str_imm(int x, int y, char *s, font_e f, int dir, uint32_t color);

#endif