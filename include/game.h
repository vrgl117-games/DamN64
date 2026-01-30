/* game.h -- game helpers header
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef __GAME_H__
#define __GAME_H__

#include "controls.h"
#include "map.h"

void game_init(void);
void game_update(control_t *keys[2]);
void game_draw(display_context_t disp);
void game_draw_title_background(int screen_w, int screen_h);

#endif //__GAME_H__
