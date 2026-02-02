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

void screen_init();
bool screen_intro(display_context_t disp);
void screen_timer_title();
void screen_title(display_context_t disp);
void screen_story(display_context_t disp, control_t *keys[2]);
void screen_single_disclaimer(display_context_t disp, control_t *keys[2]);
bool screen_game(display_context_t disp, control_t *keys[2]);
void screen_game_over(display_context_t disp, control_t *keys[2]);

#endif //__SCREENS_H__
