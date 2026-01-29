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

#define MAP_WIDTH 27
#define MAP_HEIGHT 27

typedef enum
{
    TILE_NONE = 0,
    TILE_BASE = 1,
    TILE_BUILDING = 2,
    TILE_WATER = 3,
    TILE_WALL = 4,
    TILE_BROKEN_WALL = 5,
    TILE_COUNT
} tile_id_t;

typedef struct
{
    int width;
    int height;
    int tiles[MAP_HEIGHT][MAP_WIDTH];
} map_t;

extern map_t game_map;

void game_init(void);
void game_update(control_t *keys[2]);
void game_draw(display_context_t disp);
void game_draw_title_background(int screen_w, int screen_h);

#endif //__GAME_H__
