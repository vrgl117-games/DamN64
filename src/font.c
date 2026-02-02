/* font.c -- font helpers implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "font.h"

// font_init: Load fonts and register them with rdpq.
void font_init()
{
    rdpq_font_t *font_debug = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
    rdpq_text_register_font(FONT_DEBUG, font_debug);

    rdpq_font_t *font_title = rdpq_font_load("rom:/gfx/fonts/Kenney_Pixel_Square.font64");

    rdpq_font_style(font_title, 0, &(rdpq_fontstyle_t){.color = RGBA32(255, 255, 255, 255)});
    rdpq_text_register_font(FONT_PIXEL_SQUARE, font_title);

    rdpq_font_t *font_pixel = rdpq_font_load("rom:/gfx/fonts/Kenney_Pixel.font64");

    rdpq_font_style(font_pixel, 0, &(rdpq_fontstyle_t){.color = RGBA32(255, 255, 255, 255)});
    rdpq_text_register_font(FONT_PIXEL, font_pixel);
}
