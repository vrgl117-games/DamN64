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
#define BA TILE_BASE
#define BD TILE_BUILDING
#define WR TILE_WATER
#define WL TILE_WALL
#define BW TILE_BROKEN_WALL

// Diamond-shaped map for horizontal scrolling
// Buildings are placed directly in the tiles array (no separate layer)
map_t game_map = {
    .width = MAP_WIDTH,
    .height = MAP_HEIGHT,
    .tiles = {
        //        0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26
        /*  0 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, __, BA, BA, BA, BA, __, __, __, __, __, __, __, __},
        /*  1 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, BA, BA, BA, BA, BA, BA, __, __, __, __, __, __, __},
        /*  2 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __, __, __, __},
        /*  3 */ {__, __, __, __, __, __, __, __, __, __, __, __, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __, __, __},
        /*  4 */ {__, __, __, __, __, __, __, __, __, __, __, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __, __},
        /*  5 */ {__, __, __, __, __, __, __, __, __, __, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __},
        /*  6 */ {__, __, __, __, __, __, __, __, __, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __},
        /*  7 */ {__, __, __, __, __, __, __, __, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __},
        /*  8 */ {__, __, __, __, __, __, __, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA},
        /*  9 */ {__, __, __, __, __, __, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA},
        /* 10 */ {__, __, __, __, __, WL, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA},
        /* 11 */ {__, __, __, __, WR, WL, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA},
        /* 12 */ {__, __, __, WR, WR, WL, BA, BA, BD, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __},
        /* 13 */ {__, __, WR, WR, WR, WL, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __},
        /* 14 */ {__, WR, WR, WR, WR, WL, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __},
        /* 15 */ {WR, WR, WR, WR, WR, WL, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __, __},
        /* 16 */ {WR, WR, WR, WR, WR, WL, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __, __, __},
        /* 17 */ {WR, WR, WR, WR, WR, WL, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BD, __, __, __, __, __, __},
        /* 18 */ {WR, WR, WR, WR, WR, WL, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __, __, __, __, __},
        /* 19 */ {__, WR, WR, WR, WR, WL, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __, __, __, __, __, __},
        /* 20 */ {__, __, WR, WR, WR, WL, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __, __, __, __, __, __, __},
        /* 21 */ {__, __, __, WR, WR, WL, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __, __, __, __, __, __, __, __},
        /* 22 */ {__, __, __, __, WR, WL, BA, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __, __, __, __, __, __, __, __, __},
        /* 23 */ {__, __, __, __, __, WL, BA, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __, __, __, __, __, __, __, __, __, __},
        /* 24 */ {__, __, __, __, __, __, BA, BA, BA, BA, BA, BA, BA, BA, __, __, __, __, __, __, __, __, __, __, __, __, __},
        /* 25 */ {__, __, __, __, __, __, __, BA, BA, BA, BA, BA, BA, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
        /* 26 */ {__, __, __, __, __, __, __, __, BA, BA, BA, BA, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
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
#undef BA
#undef BD

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

            if (tile_id == TILE_BUILDING)
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
    sprite_t *building = render->building_sprite;

    if (!building)
        return;

    for (int y = 0; y < map->height; y++)
    {
        for (int x = map->row_start[y]; x < map->width; x++)
        {
            if (map->tiles[y][x] != TILE_BUILDING)
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
