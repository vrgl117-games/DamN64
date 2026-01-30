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
#include "fps.h"
// Logical diamond footprint in screen space (not necessarily sprite size)
#define ISO_W 32
#define ISO_H 16

static sprite_t *base_tile = NULL;
static sprite_t *building_sprite = NULL;
static sprite_t *water_tile = NULL;
static sprite_t *wall_tile = NULL;
static sprite_t *broken_wall_tile = NULL;
static map_render_t map_render = {0};

// Split-screen configuration
#define SPLIT_HYSTERESIS 0 // Buffer to prevent flickering
#define DIVIDER_WIDTH 2    // Width of the divider line

static bool split_screen_active = false;

// Debug overlay toggle (L button)
static bool debug_enabled = true;

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
#define MAX_BUILDINGS 16

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

/**
 * @brief in_bounds: Check if a tile coordinate is inside the map.
 */
static bool in_bounds(int x, int y)
{
    return (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT);
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
static bool is_blocked_position(int world_x, int world_y)
{
    int grid_x = 0;
    int grid_y = 0;

    // Out of map bounds = blocked
    if (!world_to_grid(world_x, world_y, &grid_x, &grid_y))
        return true;

    // TILE_NONE = blocked (void/water)
    int tile = game_map.tiles[grid_y][grid_x];
    if (tile == TILE_NONE || tile == TILE_WATER)
        return true;

    // Vehicle position with offset
    int vx = world_x;
    int vy = world_y + vehicle_offset_y;

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

        int combined_hw1 = vehicle_half_w + b->box1_half_w;
        int combined_hh1 = vehicle_half_h + b->box1_half_h;

        if (dx1 < combined_hw1 && dy1 < combined_hh1)
            return true;

        // Check box 2 (right side of building)
        int dx2 = vx - (b->world_x + b->box2_off_x);
        int dy2 = vy - (b->world_y + b->box2_off_y);
        if (dx2 < 0)
            dx2 = -dx2;
        if (dy2 < 0)
            dy2 = -dy2;

        int combined_hw2 = vehicle_half_w + b->box2_half_w;
        int combined_hh2 = vehicle_half_h + b->box2_half_h;

        if (dx2 < combined_hw2 && dy2 < combined_hh2)
            return true;
    }

    return false;
}

/**
 * @brief draw_debug_rect: Draw a rectangle outline for debug visualization.
 */
static void draw_debug_rect(int x1, int y1, int x2, int y2, color_t color)
{
    rdpq_set_mode_fill(color);
    // Top edge
    rdpq_fill_rectangle(x1, y1, x2, y1 + 1);
    // Bottom edge
    rdpq_fill_rectangle(x1, y2 - 1, x2, y2);
    // Left edge
    rdpq_fill_rectangle(x1, y1, x1 + 1, y2);
    // Right edge
    rdpq_fill_rectangle(x2 - 1, y1, x2, y2);
}

/**
 * @brief draw_debug_collision: Draw collision boxes for buildings and vehicle.
 */
static void draw_debug_collision(int current_cam_x)
{
    // Draw building collision boxes (red) - 2 boxes per building
    color_t building_color = RGBA32(255, 0, 0, 255);
    for (int i = 0; i < building_count; i++)
    {
        building_bbox_t *b = &building_boxes[i];

        int base_screen_x = b->world_x - current_cam_x;
        int base_screen_y = b->world_y - cam_y;

        // Box 1 (left side)
        int screen_x1 = base_screen_x + b->box1_off_x;
        int screen_y1 = base_screen_y + b->box1_off_y;
        draw_debug_rect(screen_x1 - b->box1_half_w, screen_y1 - b->box1_half_h,
                        screen_x1 + b->box1_half_w, screen_y1 + b->box1_half_h, building_color);

        // Box 2 (right side)
        int screen_x2 = base_screen_x + b->box2_off_x;
        int screen_y2 = base_screen_y + b->box2_off_y;
        draw_debug_rect(screen_x2 - b->box2_half_w, screen_y2 - b->box2_half_h,
                        screen_x2 + b->box2_half_w, screen_y2 + b->box2_half_h, building_color);
    }

    // Draw active vehicle collision box (green)
    color_t vehicle_color = RGBA32(0, 255, 0, 255);
    int active = character_get_active_player();
    int vx, vy;
    character_get_position(active, &vx, &vy);

    int screen_x = vx - current_cam_x;
    int screen_y = vy - cam_y + vehicle_offset_y; // Apply offset to move box lower

    int x1 = screen_x - vehicle_half_w;
    int y1 = screen_y - vehicle_half_h;
    int x2 = screen_x + vehicle_half_w;
    int y2 = screen_y + vehicle_half_h;

    draw_debug_rect(x1, y1, x2, y2, vehicle_color);
}

/**
 * @brief game_draw_title_background: Draw a single tiled band.
 */
void game_draw_title_background(int screen_w, int screen_h)
{
    rdpq_clear(background);

    if (!base_tile)
        return;

    int half_w = ISO_W / 2;
    int half_h = ISO_H / 2;

    int origin_x = (screen_w / 2) - half_w;
    int origin_y = -base_tile->height * 2;

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

            if (screen_x < -base_tile->width || screen_x > screen_w)
                continue;

            rdpq_sprite_blit(base_tile, screen_x, screen_y, NULL);
        }
    }
}

/**
 * @brief game_init: Load sprites and initialize game state.
 */
void game_init(void)
{
    base_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_base.sprite");
    building_sprite = sprite_load("rom:/gfx/sprites/isometric-city/cityBuilding_right_red_two.sprite");
    water_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_water.sprite");
    wall_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_wall.sprite");
    broken_wall_tile = sprite_load("rom:/gfx/sprites/isometric-city/cityTiles_broken_wall.sprite");

    map_render.tile_sprites[TILE_NONE] = NULL;
    map_render.tile_sprites[TILE_BASE] = base_tile;
    map_render.tile_sprites[TILE_BUILDING] = building_sprite;
    map_render.tile_sprites[TILE_WATER] = water_tile;
    map_render.tile_sprites[TILE_WALL] = wall_tile;
    map_render.tile_sprites[TILE_BROKEN_WALL] = broken_wall_tile;

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
    map_render.building_sprite = building_sprite;

    character_init(base_x, base_y, half_w, half_h, cam_y);

    // Build collision boxes for all buildings (T-shape: 2 boxes per building)
    building_count = 0;
    for (int y = 0; y < MAP_HEIGHT && building_count < MAX_BUILDINGS; y++)
    {
        for (int x = 0; x < MAP_WIDTH && building_count < MAX_BUILDINGS; x++)
        {
            if (game_map.tiles[y][x] == TILE_BUILDING)
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
                building_boxes[building_count].box2_off_y = 6;
                building_boxes[building_count].box2_half_w = 6;
                building_boxes[building_count].box2_half_h = 2;

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

    // Toggle debug overlay with L button
    if (p1->L || (p2 && p2->L))
        debug_enabled = !debug_enabled;

    character_update(player_keys, is_blocked_position);

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
 * @brief game_draw: Render the scene and debug overlay.
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

        // Debug info for single mode
        if (debug_enabled)
        {
            // Draw collision debug boxes
            draw_debug_collision(cam_x);
        }
    }

    fps_draw();
    rdpq_detach_show();
}
