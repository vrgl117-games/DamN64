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

// game_init: Initialize game state.
void game_init(void);
// game_update: Update movement, facing, and camera.
void game_update(control_t *keys[2]);
// game_draw: Render the scene.
void game_draw(display_context_t disp);
// game_draw_title_background: Draw a single tiled band.
void game_draw_title_background(int screen_w, int screen_h);
// game_draw_title_band: Draw a single tiled band using a tile sprite.
void game_draw_title_band(sprite_t *tile, int screen_w, int screen_h);
// game_get_tile_sprite: Return tile sprite by tile id.
sprite_t *game_get_tile_sprite(tile_id_t tile_id);
// game_load_sprites: Load sprites.
void game_load_sprites(void);
#endif //__GAME_H__
