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
#include "dam.h"
#include "pause.h"
#include "bgm.h"
#ifndef NDEBUG
#include "fps.h"
#endif
#include "font.h"
// Logical diamond footprint in screen space (not necessarily sprite size)
#define ISO_W 32
#define ISO_H 16
#define SPAWN_P1_GRID_X 9
#define SPAWN_P1_GRID_Y 24
#define SPAWN_P2_GRID_X 20
#define SPAWN_P2_GRID_Y 5

static sprite_t *base_tile = NULL;
static sprite_t *building_right_red_two = NULL;
static sprite_t *beton_sprite = NULL;
static sprite_t *beton_red_sprite = NULL;
static sprite_t *beton_yellow_sprite = NULL;
static sprite_t *building_right_brown_two = NULL;
static sprite_t *building_left_red_three = NULL;
static sprite_t *building_left_white_one = NULL;
static sprite_t *building_right_yellow_four = NULL;
static sprite_t *building_left_white_six = NULL;
static sprite_t *building_left_brown_three = NULL;
static sprite_t *water_tile = NULL;
static sprite_t *waves_tile = NULL;
static sprite_t *waves_boat_tile = NULL;
static sprite_t *wall_tile = NULL;
static sprite_t *broken_wall_tile = NULL;
static sprite_t *broken_wall_water_tile = NULL;
static sprite_t *wall_water_tile = NULL;
static sprite_t *road_left_tile = NULL;
static sprite_t *road_right_tile = NULL;
static sprite_t *road_t_tile = NULL;
static sprite_t *road_d_left_tile = NULL;
static sprite_t *road_d_right_tile = NULL;
static sprite_t *road_corner_1_tile = NULL;
static sprite_t *road_corner_2_tile = NULL;
static sprite_t *road_corner_3_tile = NULL;
static sprite_t *road_corner_4_tile = NULL;
static sprite_t *tree_tile = NULL;
static sprite_t *fountain_tile = NULL;
static map_render_t map_render = {0};

// Split-screen configuration
#define SPLIT_HYSTERESIS 0 // Buffer to prevent flickering
#define DIVIDER_WIDTH 2    // Width of the divider line

static bool split_screen_active = false;

// Which player is on the left half in split mode (0 = P1, 1 = P2)
static int left_player = 0;

// Cameras X for each half (used in split mode)
static int cam_x_left = 0;
static int cam_x_right = 0;

// Shared camera X (used in single mode)
static int cam_x = 0;

// Fixed vertical camera offset (centers map on screen, computed once in game_init)
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

// blue background
const color_t background = RGBA32(35, 54, 66, 255);

// Maximum number of buildings we track for collision
#define MAX_BUILDINGS 64

// Building collision uses 2 boxes in a T-shape to match isometric base
typedef struct
{
    int world_x; // Center X in world coords
    int world_y; // Center Y in world coords
    int grid_x;  // Grid X position
    int grid_y;  // Grid Y position
    // Box 1: Left side of building (going down-left in iso)
    int box1_off_x;
    int box1_off_y;
    int box1_half_w;
    int box1_half_h;
    // Box 2: Right side of building (going down-right in iso)
    int box2_off_x;
    int box2_off_y;
    int box2_half_w;
    int box2_half_h;
} building_bbox_t;

static building_bbox_t building_boxes[MAX_BUILDINGS];
static int building_count = 0;

// Vehicle collision box dimensions (world space)
static int vehicle_half_w = 7;   // Width
static int vehicle_half_h = 6;   // Height
static int vehicle_offset_y = 2; // Offset to move box lower

#define PLANT_REFILL_SECONDS 8

static bool plant_ready_y = true;
static bool plant_ready_r = true;
static int plant_y_x = -1;
static int plant_y_y = -1;
static int plant_r_x = -1;
static int plant_r_y = -1;
static uint32_t plant_y_empty_tick = 0;
static uint32_t plant_r_empty_tick = 0;
static int rumble_ticks[2] = {0, 0};

static bool world_to_grid(int world_x, int world_y, int *grid_x, int *grid_y);
static void update_plant_tiles(void);
static void update_music_track(void);

/**
 * @brief update_truck_full: Set full state when driving below beton.
 */
static void update_truck_full(void)
{
    for (int i = 0; i < 2; i++)
    {
        int wx = 0;
        int wy = 0;
        int gx = 0;
        int gy = 0;

        character_get_position(i, &wx, &wy);
        if (!world_to_grid(wx, wy, &gx, &gy))
            continue;
        if (gy <= 0)
            continue;
        int tile_above = game_map.tiles[gy - 1][gx];

        if (i == 0 &&
            !character_is_full(0) &&
            plant_ready_y &&
            tile_above == TILE_BETON_YELLOW)
        {
            character_set_full(0, true);
            rumble_ticks[0] = 3;
            plant_ready_y = false;
            plant_y_empty_tick = get_ticks();
            update_plant_tiles();
        }
        else if (i == 1 &&
                 !character_is_full(1) &&
                 plant_ready_r &&
                 tile_above == TILE_BETON_RED)
        {
            character_set_full(1, true);
            rumble_ticks[1] = 3;
            plant_ready_r = false;
            plant_r_empty_tick = get_ticks();
            update_plant_tiles();
        }
    }
}

/**
 * @brief update_truck_repair: Repair broken walls when trucks are full.
 */
static void update_truck_repair(void)
{
    for (int i = 0; i < 2; i++)
    {
        if (!character_is_full(i))
            continue;

        int wx = 0;
        int wy = 0;
        int gx = 0;
        int gy = 0;

        character_get_position(i, &wx, &wy);
        if (!world_to_grid(wx, wy, &gx, &gy))
            continue;

        int tile = game_map.tiles[gy][gx];
        if (tile == TILE_BROKEN_WALL || tile == TILE_BROKEN_WALL_WATER)
        {
            dam_repair_wall(gx, gy);
            character_set_full(i, false);
        }
    }
}

/**
 * @brief in_bounds: Check if a tile coordinate is inside the map.
 */
static bool in_bounds(int x, int y)
{
    return (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT);
}

/**
 * @brief update_plant_tiles: Update plant tiles based on readiness.
 */
static void update_plant_tiles(void)
{
    uint32_t now = get_ticks();

    if (!plant_ready_y && plant_y_empty_tick > 0 &&
        now - plant_y_empty_tick >= PLANT_REFILL_SECONDS * TICKS_PER_SECOND)
    {
        plant_ready_y = true;
        plant_y_empty_tick = 0;
    }

    if (!plant_ready_r && plant_r_empty_tick > 0 &&
        now - plant_r_empty_tick >= PLANT_REFILL_SECONDS * TICKS_PER_SECOND)
    {
        plant_ready_r = true;
        plant_r_empty_tick = 0;
    }

    if (plant_y_x >= 0 && plant_y_y >= 0)
    {
        game_map.tiles[plant_y_y][plant_y_x] = plant_ready_y ? TILE_BETON_YELLOW : TILE_BETON;
    }

    if (plant_r_x >= 0 && plant_r_y >= 0)
    {
        game_map.tiles[plant_r_y][plant_r_x] = plant_ready_r ? TILE_BETON_RED : TILE_BETON;
    }
}

/**
 * @brief update_music_track: Switch game music based on dam state.
 */
static void update_music_track(void)
{
    if (!pause_get_music_on())
    {
        bgm_mute();
        return;
    }

    int broken = dam_get_broken_sections();
    if (broken >= 2)
        bgm_set_track(BGM_TRACK_WEIRD2);
    else if (broken >= 1)
        bgm_set_track(BGM_TRACK_WEIRD);
    else
        bgm_set_track(BGM_TRACK_JOY);

    bgm_play();
}

/**
 * @brief world_to_grid: Convert world coordinates to grid tile indices.
 */
static bool world_to_grid(int world_x, int world_y, int *grid_x, int *grid_y)
{
    int half_w = step_x / 2;         // 16
    int half_h = step_y;             // 8
    int denom = 2 * half_w * half_h; // 256

    // Offset to tile center
    int local_x = world_x - map_origin_x - half_w;
    int local_y = world_y - map_origin_y - half_h;

    // Inverse isometric projection with rounding
    int gx_num = local_x * half_h + local_y * half_w;
    int gy_num = local_y * half_w - local_x * half_h;

    int gx = (gx_num >= 0) ? (gx_num + denom / 2) / denom : (gx_num - denom / 2) / denom;
    int gy = (gy_num >= 0) ? (gy_num + denom / 2) / denom : (gy_num - denom / 2) / denom;

    *grid_x = gx;
    *grid_y = gy;

    return in_bounds(gx, gy);
}

/**
 * @brief is_blocked_position: Return true if a world position is blocked.
 * Uses bounding box collision against buildings.
 */
static bool is_blocked_position(int world_x, int world_y, int index)
{
    int grid_x = 0;
    int grid_y = 0;

    // Out of map bounds = blocked
    if (!world_to_grid(world_x, world_y, &grid_x, &grid_y))
        return true;

    // TILE_NONE = blocked (void/water/waves)
    int tile = game_map.tiles[grid_y][grid_x];
    if (tile == TILE_NONE || tile == TILE_WAVES || tile == TILE_WAVES_BOAT)
        return true;

    // Vehicle position with offset
    int half_w = vehicle_half_w;
    int half_h = vehicle_half_h;
    int offset_y = vehicle_offset_y;
    character_get_collision_box(index, &half_w, &half_h, &offset_y);

    int vx = world_x;
    int vy = world_y + offset_y;

    // Check collision against all building bounding boxes (2 boxes per building)
    for (int i = 0; i < building_count; i++)
    {
        building_bbox_t *b = &building_boxes[i];

        // Check box 1 (left side of building)
        int dx1 = vx - (b->world_x + b->box1_off_x);
        int dy1 = vy - (b->world_y + b->box1_off_y);
        if (dx1 < 0)
            dx1 = -dx1;
        if (dy1 < 0)
            dy1 = -dy1;

        int combined_hw1 = half_w + b->box1_half_w;
        int combined_hh1 = half_h + b->box1_half_h;

        if (dx1 < combined_hw1 && dy1 < combined_hh1)
            return true;

        // Check box 2 (right side of building)
        int dx2 = vx - (b->world_x + b->box2_off_x);
        int dy2 = vy - (b->world_y + b->box2_off_y);
        if (dx2 < 0)
            dx2 = -dx2;
        if (dy2 < 0)
            dy2 = -dy2;

        int combined_hw2 = half_w + b->box2_half_w;
        int combined_hh2 = half_h + b->box2_half_h;

        if (dx2 < combined_hw2 && dy2 < combined_hh2)
            return true;
    }

    return false;
}

/**
 * @brief game_draw_title_band: Draw a single tiled band using a tile sprite.
 */
void game_draw_title_band(sprite_t *tile, int screen_w, int screen_h)
{
    rdpq_clear(background);

    if (!tile)
        return;

    int half_w = ISO_W / 2;
    int half_h = ISO_H / 2;

    int origin_x = (screen_w / 2) - half_w;
    int origin_y = -tile->height * 2;

    int max_tiles = (screen_w / half_w) + 2;

    // Convert screen center to iso sum
    int center_sum = ((screen_h / 2) - origin_y) / half_h;
    int band_half = 2; // 5 tiles high

    rdpq_set_mode_copy(true);

    for (int s = center_sum - band_half; s <= center_sum + band_half; s++)
    {
        // s == x + y
        for (int x = -max_tiles; x <= max_tiles; x++)
        {
            int y = s - x;

            int screen_x = origin_x + (x - y) * half_w;
            int screen_y = origin_y + s * half_h;

            if (screen_x < -tile->width || screen_x > screen_w)
                continue;

            rdpq_sprite_blit(tile, screen_x, screen_y, NULL);
        }
    }
}

/**
 * @brief game_draw_title_background: Draw a single tiled band.
 */
void game_draw_title_background(int screen_w, int screen_h)
{
    game_draw_title_band(base_tile, screen_w, screen_h);
}

// game_get_tile_sprite: Return tile sprite by tile id.
sprite_t *game_get_tile_sprite(tile_id_t tile_id)
{
    if (tile_id < 0 || tile_id >= TILE_COUNT)
        return NULL;
    return map_render.tile_sprites[tile_id];
}

/**
 * @brief game_init: Load sprites and initialize game state.
 */
void game_init(void)
{
    map_reset();
    base_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_base.sprite");
    building_right_red_two = sprite_load("rom:/gfx/sprites/isometric-city/cityBuilding_right_red_two.sprite");
    beton_sprite = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_beton.sprite");
    beton_red_sprite = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_beton_red.sprite");
    beton_yellow_sprite = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_beton_yellow.sprite");
    building_right_brown_two = sprite_load("rom:/gfx/sprites/isometric-city/cityBuilding_right_brown_two.sprite");
    building_left_red_three = sprite_load("rom:/gfx/sprites/isometric-city/cityBuilding_left_red_three.sprite");
    building_left_white_one = sprite_load("rom:/gfx/sprites/isometric-city/cityBuilding_left_white_one.sprite");
    building_right_yellow_four = sprite_load("rom:/gfx/sprites/isometric-city/cityBuilding_right_yellow_four.sprite");
    building_left_white_six = sprite_load("rom:/gfx/sprites/isometric-city/cityBuilding_left_white_six.sprite");
    building_left_brown_three = sprite_load("rom:/gfx/sprites/isometric-city/cityBuilding_left_brown_three.sprite");
    water_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_water.sprite");
    waves_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_water_waves.sprite");
    waves_boat_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_water_waves_boat.sprite");
    wall_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_wall.sprite");
    broken_wall_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_broken_wall.sprite");
    broken_wall_water_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_broken_wall_water.sprite");
    wall_water_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_wall_water.sprite");
    road_left_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_road_left.sprite");
    road_right_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_road_right.sprite");
    road_t_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_road_t.sprite");
    road_d_left_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_road_d_left.sprite");
    road_d_right_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_road_d_right.sprite");
    road_corner_1_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_road_corner_1.sprite");
    road_corner_2_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_road_corner_2.sprite");
    road_corner_3_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_road_corner_3.sprite");
    road_corner_4_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_road_corner_4.sprite");
    tree_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_tree.sprite");
    fountain_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_fountain.sprite");

    map_render.tile_sprites[TILE_NONE] = NULL;
    map_render.tile_sprites[TILE_BASE] = base_tile;
    map_render.tile_sprites[TILE_BUILDING_RIGHT_RED_TWO] = building_right_red_two;
    map_render.tile_sprites[TILE_WATER] = water_tile;
    map_render.tile_sprites[TILE_WALL] = wall_tile;
    map_render.tile_sprites[TILE_BROKEN_WALL] = broken_wall_tile;
    map_render.tile_sprites[TILE_BROKEN_WALL_WATER] = broken_wall_water_tile;
    map_render.tile_sprites[TILE_WALL_WATER] = wall_water_tile;
    map_render.tile_sprites[TILE_WAVES] = waves_tile;
    map_render.tile_sprites[TILE_WAVES_BOAT] = waves_boat_tile;
    map_render.tile_sprites[TILE_BETON] = beton_sprite;
    map_render.tile_sprites[TILE_ROAD_LEFT] = road_left_tile;
    map_render.tile_sprites[TILE_ROAD_RIGHT] = road_right_tile;
    map_render.tile_sprites[TILE_ROAD_T] = road_t_tile;
    map_render.tile_sprites[TILE_ROAD_D_LEFT] = road_d_left_tile;
    map_render.tile_sprites[TILE_ROAD_D_RIGHT] = road_d_right_tile;
    map_render.tile_sprites[TILE_ROAD_CORNER_1] = road_corner_1_tile;
    map_render.tile_sprites[TILE_ROAD_CORNER_2] = road_corner_2_tile;
    map_render.tile_sprites[TILE_ROAD_CORNER_3] = road_corner_3_tile;
    map_render.tile_sprites[TILE_ROAD_CORNER_4] = road_corner_4_tile;
    map_render.tile_sprites[TILE_TREE] = tree_tile;
    map_render.tile_sprites[TILE_FOUNTAIN] = fountain_tile;
    map_render.tile_sprites[TILE_BETON_RED] = beton_red_sprite;
    map_render.tile_sprites[TILE_BETON_YELLOW] = beton_yellow_sprite;
    map_render.tile_sprites[TILE_BUILDING_RIGHT_BROWN_TWO] = building_right_brown_two;
    map_render.tile_sprites[TILE_BUILDING_LEFT_RED_THREE] = building_left_red_three;
    map_render.tile_sprites[TILE_BUILDING_LEFT_WHITE_ONE] = building_left_white_one;
    map_render.tile_sprites[TILE_BUILDING_RIGHT_YELLOW_FOUR] = building_right_yellow_four;
    map_render.tile_sprites[TILE_BUILDING_LEFT_WHITE_SIX] = building_left_white_six;
    map_render.tile_sprites[TILE_BUILDING_LEFT_BROWN_THREE] = building_left_brown_three;

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

    int base_x = map_origin_x + (SPAWN_P1_GRID_X - SPAWN_P1_GRID_Y) * half_w + half_w;
    int base_y = map_origin_y + (SPAWN_P1_GRID_X + SPAWN_P1_GRID_Y) * half_h + half_h;
    int p2_x = map_origin_x + (SPAWN_P2_GRID_X - SPAWN_P2_GRID_Y) * half_w + half_w;
    int p2_y = map_origin_y + (SPAWN_P2_GRID_X + SPAWN_P2_GRID_Y) * half_h + half_h;

    // Compute fixed vertical camera offset to center map on screen
    int screen_h = display_get_height();
    cam_y = ((tile_draw_off_y + map_pixel_height) - screen_h) / 2;

    map_render.origin_x = map_origin_x;
    map_render.origin_y = map_origin_y;
    map_render.step_x = step_x;
    map_render.step_y = step_y;
    map_render.spr_h = spr_h;
    map_render.cam_y = cam_y;
    map_render.base_tile = base_tile;
    map_render.building_right_red_two = building_right_red_two;
    map_render.waves_boat_tile = waves_boat_tile;
    map_render.beton_sprite = beton_sprite;
    map_render.beton_red_sprite = beton_red_sprite;
    map_render.beton_yellow_sprite = beton_yellow_sprite;
    map_render.building_right_brown_two = building_right_brown_two;
    map_render.building_left_red_three = building_left_red_three;
    map_render.building_left_white_one = building_left_white_one;
    map_render.building_right_yellow_four = building_right_yellow_four;
    map_render.building_left_white_six = building_left_white_six;
    map_render.building_left_brown_three = building_left_brown_three;

    dam_init();
    character_init(base_x, base_y, p2_x - base_x, p2_y - base_y, cam_y);

    plant_y_x = -1;
    plant_y_y = -1;
    plant_r_x = -1;
    plant_r_y = -1;

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            if (game_map.tiles[y][x] == TILE_BETON_YELLOW)
            {
                plant_y_x = x;
                plant_y_y = y;
            }
            else if (game_map.tiles[y][x] == TILE_BETON_RED)
            {
                plant_r_x = x;
                plant_r_y = y;
            }
        }
    }

    plant_ready_y = true;
    plant_ready_r = true;
    plant_y_empty_tick = 0;
    plant_r_empty_tick = 0;
    update_plant_tiles();

    // Build collision boxes for all buildings (T-shape: 2 boxes per building)
    building_count = 0;
    for (int y = 0; y < MAP_HEIGHT && building_count < MAX_BUILDINGS; y++)
    {
        for (int x = 0; x < MAP_WIDTH && building_count < MAX_BUILDINGS; x++)
        {
            if (game_map.tiles[y][x] == TILE_BUILDING_RIGHT_RED_TWO ||
                game_map.tiles[y][x] == TILE_BETON ||
                game_map.tiles[y][x] == TILE_BETON_RED ||
                game_map.tiles[y][x] == TILE_BETON_YELLOW ||
                game_map.tiles[y][x] == TILE_BUILDING_RIGHT_BROWN_TWO ||
                game_map.tiles[y][x] == TILE_BUILDING_LEFT_RED_THREE ||
                game_map.tiles[y][x] == TILE_BUILDING_LEFT_WHITE_ONE ||
                game_map.tiles[y][x] == TILE_BUILDING_RIGHT_YELLOW_FOUR ||
                game_map.tiles[y][x] == TILE_BUILDING_LEFT_WHITE_SIX ||
                game_map.tiles[y][x] == TILE_BUILDING_LEFT_BROWN_THREE ||
                game_map.tiles[y][x] == TILE_TREE ||
                game_map.tiles[y][x] == TILE_FOUNTAIN)
            {
                // Compute world center of this building tile
                int world_x = map_origin_x + (x - y) * half_w + half_w;
                int world_y = map_origin_y + (x + y) * half_h + half_h;

                building_boxes[building_count].world_x = world_x;
                building_boxes[building_count].world_y = world_y;
                building_boxes[building_count].grid_x = x;
                building_boxes[building_count].grid_y = y;

                // Box 1: Top bar of the T (wide, spans building width)
                building_boxes[building_count].box1_off_x = 0;
                building_boxes[building_count].box1_off_y = 1;
                building_boxes[building_count].box1_half_w = 12;
                building_boxes[building_count].box1_half_h = 2;

                // Box 2: Bottom stem of the T (narrow, shorter)
                building_boxes[building_count].box2_off_x = 0;
                building_boxes[building_count].box2_off_y = 5;
                building_boxes[building_count].box2_half_w = 6;
                building_boxes[building_count].box2_half_h = 2;

                if (game_map.tiles[y][x] == TILE_TREE)
                {
                    building_boxes[building_count].box1_half_w = 10;
                    building_boxes[building_count].box1_half_h = 3;
                    building_boxes[building_count].box2_off_y = -8;
                    building_boxes[building_count].box2_half_w = 5;
                }
                else if (game_map.tiles[y][x] == TILE_FOUNTAIN)
                {
                    building_boxes[building_count].box1_half_w = 12;
                    building_boxes[building_count].box1_half_h = 3;
                    building_boxes[building_count].box2_off_y = -6;
                    building_boxes[building_count].box2_half_w = 6;
                }

                building_count++;
            }
        }
    }
}

/**
 * @brief clamp_camera_x: Clamp horizontal camera position to map bounds.
 */
static int clamp_camera_x(int cam_x, int view_width)
{
    if (cam_x < 0)
        cam_x = 0;
    if (cam_x > map_pixel_width - view_width)
        cam_x = map_pixel_width - view_width;
    return cam_x;
}

/**
 * @brief game_update: Update movement, facing, and camera.
 */
void game_update(control_t *keys[2])
{
    const int screen_w = display_get_width();
    const control_t *p1 = keys[0];
    const control_t *p2 = keys[1];
    const control_t *player_keys[2] = {p1, p2};

    dam_update();
    update_music_track();
    character_update(player_keys, is_blocked_position);
    update_truck_full();
    update_truck_repair();
    if (dam_is_spreading())
    {
        if (keys[0] && keys[0]->rumble && pause_get_rumble_on())
            rumble_ticks[0] = 1;
        if (keys[1] && keys[1]->rumble && pause_get_rumble_on())
            rumble_ticks[1] = 1;
    }
    if (keys[0] && keys[0]->rumble && pause_get_rumble_on())
    {
        joypad_set_rumble_active(JOYPAD_PORT_1, rumble_ticks[0] > 0);
    }
    if (keys[1] && keys[1]->rumble && pause_get_rumble_on())
    {
        joypad_set_rumble_active(JOYPAD_PORT_2, rumble_ticks[1] > 0);
    }
    if (rumble_ticks[0] > 0)
        rumble_ticks[0]--;
    if (rumble_ticks[1] > 0)
        rumble_ticks[1]--;
    update_plant_tiles();

    // Get both player positions
    int p1_x = 0, p1_y = 0;
    int p2_x = 0, p2_y = 0;
    character_get_position(0, &p1_x, &p1_y);
    character_get_position(1, &p2_x, &p2_y);

    // Calculate horizontal distance between players
    int dist_x = p1_x > p2_x ? p1_x - p2_x : p2_x - p1_x;

    // Split threshold: when a player would be past the center of their half-screen
    // In single mode, each player is dist_x/2 from camera center
    // Split when dist_x/2 > screen_w/4, i.e. dist_x > screen_w/2
    int split_threshold = screen_w / 2;
    int merge_threshold = split_threshold - SPLIT_HYSTERESIS;

    // Determine split-screen mode with hysteresis
    if (split_screen_active)
    {
        // Exit split mode when players can fit on screen together
        if (dist_x < merge_threshold)
            split_screen_active = false;
    }
    else
    {
        // Enter split mode when players are too far apart to fit
        if (dist_x > split_threshold)
            split_screen_active = true;
    }

    if (split_screen_active)
    {
        // Split mode: each player has their own camera (half screen width)
        int half_w = screen_w / 2;

        // Determine which player is on the left based on world position
        int left_x, right_x;
        if (p1_x <= p2_x)
        {
            left_player = 0;
            left_x = p1_x;
            right_x = p2_x;
        }
        else
        {
            left_player = 1;
            left_x = p2_x;
            right_x = p1_x;
        }

        // Left half camera - center left player
        cam_x_left = left_x - half_w / 2;
        cam_x_left = clamp_camera_x(cam_x_left, half_w);

        // Right half camera - center right player
        cam_x_right = right_x - half_w / 2;
        cam_x_right = clamp_camera_x(cam_x_right, half_w);
    }
    else
    {
        // Single mode: camera centered between both players
        int center_x = (p1_x + p2_x) / 2;
        cam_x = center_x - screen_w / 2;
        cam_x = clamp_camera_x(cam_x, screen_w);
    }
}

/**
 * @brief game_draw: Render the scene.
 */
void game_draw(display_context_t disp)
{
    int screen_w = display_get_width();
    int screen_h = display_get_height();
    int right_view_x = 0;
    int left_view_w = 0;
    int right_view_w = 0;
    int divider_x = 0;
    int half_w = 0;

    if (split_screen_active)
    {
        half_w = screen_w / 2;
        int align = ISO_W / 2;
        right_view_x = (half_w / align) * align;
        if (right_view_x < DIVIDER_WIDTH)
            right_view_x = DIVIDER_WIDTH;
        left_view_w = right_view_x - DIVIDER_WIDTH;
        right_view_w = screen_w - right_view_x;
        divider_x = left_view_w;
    }

    if (split_screen_active)
    {
        rdpq_attach(disp, NULL);
        rdpq_clear(background);
        rdpq_detach();
        rdpq_set_mode_copy(true);

        // === Left half: player who is further left in world ===
        surface_t left_view = surface_make_sub(disp, 0, 0, left_view_w, screen_h);
        rdpq_attach(&left_view, NULL);
        map_draw(&game_map, &map_render, cam_x_left, left_view_w);
        character_draw(cam_x_left);
        map_draw_buildings(&game_map, &map_render, cam_x_left, left_view_w);
        rdpq_detach();

        // === Right half: player who is further right in world ===
        surface_t right_view = surface_make_sub(disp, right_view_x, 0, right_view_w, screen_h);
        rdpq_attach(&right_view, NULL);
        map_draw(&game_map, &map_render, cam_x_right, right_view_w);
        character_draw(cam_x_right);
        map_draw_buildings(&game_map, &map_render, cam_x_right, right_view_w);
        rdpq_detach();

        // === Draw divider line ===
        rdpq_attach(disp, NULL);
        rdpq_set_mode_fill(RGBA32(0, 0, 0, 255));
        rdpq_fill_rectangle(divider_x, 0, divider_x + DIVIDER_WIDTH, screen_h);
    }
    else
    {
        // === Single screen mode ===
        rdpq_attach(disp, NULL);
        rdpq_clear(background);
        rdpq_set_mode_copy(true);
        map_draw(&game_map, &map_render, cam_x, screen_w);
        character_draw(cam_x);
        map_draw_buildings(&game_map, &map_render, cam_x, screen_w);
    }

    dam_draw_breach_bar(screen_w);

#ifndef NDEBUG
    fps_draw();
#endif
    rdpq_detach_show();
}

// game_get_broken_sections: Return number of broken wall sections.
int game_get_broken_sections(void)
{
    return dam_get_broken_sections();
}
