/* pause.c -- pause menu helpers
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "pause.h"

#include <stdio.h>

#include "bgm.h"
#include "font.h"

extern const color_t background;

typedef enum
{
    PAUSE_ROW_MUSIC = 0,
    PAUSE_ROW_RUMBLE = 1,
    PAUSE_ROW_COUNT
} pause_row_t;

static bool pause_active = false;
static bool music_on = true;
static bool rumble_on = true;
static bool rumble_available = false;
static pause_row_t selected_row = PAUSE_ROW_MUSIC;
static direction_t prev_direction = d_none;

/**
 * @brief pause_init: Initialize pause menu state.
 */
void pause_init(void)
{
    pause_active = false;
    music_on = true;
    rumble_on = true;
    rumble_available = false;
    selected_row = PAUSE_ROW_MUSIC;
    prev_direction = d_none;
}

/**
 * @brief pause_is_active: Return whether pause menu is active.
 */
bool pause_is_active(void)
{
    return pause_active;
}

/**
 * @brief pause_get_music_on: Return music toggle state.
 */
bool pause_get_music_on(void)
{
    return music_on;
}

/**
 * @brief pause_get_rumble_on: Return rumble toggle state.
 */
bool pause_get_rumble_on(void)
{
    return rumble_on;
}

/**
 * @brief toggle_pause: Toggle pause state.
 */
static void toggle_pause(void)
{
    pause_active = !pause_active;
}

/**
 * @brief pause_handle_input: Update pause menu state based on input.
 */
void pause_handle_input(control_t *keys[2])
{
    if (!keys || !keys[0])
        return;

    control_t *p1 = keys[0];
    rumble_available = p1->rumble;

    if (!rumble_available && selected_row == PAUSE_ROW_RUMBLE)
        selected_row = PAUSE_ROW_MUSIC;

    if (p1->start)
    {
        toggle_pause();
        if (pause_active)
        {
            if (music_on)
            {
                bgm_set_track(BGM_TRACK_INTRO_PAUSE);
                bgm_pause();
            }
            else
            {
                bgm_mute();
            }
        }
        else
        {
            if (music_on)
            {
                bgm_set_track(BGM_TRACK_JOY);
                bgm_play();
            }
            else
            {
                bgm_mute();
            }
        }
        prev_direction = p1->direction;
        return;
    }

    if (!pause_active)
    {
        prev_direction = p1->direction;
        return;
    }

    direction_t just_pressed = (direction_t)(p1->direction & ~prev_direction);

    if (just_pressed & d_up)
    {
        if (selected_row > 0)
            selected_row--;
    }
    if (just_pressed & d_down)
    {
        if (selected_row < PAUSE_ROW_COUNT - 1 && rumble_available)
            selected_row++;
    }

    if ((just_pressed & d_left) || (just_pressed & d_right))
    {
        if (selected_row == PAUSE_ROW_MUSIC)
        {
            music_on = !music_on;
            if (music_on)
            {
                bgm_set_track(BGM_TRACK_INTRO_PAUSE);
                bgm_pause();
            }
            else
            {
                bgm_mute();
            }
        }
        else if (selected_row == PAUSE_ROW_RUMBLE)
        {
            if (rumble_available)
                rumble_on = !rumble_on;
        }
    }

    prev_direction = p1->direction;
}

/**
 * @brief pause_draw: Render pause menu overlay.
 */
void pause_draw(display_context_t disp)
{
    const char *title = "PAUSE";
    char music_line[32];
    char rumble_line[32];
    int screen_w = display_get_width();
    int screen_h = display_get_height();

    if (!pause_active)
        return;

    snprintf(music_line, sizeof(music_line), "< Music %s >", music_on ? "ON" : "OFF");

    if (selected_row == PAUSE_ROW_MUSIC)
        snprintf(music_line, sizeof(music_line), "Music: < %s >", music_on ? "ON" : "OFF");
    else
        snprintf(music_line, sizeof(music_line), "Music:   %s  ", music_on ? "ON" : "OFF");

    if (!rumble_available)
    {
        snprintf(rumble_line, sizeof(rumble_line), "Rumble:  NO PAK DETECTED");
    }
    else if (selected_row == PAUSE_ROW_RUMBLE)
    {
        snprintf(rumble_line, sizeof(rumble_line), "Rumble: < %s >", rumble_on ? "ON" : "OFF");
    }
    else
    {
        snprintf(rumble_line, sizeof(rumble_line), "Rumble:   %s  ", rumble_on ? "ON" : "OFF");
    }

    rdpq_attach(disp, NULL);
    rdpq_clear(background);

    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                    FONT_PIXEL_SQUARE, 0, screen_h / 6, title);

    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                    FONT_PIXEL, 0, screen_h / 2 - 16, music_line);
    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_CENTER},
                    FONT_PIXEL, 0, screen_h / 2 + 16, rumble_line);

    rdpq_detach_show();
}
