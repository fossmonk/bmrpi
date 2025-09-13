#include "hw.h"
#include "term.h"

#ifndef _GFX_H
#define _GFX_H

#define PD_WIDTH  (1920)
#define PD_HEIGHT (1080)
#define VD_WIDTH  (PD_WIDTH)
#define VD_HEIGHT (PD_HEIGHT)

void gfx_init(void);
void gfx_clearscreen(void);
#ifdef DOUBLE_BUFFER
void gfx_update_display(void);
#endif
void gfx_push_to_screen(void);
void gfx_clear_rect(int x0, int y0, int x1, int y1);
uint32_t gfx_print_color_list();
uint32_t gfx_get_color_from_str(char *cname);
uint32_t gfx_get_color_by_idx(int i);
void gfx_draw_pixel(int32_t x, int32_t y, uint32_t color);
void gfx_draw_pixel_direct(int32_t x, int32_t y, uint32_t color);
void gfx_draw_circle(int x0, int y0, int radius, uint32_t color, int fill);
void gfx_draw_circle_imm(int x0, int y0, int radius, uint32_t color, int fill);
void gfx_draw_rect(int x1, int y1, int x2, int y2, uint32_t color, int fill);
void gfx_draw_rect_imm(int x1, int y1, int x2, int y2, uint32_t color, int fill);
void gfx_draw_square(int x, int y, int a, uint32_t color, int fill);
void gfx_draw_square_imm(int x, int y, int a, uint32_t color, int fill);
void gfx_draw_line(int x1, int y1, int x2, int y2, uint32_t color);
void gfx_draw_line_imm(int x1, int y1, int x2, int y2, uint32_t color);

#endif /* _GFX_H_ */