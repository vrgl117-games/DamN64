/* map.c -- map data
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "game.h"

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
    }};

#undef __
#undef BA
#undef BD
