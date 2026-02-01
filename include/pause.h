/* pause.h -- pause menu helpers
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef __PAUSE_H__
#define __PAUSE_H__

#include <stdbool.h>

#include <libdragon.h>

#include "controls.h"

void pause_init(void);
bool pause_is_active(void);
bool pause_get_music_on(void);
bool pause_get_rumble_on(void);
void pause_handle_input(control_t *keys[2]);
void pause_draw(display_context_t disp);

#endif //__PAUSE_H__
