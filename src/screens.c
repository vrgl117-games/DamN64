/* screens.c -- screens helpers implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "rdpq.h"
#include "screens.h"
#include "sprite.h"

static volatile int tick = 0;

void screen_init()
{
}

// display the n64 logo and then the VRGL117 Games logo.
// return true when the animation is done.
bool screen_intro(display_context_t disp)
{
    static int anim = 0;
    int alpha = 255;

    if (tick > 0)
    {
        anim++;
        tick = 0;
    }

    sprite_t *intro = NULL;

    if (anim >= 1 && anim <= 39)
    {
        intro = sprite_load("rom:/gfx/sprites/n64brew.rgba16.sprite");

        if (anim <= 9) // fade in
            alpha = (anim - 1) * (255 / 9.0f);
        else if (anim <= 30) // full
            alpha = 255;
        else // fade out
        {
            int a = anim - 30; // 1..9
            alpha = 255 - a * (255 / 9.0f);
        }
    }

    if (anim >= 41 && anim <= 79)
    {
        intro = sprite_load("rom:/gfx/sprites/intro.rgba16.sprite");

        if (anim <= 49) // fade in
        {
            int a = anim - 40; // 1..9
            alpha = a * (255 / 9.0f);
        }
        else if (anim <= 70) // full
            alpha = 255;
        else // fade out
        {
            int a = anim - 70; // 1..9
            alpha = 255 - a * (255 / 9.0f);
        }
    }

    if (intro != NULL)
    {
        rdpq_attach_clear(disp, NULL);
        rdpq_draw_faded_sprite(intro, 320 - intro->width / 2, 150, alpha);
        rdpq_detach();
        sprite_free(intro);
    }

    return (anim >= 82);
}

void screen_timer_title()
{
    tick++;
}

void screen_title(display_context_t disp)
{
}
