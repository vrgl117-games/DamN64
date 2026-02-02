/* fps.c -- fps counter implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef NDEBUG
#include "fps.h"

#include <stdint.h>

#include <libdragon.h>

#include "font.h"

static uint32_t fps_last_sec = 0;
static int fps_frames = 0;
static int fps_value = 60;

// fps_tick: Update FPS counter using libdragon ticks.
void fps_tick(void)
{
    uint32_t now = get_ticks();

    fps_frames++;

    if (now - fps_last_sec >= TICKS_PER_SECOND)
    {
        fps_value = fps_frames;
        fps_frames = 0;
        fps_last_sec = now;
    }
}

// fps_draw: Render FPS value in the top-right corner.
void fps_draw(void)
{
    int screen_w = display_get_width();

    rdpq_text_printf(&(rdpq_textparms_t){.width = screen_w - 8, .align = ALIGN_RIGHT},
                     FONT_DEBUG, 1, 10, "FPS: %d", fps_value);
}

#endif
