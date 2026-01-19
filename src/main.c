/* main.c -- main implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include <stdlib.h>
#include "controls.h"
#include "font.h"
#include "game.h"
#include "screens.h"

screen_t screen = game;

// main: Initialize systems and run main loop.
int main()
{
    display_init(RESOLUTION_640x480, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
    dfs_init(DFS_DEFAULT_LOCATION);
    rdpq_init();
#ifndef NDEBUG
    debug_init(DEBUG_FEATURE_LOG_ISVIEWER);
    rdpq_debug_start();
#endif
    joypad_init();
    timer_init();

    srand(timer_ticks() & 0x7FFFFFFF);

    new_timer(TIMER_TICKS(50000), TF_CONTINUOUS, screen_timer_title);
    surface_t *disp = NULL;
    game_init();

    while (true)
    {
        // get the keys pressed.
        control_t keys = controls_get_keys();

        // wait for the screen to be availalble.
        while (!(disp = display_try_get()))
            ;

        switch (screen) // state machine for the screens. intro -> title -> game.
        {
        case intro: // n64 logo and vrgl117 logo.
            if (screen_intro(disp))
                screen = title;
            break;
        case title: // press start.
            screen_title(disp);
            break;
        case game: // main game loop
            screen_game(disp, keys);
        }

    }

    // cleanup, never called
    timer_close();
    rdpq_close();
    display_close();
    return 0;
}
