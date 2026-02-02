/*
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "character.h"
#include "rdpq.h"

#define CHARACTER_COUNT 2
#define CAR_DIR_COUNT 8
#define CAR_DIR_N 0
#define CAR_DIR_NE 1
#define CAR_DIR_E 2
#define CAR_DIR_SE 3
#define CAR_DIR_S 4
#define CAR_DIR_SW 5
#define CAR_DIR_W 6
#define CAR_DIR_NW 7

static sprite_t *garbage_yellow_sprites[CAR_DIR_COUNT] = {0};
static sprite_t *garbage_red_sprites[CAR_DIR_COUNT] = {0};
static int character_x[CHARACTER_COUNT] = {0};
static int character_y[CHARACTER_COUNT] = {0};
static int active_player = 0;
static int char_facing[CHARACTER_COUNT] = {0};
static int stored_cam_y = 0;
static bool character_full[CHARACTER_COUNT] = {false, false};
static int move_scale = 1;
static uint32_t last_move_tick = 0;

#define COLLISION_UPDOWN_HALF_W 6
#define COLLISION_UPDOWN_HALF_H 8
#define COLLISION_UPDOWN_OFFSET_Y 1
#define COLLISION_SIDE_HALF_W 9
#define COLLISION_SIDE_HALF_H 8
#define COLLISION_SIDE_OFFSET_Y 1
#define COLLISION_DIAG_HALF_W 8
#define COLLISION_DIAG_HALF_H 8
#define COLLISION_DIAG_OFFSET_Y 2

/**
 * @brief character_load_vehicle_sprites: Load vehicle sprites.
 */
void character_load_vehicle_sprites(void)
{
    garbage_yellow_sprites[CAR_DIR_N] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_yellow_N.sprite");
    garbage_yellow_sprites[CAR_DIR_NE] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_yellow_NE.sprite");
    garbage_yellow_sprites[CAR_DIR_E] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_yellow_E.sprite");
    garbage_yellow_sprites[CAR_DIR_SE] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_yellow_SE.sprite");
    garbage_yellow_sprites[CAR_DIR_S] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_yellow_S.sprite");
    garbage_yellow_sprites[CAR_DIR_SW] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_yellow_SW.sprite");
    garbage_yellow_sprites[CAR_DIR_W] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_yellow_W.sprite");
    garbage_yellow_sprites[CAR_DIR_NW] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_yellow_NW.sprite");

    garbage_red_sprites[CAR_DIR_N] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_red_N.sprite");
    garbage_red_sprites[CAR_DIR_NE] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_red_NE.sprite");
    garbage_red_sprites[CAR_DIR_E] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_red_E.sprite");
    garbage_red_sprites[CAR_DIR_SE] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_red_SE.sprite");
    garbage_red_sprites[CAR_DIR_S] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_red_S.sprite");
    garbage_red_sprites[CAR_DIR_SW] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_red_SW.sprite");
    garbage_red_sprites[CAR_DIR_W] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_red_W.sprite");
    garbage_red_sprites[CAR_DIR_NW] = sprite_load("rom:/gfx/sprites/isometric-vehicles/garbage_red_NW.sprite");
}

/**
 * @brief character_init: Initialize character state.
 */
void character_init(int base_x, int base_y, int offset_x, int offset_y, int cam_y)
{
    active_player = 0;
    stored_cam_y = cam_y;

    for (int i = 0; i < CHARACTER_COUNT; i++)
    {
        character_x[i] = base_x + i * offset_x;
        character_y[i] = base_y + i * offset_y;
        char_facing[i] = (i == 0) ? CAR_DIR_NE : CAR_DIR_SW;
        character_full[i] = false;
    }

    last_move_tick = 0;
    move_scale = 1;
}

/**
 * @brief character_move_player: Update movement and facing for a character.
 */
static void character_move_player(int index, const control_t *keys, character_block_fn is_blocked)
{
    if (!keys)
        return;
    if (index < 0 || index >= CHARACTER_COUNT)
        return;

    int dir_x = 0;
    int dir_y = 0;

    if (keys->direction & d_left)
        dir_x -= 1;
    if (keys->direction & d_right)
        dir_x += 1;
    if (keys->direction & d_up)
        dir_y -= 1;
    if (keys->direction & d_down)
        dir_y += 1;

    int *player_x = &character_x[index];
    int *player_y = &character_y[index];
    int *player_facing = &char_facing[index];

    if (dir_x || dir_y)
    {
        int step_x = 2;
        int step_y = 2;
        if (!character_full[index])
        {
            step_x += 1;
            step_y += 1;
        }

        if (move_scale > 1)
        {
            step_x *= move_scale;
            step_y *= move_scale;
        }

        step_x = (step_x + 1) / 2;
        step_y = (step_y + 1) / 2;

        if (dir_x && dir_y)
        {
            step_y = (step_y + 1) / 2;
            if (step_y < 1)
            {
                step_y = 1;
            }
            step_x = step_y * 2;
        }

        int next_x = *player_x + dir_x * step_x;
        int next_y = *player_y + dir_y * step_y;

        // Check collision at center point
        // AABB collision with buildings is handled in is_blocked_position
        bool blocked = false;

        if (is_blocked)
        {
            blocked = is_blocked(next_x, next_y, index);
        }

        if (!blocked)
        {
            *player_x = next_x;
            *player_y = next_y;
        }
    }

    if (dir_x || dir_y)
    {
        if (dir_y < 0)
        {
            if (dir_x < 0)
                *player_facing = CAR_DIR_NW;
            else if (dir_x > 0)
                *player_facing = CAR_DIR_NE;
            else
                *player_facing = CAR_DIR_N;
        }
        else if (dir_y > 0)
        {
            if (dir_x < 0)
                *player_facing = CAR_DIR_SW;
            else if (dir_x > 0)
                *player_facing = CAR_DIR_SE;
            else
                *player_facing = CAR_DIR_S;
        }
        else
        {
            if (dir_x < 0)
                *player_facing = CAR_DIR_W;
            else if (dir_x > 0)
                *player_facing = CAR_DIR_E;
        }
    }
}

// character_get_collision_box: Get collision half-sizes and offset.
void character_get_collision_box(int index, int *half_w, int *half_h, int *offset_y)
{
    if (index < 0 || index >= CHARACTER_COUNT)
        return;

    int dir = char_facing[index];
    if (dir == CAR_DIR_N || dir == CAR_DIR_S)
    {
        if (half_w)
            *half_w = COLLISION_UPDOWN_HALF_W;
        if (half_h)
            *half_h = COLLISION_UPDOWN_HALF_H;
        if (offset_y)
            *offset_y = COLLISION_UPDOWN_OFFSET_Y;
        return;
    }

    if (dir == CAR_DIR_E || dir == CAR_DIR_W)
    {
        if (half_w)
            *half_w = COLLISION_SIDE_HALF_W;
        if (half_h)
            *half_h = COLLISION_SIDE_HALF_H;
        if (offset_y)
            *offset_y = COLLISION_SIDE_OFFSET_Y;
        return;
    }

    if (half_w)
        *half_w = COLLISION_DIAG_HALF_W;
    if (half_h)
        *half_h = COLLISION_DIAG_HALF_H;
    if (offset_y)
        *offset_y = COLLISION_DIAG_OFFSET_Y;
}

/**
 * @brief character_update: Update movement, facing, and active character(s).
 */
void character_update(const control_t *keys[2], character_block_fn is_blocked)
{
    const control_t *p1 = keys[0];
    const control_t *p2 = keys[1];
    bool has_p2 = (p2 != NULL);

    uint32_t now = get_ticks();
    if (last_move_tick == 0)
        last_move_tick = now;
    uint32_t dt = now - last_move_tick;
    last_move_tick = now;
    int scale = (int)((dt * 60) / TICKS_PER_SECOND);
    if (scale < 1)
        scale = 1;
    if (scale > 4)
        scale = 4;
    move_scale = scale;

    if (has_p2)
    {
        active_player = 0;
        character_move_player(0, p1, is_blocked);
        character_move_player(1, p2, is_blocked);
        return;
    }

    const control_t *single = p1;

    if (single->z)
        active_player = (active_player + 1) % CHARACTER_COUNT;

    character_move_player(active_player, single, is_blocked);
}

/**
 * @brief character_draw_single: Render a single vehicle by index.
 */
void character_draw_single(int index, int cam_x)
{
    if (index < 0 || index >= CHARACTER_COUNT)
        return;

    int dir = char_facing[index];
    if (dir < 0 || dir >= CAR_DIR_COUNT)
        dir = CAR_DIR_S;

    sprite_t *car = (index == 0) ? garbage_yellow_sprites[dir] : garbage_red_sprites[dir];
    if (!car)
        return;

    int screen_x = character_x[index] - cam_x - (car->width / 2);
    int screen_y = character_y[index] - stored_cam_y - car->height + ISO_H_HALF;

    rdpq_sprite_blit(car, screen_x, screen_y, NULL);
}

/**
 * @brief character_draw: Render both vehicles in depth order.
 */
void character_draw(int cam_x)
{
    int first = 0;
    int second = 1;

    if (character_y[first] > character_y[second] ||
        (character_y[first] == character_y[second] && character_x[first] > character_x[second]))
    {
        first = 1;
        second = 0;
    }

    for (int i = 0; i < CHARACTER_COUNT; i++)
    {
        int index = (i == 0) ? first : second;
        int dir = char_facing[index];
        if (dir < 0 || dir >= CAR_DIR_COUNT)
            dir = CAR_DIR_S;

        sprite_t *car = (index == 0) ? garbage_yellow_sprites[dir] : garbage_red_sprites[dir];
        if (!car)
            continue;

        int screen_x = character_x[index] - cam_x - (car->width / 2);
        int screen_y = character_y[index] - stored_cam_y - car->height + ISO_H_HALF;

        rdpq_sprite_blit(car, screen_x, screen_y, NULL);
    }
}

/**
 * @brief character_get_position: Fetch a character's world position.
 */
void character_get_position(int index, int *x, int *y)
{
    if (index < 0 || index >= CHARACTER_COUNT)
        return;

    if (x)
        *x = character_x[index];
    if (y)
        *y = character_y[index];
}

/**
 * @brief character_get_active_player: Return active player index.
 */
int character_get_active_player(void)
{
    return active_player;
}

// character_set_full: Set whether a player is carrying concrete.
void character_set_full(int index, bool full)
{
    if (index < 0 || index >= CHARACTER_COUNT)
        return;
    character_full[index] = full;
}

// character_is_full: Return whether a player is carrying concrete.
bool character_is_full(int index)
{
    if (index < 0 || index >= CHARACTER_COUNT)
        return false;
    return character_full[index];
}

/**
 * @brief character_get_vehicle_sprite: Fetch vehicle sprite for color and direction.
 */
sprite_t *character_get_vehicle_sprite(int color_index, int dir)
{
    if (dir < 0 || dir >= CAR_DIR_COUNT)
        dir = CAR_DIR_S;

    if (color_index == 0)
        return garbage_yellow_sprites[dir];
    if (color_index == 1)
        return garbage_red_sprites[dir];

    return NULL;
}
