#include "hw.h"
#include "term.h"

#ifndef _GFX_H
#define _GFX_H

void gfx_init(void);

void gfx_draw_pixel(int32_t x, int32_t y, uint32_t color);

void gfx_draw_line(int x1, int y1, int x2, int y2, uint32_t color);

#endif /* _GFX_H_ */