/* map.c -- map data
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "map.h"
#include "rdpq.h"

#define __ TILE_NONE
#define OO TILE_BASE
#define BT TILE_BETON
#define B1 TILE_BUILDING_RIGHT_RED_TWO
#define B2 TILE_BUILDING_RIGHT_RED_ONE
#define B3 TILE_BUILDING_LEFT_RED_THREE
#define B4 TILE_BUILDING_LEFT_WHITE_ONE
#define B5 TILE_BUILDING_RIGHT_YELLOW_FOUR
#define B6 TILE_BUILDING_LEFT_WHITE_SIX
#define B7 TILE_BUILDING_LEFT_BROWN_THREE
#define WR TILE_WAVES
#define WV TILE_WAVES
#define WL TILE_WALL
#define BW TILE_BROKEN_WALL

// Diamond-shaped map for horizontal scrolling
// Buildings are placed directly in the tiles array (no separate layer)
map_t game_map = {
    .width = MAP_WIDTH,
    .height = MAP_HEIGHT,
    .tiles = {
        //        0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26
        /*  0 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, __, OO, OO, OO, OO, __, __, __, __, __, __, __, __},
        /*  1 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, OO, BT, OO, OO, BT, OO, __, __, __, __, __, __, __},
        /*  2 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, OO, OO, OO, OO, OO, OO, OO, OO, __, __, __, __, __, __},
        /*  3 */ {__, __, __, __, __, __, __, __, __, __, __, __, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, __, __, __, __, __},
        /*  4 */ {__, __, __, __, __, __, __, __, __, __, __, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, __, __, __, __},
        /*  5 */ {__, __, __, __, __, __, __, __, __, __, OO, OO, OO, OO, OO, OO, B3, OO, OO, OO, OO, OO, OO, OO, __, __, __},
        /*  6 */ {__, __, __, __, __, __, __, __, __, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, __, __},
        /*  7 */ {__, __, __, __, __, __, __, __, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, __},
        /*  8 */ {__, __, __, __, __, __, __, WL, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, B6, OO, OO, OO, OO},
        /*  9 */ {__, __, __, __, __, __, WV, WL, OO, OO, OO, B4, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO},
        /* 10 */ {__, __, __, __, __, WR, WV, WL, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO},
        /* 11 */ {__, __, __, __, WV, WR, WV, WL, OO, OO, OO, OO, OO, OO, OO, OO, B7, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO},
        /* 12 */ {__, __, __, WR, WV, WR, WV, WL, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, B2, OO, OO, OO, OO, __},
        /* 13 */ {__, __, WV, WR, WV, WR, WR, WL, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, __, __},
        /* 14 */ {__, WR, WV, WR, WV, WR, WV, WL, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, __, __, __},
        /* 15 */ {WV, WR, WR, WV, WR, WV, WR, WL, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, __, __, __, __},
        /* 16 */ {WR, WV, WR, WR, WV, WR, WV, WL, OO, OO, OO, OO, OO, OO, OO, OO, B5, OO, OO, OO, OO, OO, __, __, __, __, __},
        /* 17 */ {WV, WR, WV, WR, WR, WV, WR, WL, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, __, __, __, __, __, __},
        /* 18 */ {WR, WV, WR, WV, WR, WR, WV, WL, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, __, __, __, __, __, __, __},
        /* 19 */ {__, WR, WV, WR, WV, WR, WV, WL, OO, OO, OO, OO, OO, OO, B1, OO, OO, OO, OO, __, __, __, __, __, __, __, __},
        /* 20 */ {__, __, WV, WR, WV, WR, WR, WL, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, __, __, __, __, __, __, __, __, __},
        /* 21 */ {__, __, __, WR, WV, WR, WV, WL, OO, OO, OO, OO, OO, OO, OO, OO, OO, __, __, __, __, __, __, __, __, __, __},
        /* 22 */ {__, __, __, __, WV, WR, WV, WL, OO, OO, OO, OO, OO, OO, OO, OO, __, __, __, __, __, __, __, __, __, __, __},
        /* 23 */ {__, __, __, __, __, WR, WV, WL, OO, OO, OO, OO, OO, OO, OO, __, __, __, __, __, __, __, __, __, __, __, __},
        /* 24 */ {__, __, __, __, __, __, WV, WL, OO, OO, OO, OO, OO, OO, __, __, __, __, __, __, __, __, __, __, __, __, __},
        /* 25 */ {__, __, __, __, __, __, __, WL, OO, OO, OO, OO, OO, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
        /* 26 */ {__, __, __, __, __, __, __, __, OO, OO, OO, OO, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
    },
    .row_start = {
        15,
        14,
        13,
        12,
        11,
        10,
        9,
        8,
        7,
        6,
        5,
        4,
        3,
        2,
        1,
        0,
        0,
        0,
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
    }};

#undef __
#undef OO
#undef BT
#undef B1
#undef B2
#undef B3
#undef B4
#undef B5
#undef B6
#undef B7
#undef WR
#undef WV
#undef WL
#undef BW

/**
 * @brief is_building_tile: Return true if tile draws in building pass.
 */
static bool is_building_tile(int tile_id)
{
    return tile_id == TILE_BUILDING_RIGHT_RED_TWO ||
           tile_id == TILE_BETON ||
           tile_id == TILE_BUILDING_RIGHT_RED_ONE ||
           tile_id == TILE_BUILDING_LEFT_RED_THREE ||
           tile_id == TILE_BUILDING_LEFT_WHITE_ONE ||
           tile_id == TILE_BUILDING_RIGHT_YELLOW_FOUR ||
           tile_id == TILE_BUILDING_LEFT_WHITE_SIX ||
           tile_id == TILE_BUILDING_LEFT_BROWN_THREE;
}

/**
 * @brief get_building_sprite: Return sprite for building tile id.
 */
static sprite_t *get_building_sprite(const map_render_t *render, int tile_id)
{
    switch (tile_id)
    {
    case TILE_BUILDING_RIGHT_RED_TWO:
        return render->building_right_red_two;
    case TILE_BETON:
        return render->beton_sprite;
    case TILE_BUILDING_RIGHT_RED_ONE:
        return render->building_right_red_one;
    case TILE_BUILDING_LEFT_RED_THREE:
        return render->building_left_red_three;
    case TILE_BUILDING_LEFT_WHITE_ONE:
        return render->building_left_white_one;
    case TILE_BUILDING_RIGHT_YELLOW_FOUR:
        return render->building_right_yellow_four;
    case TILE_BUILDING_LEFT_WHITE_SIX:
        return render->building_left_white_six;
    case TILE_BUILDING_LEFT_BROWN_THREE:
        return render->building_left_brown_three;
    default:
        return NULL;
    }
}

/**
 * @brief map_draw: Draw tiles in row order.
 */
void map_draw(const map_t *map, const map_render_t *render, int cam_x, int view_width)
{
    const int origin_x = render->origin_x;
    const int origin_y = render->origin_y;
    const int screen_h = display_get_height();

    int half_w = render->step_x / 2;
    int half_h = render->step_y;

    for (int y = 0; y < map->height; y++)
    {
        bool drawn = false;
        for (int x = map->row_start[y]; x < map->width; x++)
        {
            int tile_id = map->tiles[y][x];
            sprite_t *tile = render->tile_sprites[tile_id];

            if (is_building_tile(tile_id))
            {
                tile = render->base_tile;
            }

            // TILE_NONE = skip drawing
            if (!tile)
            {
                if (drawn)
                    break;
                continue;
            }

            int world_x = origin_x + (x - y) * half_w;
            int world_y = origin_y + (x + y) * half_h;
            int screen_x = world_x - cam_x;
            int screen_y = world_y - render->cam_y - (tile->height - render->spr_h);

            if (screen_x < -tile->width || screen_x > view_width ||
                screen_y < -tile->height || screen_y > screen_h)
            {
                if (drawn)
                    break;
                continue;
            }

            rdpq_sprite_blit(tile, screen_x, screen_y, NULL);
            drawn = true;
        }
    }
}

/**
 * @brief map_draw_buildings: Draw building tiles on top.
 */
void map_draw_buildings(const map_t *map, const map_render_t *render, int cam_x, int view_width)
{
    const int origin_x = render->origin_x;
    const int origin_y = render->origin_y;
    const int screen_h = display_get_height();

    int half_w = render->step_x / 2;
    int half_h = render->step_y;
    for (int y = 0; y < map->height; y++)
    {
        for (int x = map->row_start[y]; x < map->width; x++)
        {
            int tile_id = map->tiles[y][x];
            sprite_t *building = get_building_sprite(render, tile_id);

            if (!building)
                continue;

            int world_x = origin_x + (x - y) * half_w;
            int world_y = origin_y + (x + y) * half_h;
            int screen_x = world_x - cam_x;
            int screen_y = world_y - render->cam_y - (building->height - render->spr_h);

            if (screen_x < -building->width || screen_x > view_width ||
                screen_y < -building->height || screen_y > screen_h)
                continue;

            rdpq_sprite_blit(building, screen_x, screen_y, NULL);
        }
    }
}
