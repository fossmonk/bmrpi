#include "hw.h"
#include "term.h"
#include "dma.h"

#ifndef _GFX_H
#define _GFX_H

#undef DOUBLE_BUFFER

#define PD_WIDTH  (640)
#define PD_HEIGHT (480)
#define VD_WIDTH  (PD_WIDTH)
#ifndef DOUBLE_BUFFER
#define VD_HEIGHT (PD_HEIGHT)
#else
#define VD_HEIGHT (PD_HEIGHT * 2)
#endif

void gfx_init(void);
uint32_t gfx_get_pitch(void);
uint32_t gfx_get_buffer(void);
uint32_t gfx_get_fb_bus_addr(void);
dma_channel* gfx_get_dma_ch(void);
void gfx_clearscreen(void);
void gfx_wait_for_vsync(void);
void gfx_set_virtual_offset(uint32_t offset);
void gfx_update_display(void);
void gfx_push_to_screen(void);
void gfx_clear_rect(int x0, int y0, int x1, int y1);
void gfx_get_dimensions(int *w, int *h);
void gfx_print_color_list(void);
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