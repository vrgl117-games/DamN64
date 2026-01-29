/*
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "controls.h"
#include "sprite.h"
#include <stdbool.h>

#define ISO_H_HALF 16

typedef bool (*character_block_fn)(int world_x, int world_y);

// character_init: Load vehicle sprites and initialize character state.
void character_init(int base_x, int base_y, int offset_x, int offset_y, int cam_y);

// character_update: Update movement, facing, and active character(s).
void character_update(const control_t *keys[2], character_block_fn is_blocked);

// character_draw: Render both vehicles in depth order.
void character_draw(int cam_x);

// character_draw_single: Render a single vehicle by index.
void character_draw_single(int index, int cam_x);

// character_get_position: Fetch a character's world position.
void character_get_position(int index, int *x, int *y);

// character_get_active_player: Return active player index.
int character_get_active_player(void);

// character_get_vehicle_sprite: Fetch vehicle sprite for color and direction.
sprite_t *character_get_vehicle_sprite(int color_index, int dir);

#endif // __CHARACTER_H__
