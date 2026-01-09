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

#define MAP_WIDTH 10
#define MAP_HEIGHT 10

typedef enum
{
    TILE_NONE = 0,
    TILE_BASE = 1,
    TILE_WALL_BOTTOM_LEFT = 2,
    TILE_WALL_BOTTOM_RIGHT = 3,
    TILE_WALL_TOP_LEFT = 4,
    TILE_WALL_TOP_RIGHT = 5,
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
void game_update(control_t keys);
void game_draw(display_context_t disp);

#endif //__GAME_H__
