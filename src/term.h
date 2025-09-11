#include <stdint.h>

#ifndef _TERM_H_
#define _TERM_H_

void term_draw_char(uint8_t ch, int x, int y, uint32_t color);
void term_draw_str(int x, int y, char *s, uint32_t color);

#endif