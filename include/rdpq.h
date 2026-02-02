/* rdpq.h -- rdp helpers header
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef __RDPQ_H__
#define __RDPQ_H__

#include <libdragon.h>

// rdpq_draw_faded_sprite: Draw sprite with a fade alpha.
void rdpq_draw_faded_sprite(sprite_t *sp, int x, int y, uint8_t alpha);
// rdpq_draw_filled_fullscreen: Fill entire screen with color.
void rdpq_draw_filled_fullscreen(color_t color);
// rdpq_draw_filled_rectangle_size: Fill rectangle with color.
void rdpq_draw_filled_rectangle_size(int x, int y, int width, int height, color_t color);
// rdpq_draw_filled_rectangle_with_border_size: Fill rectangle with border.
void rdpq_draw_filled_rectangle_with_border_size(int x, int y, int width, int height, color_t color, color_t border_color);
// rdpq_draw_sprite: Draw sprite with optional mirroring.
void rdpq_draw_sprite(sprite_t *sp, int x, int y, mirror_t mirror);

#endif //__RDPQ_H__
