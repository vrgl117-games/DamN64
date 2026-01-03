/* rdpq.c -- rdpq helpers implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "rdpq.h"

void rdpq_draw_faded_sprite(sprite_t *sp, int x, int y, uint8_t alpha)
{
    rdpq_set_mode_standard();
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY_CONST);

    rdpq_set_fog_color(RGBA32(255, 255, 255, alpha));

    rdpq_sprite_blit(sp, x, y, NULL);
}

void rdpq_draw_filled_fullscreen(color_t color)
{
    uint32_t w = display_get_width();
    uint32_t h = display_get_height();
    rdpq_draw_filled_rectangle_size(0, 0, w, h, color);
}

void rdpq_draw_filled_rectangle_size(int x, int y, int width, int height, color_t color)
{
    rdpq_set_mode_fill(color);
    rdpq_fill_rectangle(x, y, x + width, y + height);
}

void rdpq_draw_filled_rectangle_with_border_size(int x, int y, int width, int height, color_t color, color_t border_color)
{
    rdpq_set_mode_fill(border_color);
    rdpq_fill_rectangle(x, y, x + width, y + height);

    rdpq_set_mode_fill(color);
    rdpq_fill_rectangle(x + 2, y + 2, x + width - 2, y + height - 2);
}

void rdpq_draw_sprite(sprite_t *sp, int x, int y, mirror_t mirror)
{
    rdpq_blitparms_t parms = {0};

    switch (mirror)
    {
    case MIRROR_X:
        parms.flip_x = true;
        break;
    case MIRROR_Y:
        parms.flip_y = true;
        break;
    case MIRROR_XY:
        parms.flip_x = true;
        parms.flip_y = true;
        break;
    case MIRROR_NONE:
    default:
        break;
    }

    rdpq_set_mode_standard();
    if (sprite_get_format(sp) == FMT_RGBA32)
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_sprite_blit(sp, x, y, &parms);
}
