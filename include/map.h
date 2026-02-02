/* map.h -- map data and rendering
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef __MAP_H__
#define __MAP_H__

#include <libdragon.h>

#define MAP_WIDTH 27
#define MAP_HEIGHT 27

typedef enum
{
    TILE_NONE = 0,
    TILE_BASE = 1,
    TILE_BUILDING_RIGHT_RED_TWO = 2,
    TILE_WATER = 3,
    TILE_WALL = 4,
    TILE_BROKEN_WALL = 5,
    TILE_BROKEN_WALL_WATER = 6,
    TILE_WALL_WATER = 7,
    TILE_WAVES = 8,
    TILE_WAVES_BOAT = 9,
    TILE_BETON = 10,
    TILE_BETON_RED = 11,
    TILE_BETON_YELLOW = 12,
    TILE_BUILDING_RIGHT_BROWN_TWO = 13,
    TILE_BUILDING_LEFT_RED_THREE = 14,
    TILE_BUILDING_LEFT_WHITE_ONE = 15,
    TILE_BUILDING_RIGHT_YELLOW_FOUR = 16,
    TILE_BUILDING_LEFT_WHITE_SIX = 17,
    TILE_BUILDING_LEFT_BROWN_THREE = 18,
    TILE_ROAD_LEFT = 19,
    TILE_ROAD_RIGHT = 20,
    TILE_ROAD_T = 21,
    TILE_ROAD_D_LEFT = 22,
    TILE_ROAD_D_RIGHT = 23,
    TILE_ROAD_CORNER_1 = 24,
    TILE_ROAD_CORNER_2 = 25,
    TILE_ROAD_CORNER_3 = 26,
    TILE_ROAD_CORNER_4 = 27,
    TILE_TREE = 28,
    TILE_FOUNTAIN = 29,
    TILE_COUNT
} tile_id_t;

typedef struct
{
    int width;
    int height;
    int tiles[MAP_HEIGHT][MAP_WIDTH];
    int row_start[MAP_HEIGHT];
} map_t;

typedef struct
{
    int origin_x;
    int origin_y;
    int step_x;
    int step_y;
    int spr_h;
    int cam_y;
    sprite_t *tile_sprites[TILE_COUNT];
} map_render_t;

extern map_t game_map;

// map_reset: Restore the map to its initial state.
void map_reset(void);

// map_draw: Draw tiles in row order.
void map_draw(const map_t *map, const map_render_t *render, int cam_x, int view_width);
// map_draw_buildings: Draw building tiles on top.
void map_draw_buildings(const map_t *map, const map_render_t *render, int cam_x, int view_width);

#endif //__MAP_H__
