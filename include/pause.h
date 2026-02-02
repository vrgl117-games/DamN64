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

// pause_init: Initialize pause menu state.
void pause_init(void);
// pause_is_active: Return whether pause menu is active.
bool pause_is_active(void);
// pause_get_music_on: Return music toggle state.
bool pause_get_music_on(void);
// pause_get_rumble_on: Return rumble toggle state.
bool pause_get_rumble_on(void);
// pause_handle_input: Update pause menu state based on input.
void pause_handle_input(control_t *keys[2]);
// pause_draw: Render pause menu overlay.
void pause_draw(display_context_t disp);

#endif //__PAUSE_H__
