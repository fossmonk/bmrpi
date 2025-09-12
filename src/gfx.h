#include "hw.h"
#include "term.h"

#ifndef _GFX_H
#define _GFX_H

#define DISPLAY_WIDTH  (1920)
#define DISPLAY_HEIGHT (1080)

void gfx_init(void);
void gfx_clearscreen(void);
uint32_t gfx_print_color_list();
uint32_t gfx_get_color_from_str(char *cname);
uint32_t gfx_get_color_by_idx(int i);
void gfx_draw_circle(int x0, int y0, int radius, uint32_t color, int fill);
void gfx_draw_rect(int x1, int y1, int x2, int y2, uint32_t color, int fill);
void gfx_draw_square(int x, int y, int a, uint32_t color, int fill);
void gfx_draw_pixel(int32_t x, int32_t y, uint32_t color);
void gfx_draw_line(int x1, int y1, int x2, int y2, uint32_t color);

#endif /* _GFX_H_ */