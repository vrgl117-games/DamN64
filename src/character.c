/*
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "character.h"
#include "rdpq.h"
#include "sprite.h"

#define CAR_DIR_COUNT 8
#define CAR_DIR_N 0
#define CAR_DIR_NE 1
#define CAR_DIR_E 2
#define CAR_DIR_SE 3
#define CAR_DIR_S 4
#define CAR_DIR_SW 5
#define CAR_DIR_W 6
#define CAR_DIR_NW 7

static sprite_t *police_sprites[CAR_DIR_COUNT] = {0};
static sprite_t *ambulance_sprites[CAR_DIR_COUNT] = {0};
static int character_x[CHARACTER_COUNT] = {0};
static int character_y[CHARACTER_COUNT] = {0};
static int active_player = 0;
static int char_facing[CHARACTER_COUNT] = {0};
static int iso_half_h = 0;

// character_init: Load vehicle sprites and initialize character state.
void character_init(int base_x, int base_y, int offset_x, int offset_y, int iso_half_h_value)
{
    police_sprites[CAR_DIR_N] = sprite_load("rom:/gfx/sprites/isometric-vehicles/police_N.sprite");
    police_sprites[CAR_DIR_NE] = sprite_load("rom:/gfx/sprites/isometric-vehicles/police_NE.sprite");
    police_sprites[CAR_DIR_E] = sprite_load("rom:/gfx/sprites/isometric-vehicles/police_E.sprite");
    police_sprites[CAR_DIR_SE] = sprite_load("rom:/gfx/sprites/isometric-vehicles/police_SE.sprite");
    police_sprites[CAR_DIR_S] = sprite_load("rom:/gfx/sprites/isometric-vehicles/police_S.sprite");
    police_sprites[CAR_DIR_SW] = sprite_load("rom:/gfx/sprites/isometric-vehicles/police_SW.sprite");
    police_sprites[CAR_DIR_W] = sprite_load("rom:/gfx/sprites/isometric-vehicles/police_W.sprite");
    police_sprites[CAR_DIR_NW] = sprite_load("rom:/gfx/sprites/isometric-vehicles/police_NW.sprite");

    ambulance_sprites[CAR_DIR_N] = sprite_load("rom:/gfx/sprites/isometric-vehicles/ambulance_N.sprite");
    ambulance_sprites[CAR_DIR_NE] = sprite_load("rom:/gfx/sprites/isometric-vehicles/ambulance_NE.sprite");
    ambulance_sprites[CAR_DIR_E] = sprite_load("rom:/gfx/sprites/isometric-vehicles/ambulance_E.sprite");
    ambulance_sprites[CAR_DIR_SE] = sprite_load("rom:/gfx/sprites/isometric-vehicles/ambulance_SE.sprite");
    ambulance_sprites[CAR_DIR_S] = sprite_load("rom:/gfx/sprites/isometric-vehicles/ambulance_S.sprite");
    ambulance_sprites[CAR_DIR_SW] = sprite_load("rom:/gfx/sprites/isometric-vehicles/ambulance_SW.sprite");
    ambulance_sprites[CAR_DIR_W] = sprite_load("rom:/gfx/sprites/isometric-vehicles/ambulance_W.sprite");
    ambulance_sprites[CAR_DIR_NW] = sprite_load("rom:/gfx/sprites/isometric-vehicles/ambulance_NW.sprite");

    active_player = 0;
    iso_half_h = iso_half_h_value;

    for (int i = 0; i < CHARACTER_COUNT; i++)
    {
        character_x[i] = base_x + i * offset_x;
        character_y[i] = base_y + i * offset_y;
        char_facing[i] = CAR_DIR_S;
    }
}

// character_update: Update movement, facing, and active character.
void character_update(control_t keys, character_block_fn is_blocked)
{
    if (keys.c_up || keys.c_down)
        active_player = (active_player + 1) % CHARACTER_COUNT;

    int dir_x = 0;
    int dir_y = 0;

    if (keys.direction & d_left)
        dir_x -= 1;
    if (keys.direction & d_right)
        dir_x += 1;
    if (keys.direction & d_up)
        dir_y -= 1;
    if (keys.direction & d_down)
        dir_y += 1;

    int *player_x = &character_x[active_player];
    int *player_y = &character_y[active_player];
    int *player_facing = &char_facing[active_player];

    if (dir_x || dir_y)
    {
        int step_x = 4;
        int step_y = 4;
        if (dir_x && dir_y)
            step_y = 2;

        int next_x = *player_x + dir_x * step_x;
        int next_y = *player_y + dir_y * step_y;

        if (!is_blocked || !is_blocked(next_x, next_y))
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

// character_draw: Render both vehicles in depth order.
void character_draw(int cam_x, int cam_y)
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

        sprite_t *car = (index == 0) ? police_sprites[dir] : ambulance_sprites[dir];
        if (!car)
            continue;

        int screen_x = character_x[index] - cam_x - (car->width / 2);
        int screen_y = character_y[index] - cam_y - car->height + iso_half_h;

        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_sprite_blit(car, screen_x, screen_y, NULL);
    }
}

// character_get_position: Fetch a character's world position.
void character_get_position(int index, int *x, int *y)
{
    if (index < 0 || index >= CHARACTER_COUNT)
        return;

    if (x)
        *x = character_x[index];
    if (y)
        *y = character_y[index];
}

// character_get_active_player: Return active player index.
int character_get_active_player(void)
{
    return active_player;
}
