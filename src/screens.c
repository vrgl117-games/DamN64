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
    const char *story_line_1 = "Damn! The dam broke and needs";
    const char *story_line_2 = "to be repaired before the city floods!";
    const char *continue_prompt = "Continue...";
    const char *single_player_line_1 = "This game is best experience with 2 controllers,";
    const char *single_player_line_2 = "use Z to switch character in single player mode.";
    int screen_w = display_get_width();
    int screen_h = display_get_height();
    int story_y = (screen_h / 2) - 20;
    int msg_y = screen_h - 90;
    int continue_y = screen_h - 40;
    bool single_controller = (keys[1] == NULL);

    rdpq_attach(disp, NULL);
    rdpq_clear(background);
    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                    FONT_PIXEL, 0, story_y, story_line_1);
    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                    FONT_PIXEL, 0, story_y + 32, story_line_2);

    if (single_controller)
    {
        rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                        FONT_PIXEL, 0, msg_y, single_player_line_1);
        rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                        FONT_PIXEL, 0, msg_y + 28, single_player_line_2);
    }

    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                    FONT_PIXEL, 0, continue_y, continue_prompt);

    fps_draw();
    rdpq_detach_show();
}

// screen_game: Update and render the game screen.
void screen_game(display_context_t disp, control_t *keys[2])
{
    game_update(keys);
    game_draw(disp);
}
