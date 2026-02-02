/* main.c -- main implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include <stdbool.h>
#include <stdlib.h>

#include "bgm.h"
#include "character.h"
#include "controls.h"
#include "fps.h"
#include "font.h"
#include "game.h"
#include "pause.h"
#include "screens.h"

#ifndef NDEBUG
screen_t screen = title;
#else
screen_t screen = intro;
#endif

static bool single_disclaimer_shown = false;

// main: Initialize systems and run main loop.
int main()
{
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
    dfs_init(DFS_DEFAULT_LOCATION);
    rdpq_init();
#ifndef NDEBUG
    debug_init(DEBUG_FEATURE_LOG_ISVIEWER);
    rdpq_debug_start();
#endif
    bgm_init();
    joypad_init();
    timer_init();
    font_init();
    pause_init();

    game_load_sprites();
    character_load_vehicle_sprites();

    srand(TICKS_READ());

    new_timer(TIMER_TICKS(50000), TF_CONTINUOUS, screen_timer_title);
    surface_t *disp = NULL;

    if (screen != intro) // auto start bgm if skipping intro
        bgm_start();

    while (true)
    {
        // get the keys pressed.
        control_t **keys = controls_get_keys();

        // wait for the screen to be availalble.
        while (!(disp = display_try_get()))
            ;

        switch (screen) // state machine for the screens. intro -> title -> story -> game.
        {
        case intro: // n64 logo and vrgl117 logo.
            if (screen_intro(disp))
            {
                screen = title;
                bgm_start();
            }
            break;
        case title: // press start.
            screen_title(disp);
            if (keys[0]->start || keys[0]->A || keys[0]->B)
            {
                if (!single_disclaimer_shown && keys[1] == NULL)
                {
                    single_disclaimer_shown = true;
                    screen = single_disclaimer;
                }
                else
                {
                    screen = story;
                }
            }
            break;
        case single_disclaimer:
            screen_single_disclaimer(disp, keys);
            if (keys[0]->start || keys[0]->A || keys[0]->B)
                screen = story;
            break;
        case story: // story screen.
            screen_story(disp, keys);
            if (keys[0]->start || keys[0]->A || keys[0]->B)
            {
                game_init();
                screen = game;
            }
            break;
        case game: // main game loop
            if (screen_game(disp, keys))
                screen = game_over;
            break;
        case game_over:
            screen_game_over(disp, keys);
            if (keys[0]->start || keys[0]->A || keys[0]->B)
            {
                pause_init();
                game_init();
                screen = title;
            }
            break;
        }

#ifndef NDEBUG
        fps_tick();
#endif
        bgm_update();
    }

    // cleanup, never called
    timer_close();
    rdpq_close();
    display_close();
    return 0;
}
