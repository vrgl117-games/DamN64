/* map.c -- map data
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "game.h"

#define BA TILE_BASE
#define TL TILE_WALL_TOP_LEFT
#define TR TILE_WALL_TOP_RIGHT
#define BL TILE_WALL_BOTTOM_LEFT
#define BR TILE_WALL_BOTTOM_RIGHT

// game_map: Static map tile layout.
map_t game_map = {
    .width = MAP_WIDTH,
    .height = MAP_HEIGHT,
    .tiles = {
        {TL, TR, TR, TR, TR, TR, TR, TR, TR, TR},
        {TL, BA, BA, BA, BA, BA, BA, BA, BA, BR},
        {TL, BA, BA, BA, BA, BA, BA, BA, BA, BR},
        {TL, BA, BA, BA, BA, BA, BA, BA, BA, BR},
        {TL, BA, BA, BA, BA, BA, BA, BA, BA, BR},
        {TL, BA, BA, BA, BA, BA, BA, BA, BA, BR},
        {TL, BA, BA, BA, BA, BA, BA, BA, BA, BR},
        {TL, BA, BA, BA, BA, BA, BA, BA, BA, BR},
        {TL, BA, BA, BA, BA, BA, BA, BA, BA, BR},
        {BL, BL, BL, BL, BL, BL, BL, BL, BL, BR},
    }};

#undef BA
#undef TL
#undef TR
#undef BL
#undef BR
