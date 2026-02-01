/* screens.c -- screens helpers implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "screens.h"

#include "bgm.h"
#include "fps.h"
#include "font.h"
#include "game.h"
#include "pause.h"
#include "rdpq.h"
#include "sprite.h"
#include "title.h"

extern const color_t background;

static volatile int tick = 0;

// screen_init: Initialize screen state.
void screen_init()
{
}

// display the n64 logo and then the VRGL117 Games logo.
// return true when the animation is done.
// screen_intro: Render intro sequence, returns true when done.
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

    rdpq_attach_clear(disp, NULL);
    fps_draw();
    if (intro != NULL)
    {
        int screen_w = display_get_width();
        int screen_h = display_get_height();
        int intro_x = (screen_w / 2) - (intro->width / 2);
        int intro_y = (screen_h * 5) / 16;
        rdpq_draw_faded_sprite(intro, intro_x, intro_y, alpha);
        rdpq_detach_show();
        sprite_free(intro);
    }
    else
        rdpq_detach_show();

    return (anim >= 82);
}

// screen_timer_title: Advance intro timer.
void screen_timer_title()
{
    tick++;
}

// screen_title: Render title screen.
void screen_title(display_context_t disp)
{
    const char *title = "DamN64";
    const char *prompt = "Press Any Button";
    int screen_w = display_get_width();
    int screen_h = display_get_height();
    int title_y = screen_h / 4;
    int prompt_y = title_y + (screen_h * 13) / 24;

    title_update(screen_w);

    rdpq_attach(disp, NULL);
    game_draw_title_background(screen_w, screen_h);
    title_draw(screen_w, screen_h);
    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                    FONT_PIXEL_SQUARE, 0, title_y, title);
    if (tick % 30 < 15)
        rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                        FONT_PIXEL, 0, prompt_y, prompt);
    fps_draw();
    rdpq_detach_show();
}

// screen_story: Render story screen.
void screen_story(display_context_t disp, control_t *keys[2])
{
    const char *top = "Damn!! The Dam broke!";

    const char *story_line = "It needs to be repaired before the city floods !";
    const char *tutorial_line_0 = "Use the trucks                          to bring";
    const char *tutorial_line_1 = "concrete from the factory";
    const char *tutorial_line_2 = "to the broken sections                       to fix them asap!";
    const char *continue_prompt = "Continue...";
    int screen_w = display_get_width();

    (void)keys;

    sprite_t *beton_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_beton.sprite");
    sprite_t *broken_wall_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_broken_wall.sprite");
    sprite_t *truck_yellow = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_yellow_SE.sprite");
    sprite_t *truck_red = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_red_SE.sprite");

    rdpq_attach(disp, NULL);
    rdpq_clear(background);
    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                    FONT_PIXEL_SQUARE, 0, display_get_height() / 4, top);
    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                    FONT_PIXEL, 0, 78, story_line);

    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                    FONT_PIXEL, 0, 116, tutorial_line_0);
    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                    FONT_PIXEL, 0, 146, tutorial_line_1);
    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                    FONT_PIXEL, 0, 177, tutorial_line_2);

    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                    FONT_PIXEL, 0, 220, continue_prompt);

    if (beton_tile && broken_wall_tile && truck_yellow && truck_red)
    {
        rdpq_set_mode_copy(true);
        rdpq_sprite_blit(beton_tile, 240, 124, NULL);
        rdpq_sprite_blit(broken_wall_tile, 160, 156, NULL);
        rdpq_sprite_blit(truck_yellow, 156, 106, NULL);
        rdpq_sprite_blit(truck_red, 178, 106, NULL);
    }

    fps_draw();
    rdpq_detach_show();

    if (beton_tile)
        sprite_free(beton_tile);
    if (broken_wall_tile)
        sprite_free(broken_wall_tile);
    if (truck_yellow)
        sprite_free(truck_yellow);
    if (truck_red)
        sprite_free(truck_red);
}

// screen_game: Update and render the game screen.
void screen_game(display_context_t disp, control_t *keys[2])
{
    pause_handle_input(keys);
    if (pause_is_active())
        pause_draw(disp);
    else
    {
        game_update(keys);
        game_draw(disp);
    }
}
