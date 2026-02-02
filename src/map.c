/* map.c -- map data
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "map.h"
#include "rdpq.h"

#include <string.h>

#define ___ TILE_NONE
#define ooo TILE_BASE
#define BRD TILE_BETON_RED
#define BYL TILE_BETON_YELLOW
#define BR2 TILE_BUILDING_RIGHT_RED_TWO
#define BR1 TILE_BUILDING_RIGHT_BROWN_TWO
#define BLR TILE_BUILDING_LEFT_RED_THREE
#define BLW TILE_BUILDING_LEFT_WHITE_ONE
#define BLY TILE_BUILDING_RIGHT_YELLOW_FOUR
#define BL6 TILE_BUILDING_LEFT_WHITE_SIX
#define BLB TILE_BUILDING_LEFT_BROWN_THREE
#define RLF TILE_ROAD_LEFT
#define RRT TILE_ROAD_RIGHT
#define RTT TILE_ROAD_T
#define RDL TILE_ROAD_D_LEFT
#define RDR TILE_ROAD_D_RIGHT
#define RC1 TILE_ROAD_CORNER_1
#define RC2 TILE_ROAD_CORNER_2
#define RC3 TILE_ROAD_CORNER_3
#define RC4 TILE_ROAD_CORNER_4
#define TRE TILE_TREE
#define TFT TILE_FOUNTAIN
#define WAV TILE_WAVES
#define WVB TILE_WAVES_BOAT
#define WAL TILE_WALL
#define BRK TILE_BROKEN_WALL
#define BWW TILE_BROKEN_WALL_WATER

// Diamond-shaped map for horizontal scrolling
// Buildings are placed directly in the tiles array (no separate layer)
static const map_t oooe_map = {
    .width = MAP_WIDTH,
    .height = MAP_HEIGHT,
    .tiles = {
        //        0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26
        /*  0 */ {___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ooo, ooo, ooo, ooo, ___, ___, ___, ___, ___, ___, ___, ___},
        /*  1 */ {___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ooo, BRD, ooo, ooo, BYL, ooo, ___, ___, ___, ___, ___, ___, ___},
        /*  2 */ {___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ooo, ooo, RRT, ooo, ooo, RRT, ooo, ooo, ___, ___, ___, ___, ___, ___},
        /*  3 */ {___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, RDL, RLF, RDL, RTT, RDL, RLF, RTT, RLF, RDL, RC3, ___, ___, ___, ___, ___},
        /*  4 */ {___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, RRT, ooo, ___, ___, ___, ___},
        /*  5 */ {___, ___, ___, ___, ___, ___, ___, ___, ___, ___, BLB, BLB, BL6, BL6, BLW, BLW, BLR, BLB, BLB, BL6, ooo, RDR, ooo, ooo, ___, ___, ___},
        /*  6 */ {___, ___, ___, ___, ___, ___, ___, ___, ___, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, RRT, ooo, ooo, ooo, ___, ___},
        /*  7 */ {___, ___, ___, ___, ___, ___, ___, ___, BLY, ooo, ooo, ooo, RC1, RLF, RDL, RLF, RDL, RLF, RDL, RLF, RLF, RC2, ooo, ooo, ooo, ooo, ___},
        /*  8 */ {___, ___, ___, ___, ___, ___, ___, WAL, ooo, BR1, ooo, ooo, RDR, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo},
        /*  9 */ {___, ___, ___, ___, ___, ___, WAV, WAL, ooo, ooo, BR2, ooo, RRT, ooo, BLB, BLB, BLB, BL6, BLB, BLB, BL6, BLB, BLB, BL6, BL6, BLB, BLB},
        /* 10 */ {___, ___, ___, ___, ___, WAV, WAV, WAL, ooo, ooo, BLY, ooo, RDR, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo},
        /* 11 */ {___, ___, ___, ___, WAV, WAV, WAV, WAL, ooo, ooo, BLY, ooo, RC3, RLF, RDL, RLF, RDL, RLF, RDL, RLF, RC3, ooo, TFT, ooo, TRE, ooo, ooo},
        /* 12 */ {___, ___, ___, WAV, WAV, WVB, WAV, WAL, ooo, ooo, BR2, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, RDR, ooo, ooo, ooo, ooo, ooo, ___},
        /* 13 */ {___, ___, WAV, WAV, WAV, WAV, WAV, WAL, ooo, ooo, BR1, BL6, BL6, BLB, BL6, BL6, BLB, BL6, BL6, ooo, RRT, ooo, TRE, ooo, ooo, ___, ___},
        /* 14 */ {___, WAV, WVB, WAV, WAV, WAV, WAV, WAL, ooo, ooo, BR2, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, RDR, ooo, ooo, ooo, ___, ___, ___},
        /* 15 */ {WAV, WAV, WAV, WAV, WAV, WAV, WAV, WAL, ooo, ooo, BR2, ooo, RC1, RLF, RDL, RLF, RDL, RLF, RDL, RLF, RC4, ooo, ooo, ___, ___, ___, ___},
        /* 16 */ {WAV, WAV, WAV, WAV, WVB, WAV, WAV, WAL, ooo, ooo, BLY, ooo, RDR, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ooo, ___, ___, ___, ___, ___},
        /* 17 */ {WAV, WAV, WAV, WVB, WAV, WAV, WAV, WAL, ooo, ooo, BLY, ooo, RRT, ooo, BR2, ooo, TRE, ooo, TFT, ooo, ooo, ___, ___, ___, ___, ___, ___},
        /* 18 */ {WAV, WVB, WAV, WAV, WAV, WAV, WAV, WAL, ooo, ooo, BR2, ooo, RDR, ooo, BR2, ooo, ooo, ooo, ooo, ooo, ___, ___, ___, ___, ___, ___, ___},
        /* 19 */ {___, WAV, WAV, WAV, WAV, WAV, WAV, WAL, ooo, ooo, BR1, ooo, RRT, ooo, BR2, ooo, TFT, ooo, ooo, ___, ___, ___, ___, ___, ___, ___, ___},
        /* 20 */ {___, ___, WAV, WAV, WVB, WAV, WAV, WAL, ooo, ooo, ooo, ooo, RDR, ooo, BLY, ooo, ooo, ooo, ___, ___, ___, ___, ___, ___, ___, ___, ___},
        /* 21 */ {___, ___, ___, WAV, WAV, WAV, WAV, WAL, ooo, ooo, RC1, RLF, RC4, ooo, BR2, ooo, ooo, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___},
        /* 22 */ {___, ___, ___, ___, WAV, WAV, WAV, WAL, ooo, ooo, RDR, ooo, ooo, ooo, BR2, ooo, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___},
        /* 23 */ {___, ___, ___, ___, ___, WVB, WAV, WAL, ooo, ooo, RRT, ooo, ooo, ooo, BR2, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___},
        /* 24 */ {___, ___, ___, ___, ___, ___, WAV, WAL, ooo, ooo, RDR, ooo, ooo, ooo, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___},
        /* 25 */ {___, ___, ___, ___, ___, ___, ___, WAL, ooo, ooo, RRT, ooo, ooo, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___},
        /* 26 */ {___, ___, ___, ___, ___, ___, ___, ___, ooo, ooo, RDR, ooo, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___},
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

map_t game_map = {0};

#undef ___
#undef ooo
#undef BRD
#undef BYL
#undef BR2
#undef BR1
#undef BLR
#undef BLW
#undef BLY
#undef BL6
#undef BLB
#undef RLF
#undef RRT
#undef RTT
#undef RDL
#undef RDR
#undef RC1
#undef RC2
#undef RC3
#undef RC4
#undef TRE
#undef TFT
#undef WAV
#undef WVB
#undef WAL
#undef BRK
#undef BWW

// map_reset: Restore the map to its initial state.
void map_reset(void)
{
    memcpy(&game_map, &oooe_map, sizeof(map_t));
}

/**
 * @brief is_building_tile: Return true if tile draws in building pass.
 */
static bool is_building_tile(int tile_id)
{
    return tile_id == TILE_BUILDING_RIGHT_RED_TWO ||
           tile_id == TILE_BETON ||
           tile_id == TILE_BETON_RED ||
           tile_id == TILE_BETON_YELLOW ||
           tile_id == TILE_BUILDING_RIGHT_BROWN_TWO ||
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
    case TILE_BETON_RED:
        return render->beton_red_sprite;
    case TILE_BETON_YELLOW:
        return render->beton_yellow_sprite;
    case TILE_BUILDING_RIGHT_BROWN_TWO:
        return render->building_right_brown_two;
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
