/* game.c -- game helpers implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */
#include "game.h"
#include "rdpq.h"
#include "sprite.h"
#include "character.h"
#include <math.h>

// Logical diamond footprint in screen space (not necessarily sprite size)
#define ISO_W 64
#define ISO_H 32

static sprite_t *base_tile = NULL;
static sprite_t *wall_bottom_left = NULL;
static sprite_t *wall_bottom_right = NULL;
static sprite_t *wall_top_left = NULL;
static sprite_t *wall_top_right = NULL;
static sprite_t *tile_sprites[TILE_COUNT] = {0};

static int cam_x = 0;
static int cam_y = 0;

// Sprite dimensions (actual texture)
static int spr_w = 0;
static int spr_h = 0;

// Steps derived from logical footprint
static int step_x = ISO_W;
static int step_y = ISO_H / 2;

// Offset if sprite is taller than the logical diamond
static int tile_draw_off_y = 0;

static int map_pixel_width = 0;
static int map_pixel_height = 0;
static int map_origin_x = 0;
static int map_origin_y = 0;

static const color_t tile_bg = RGBA32(132, 99, 64, 255);

// in_bounds: Check if a tile coordinate is inside the map.
static bool in_bounds(int x, int y)
{
    return (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT);
}

// is_blocking_tile: Return true if the tile blocks movement.
static bool is_blocking_tile(int tile)
{
    return tile == TILE_NONE || tile == TILE_WALL_BOTTOM_LEFT || tile == TILE_WALL_BOTTOM_RIGHT;
}

// point_in_diamond: Check if a world point falls inside a tile diamond.
static bool point_in_diamond(int world_x, int world_y, int grid_x, int grid_y, int half_w, int half_h)
{
    int center_x = map_origin_x + (grid_x - grid_y) * half_w + half_w;
    int center_y = map_origin_y + (grid_x + grid_y) * half_h + half_h;

    int dx = world_x - center_x;
    int dy = world_y - center_y;
    int adx = dx < 0 ? -dx : dx;
    int ady = dy < 0 ? -dy : dy;

    return (long long)adx * half_h + (long long)ady * half_w <= (long long)half_w * half_h;
}

// world_to_grid: Convert world coordinates to grid tile indices.
static bool world_to_grid(int world_x, int world_y, int *grid_x, int *grid_y)
{
    if (step_x <= 0 || step_y <= 0)
        return false;

    int half_w = step_x / 2;
    int half_h = step_y;

    if (half_w <= 0 || half_h <= 0)
        return false;

    double local_x = (double)(world_x - map_origin_x - half_w);
    double local_y = (double)(world_y - map_origin_y - half_h);

    double gx = (local_x / (double)half_w + local_y / (double)half_h) * 0.5;
    double gy = (local_y / (double)half_h - local_x / (double)half_w) * 0.5;

    int base_x = (int)floor(gx);
    int base_y = (int)floor(gy);

    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int cx = base_x + dx;
            int cy = base_y + dy;
            if (!in_bounds(cx, cy))
                continue;
            if (point_in_diamond(world_x, world_y, cx, cy, half_w, half_h))
            {
                *grid_x = cx;
                *grid_y = cy;
                return true;
            }
        }
    }

    return false;
}

// is_blocked_position: Return true if a world position is blocked.
static bool is_blocked_position(int world_x, int world_y)
{
    int grid_x = 0;
    int grid_y = 0;

    if (!world_to_grid(world_x, world_y, &grid_x, &grid_y))
        return true;

    int tile = game_map.tiles[grid_y][grid_x];
    return is_blocking_tile(tile);
}

// draw_map: Render the isometric tile map.
static void draw_map(map_t *map)
{
    const int origin_x = map_origin_x;
    const int origin_y = map_origin_y;

    const int screen_w = display_get_width();
    const int screen_h = display_get_height();

    rdpq_set_mode_standard();

    for (int y = 0; y < map->height; y++)
    {
        for (int x = 0; x < map->width; x++)
        {
            int half_w = step_x / 2;
            int half_h = step_y;
            int world_x = origin_x + (x - y) * half_w;
            int world_y = origin_y + (x + y) * half_h;
            int screen_x = world_x - cam_x;

            int tile_id = map->tiles[y][x];

            if (tile_id < 0 || tile_id >= TILE_COUNT)
                tile_id = TILE_NONE;
            sprite_t *tile = tile_sprites[tile_id];
            if (!tile)
                continue;

            int screen_y = world_y - cam_y + (spr_h - tile->height);

            // Culling should use sprite bounds (not logical bounds)
            if (screen_x < -tile->width || screen_x > screen_w ||
                screen_y < -tile->height || screen_y > screen_h)
                continue;

            rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
            rdpq_sprite_blit(tile, screen_x, screen_y, NULL);
        }
    }
}


// game_init: Load sprites and initialize game state.
void game_init(void)
{
    rdpq_font_t *font_debug = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
    rdpq_text_register_font(1, font_debug);

    base_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_base.sprite");
    wall_bottom_left = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_wall_bottom_left.sprite");
    wall_bottom_right = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_wall_bottom_right.sprite");
    wall_top_left = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_wall_top_left.sprite");
    wall_top_right = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_wall_top_right.sprite");

    tile_sprites[TILE_NONE] = NULL;
    tile_sprites[TILE_BASE] = base_tile;
    tile_sprites[TILE_WALL_BOTTOM_LEFT] = wall_bottom_left;
    tile_sprites[TILE_WALL_BOTTOM_RIGHT] = wall_bottom_right;
    tile_sprites[TILE_WALL_TOP_LEFT] = wall_top_left;
    tile_sprites[TILE_WALL_TOP_RIGHT] = wall_top_right;

    spr_w = base_tile->width;
    spr_h = base_tile->height;

    // Depth below the diamond face (used for camera centering).
    tile_draw_off_y = spr_h - ISO_H;

    step_x = ISO_W;
    step_y = ISO_H / 2;

    int half_w = step_x / 2;
    int half_h = step_y;

    map_origin_x = (MAP_HEIGHT - 1) * half_w;
    map_origin_y = 0;

    // Map extents in projected screen space (logical footprint)
    map_pixel_width = (MAP_WIDTH + MAP_HEIGHT - 2) * half_w + step_x;
    map_pixel_height = (MAP_WIDTH + MAP_HEIGHT - 2) * half_h + ISO_H;

    int center_x = MAP_WIDTH / 2;
    int center_y = MAP_HEIGHT / 2;
    int base_x = map_origin_x + (center_x - center_y) * half_w + half_w;
    int base_y = map_origin_y + (center_x + center_y) * half_h + half_h;

    character_init(base_x, base_y, half_w, half_h, ISO_H / 2);
}

// game_update: Update movement, facing, and camera.
void game_update(control_t keys)
{
    const int screen_w = display_get_width();
    const int screen_h = display_get_height();

    character_update(keys, is_blocked_position);

    cam_x = (map_pixel_width - screen_w) / 2;
    cam_y = ((tile_draw_off_y + map_pixel_height) - screen_h) / 2;
}

// game_draw: Render the scene and debug overlay.
void game_draw(display_context_t disp)
{
    rdpq_attach_clear(disp, NULL);
    rdpq_draw_filled_fullscreen(tile_bg);
    draw_map(&game_map);
    character_draw(cam_x, cam_y);

    int screen_w = display_get_width();
    int screen_h = display_get_height();
    int player_x = 0;
    int player_y = 0;
    int active_player = character_get_active_player();

    character_get_position(active_player, &player_x, &player_y);

    int tile_x = -1;
    int tile_y = -1;

    if (!world_to_grid(player_x, player_y, &tile_x, &tile_y))
    {
        tile_x = -1;
        tile_y = -1;
    }

    int p1_x = 0;
    int p1_y = 0;
    int p2_x = 0;
    int p2_y = 0;

    character_get_position(0, &p1_x, &p1_y);
    character_get_position(1, &p2_x, &p2_y);

    rdpq_text_printf(NULL, 1, 10, 10, "SCREEN W: %d, SCREEN H: %d", screen_w, screen_h);
    rdpq_text_printf(NULL, 1, 10, 30, "CAM X: %d, CAM Y: %d", cam_x, cam_y);
    rdpq_text_printf(NULL, 1, 10, 50, "PLAYER %d X: %d, Y: %d", active_player + 1, player_x, player_y);
    rdpq_text_printf(NULL, 1, 10, 70, "P1 X: %d, Y: %d", p1_x, p1_y);
    rdpq_text_printf(NULL, 1, 10, 90, "P2 X: %d, Y: %d", p2_x, p2_y);
    rdpq_text_printf(NULL, 1, 10, 110, "TILE X: %d, TILE Y: %d", tile_x, tile_y);

    rdpq_detach();
}
