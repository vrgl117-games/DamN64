/* main.c -- main implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include <stdlib.h>

#include "bgm.h"
#include "controls.h"
#include "fps.h"
#include "font.h"
#include "game.h"
#include "pause.h"
#include "screens.h"

screen_t screen = game;

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

    srand(TICKS_READ());

    new_timer(TIMER_TICKS(50000), TF_CONTINUOUS, screen_timer_title);
    surface_t *disp = NULL;
    game_init();

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
                screen = story;
            break;
        case story: // story screen.
            screen_story(disp, keys);
            if (keys[0]->start || keys[0]->A || keys[0]->B)
                screen = game;
            break;
        case game: // main game loop
            screen_game(disp, keys);
        }

        fps_tick();
        bgm_update();
    }

    // cleanup, never called
    timer_close();
    rdpq_close();
    display_close();
    return 0;
}
