/* screens.h -- screens helpers header
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef __SCREENS_H__
#define __SCREENS_H__

#include "controls.h"

typedef enum screens
{
    intro,
    title,
    single_disclaimer,
    story,
    game,
    game_over,
} screen_t;

// screen_init: Initialize screen state.
void screen_init();
// screen_intro: Render intro sequence, returns true when done.
bool screen_intro(display_context_t disp);
// screen_timer_title: Advance intro timer.
void screen_timer_title();
// screen_title: Render title screen.
void screen_title(display_context_t disp);
// screen_story: Render story screen.
void screen_story(display_context_t disp, control_t *keys[2]);
// screen_single_disclaimer: Render single-player disclaimer screen.
void screen_single_disclaimer(display_context_t disp, control_t *keys[2]);
// screen_game: Update and render the game screen.
bool screen_game(display_context_t disp, control_t *keys[2]);
// screen_game_over: Render game over screen.
void screen_game_over(display_context_t disp, control_t *keys[2]);

#endif //__SCREENS_H__
