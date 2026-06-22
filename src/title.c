/* title.c -- title screen helpers implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "title.h"

#include <stdlib.h>

#include "character.h"
#include "game.h"

#define TITLE_ISO_H 16
#define TITLE_TRUCK_MAX 4
#define TITLE_TRUCK_VARIANTS 2
#define TITLE_TRUCK_SPAWN_FRAMES 45
#define TITLE_TRUCK_MIN_SPEED 1
#define TITLE_TRUCK_MAX_SPEED 2
#define TITLE_DIR_E 2
#define TITLE_DIR_W 6

typedef struct
{
    bool active;
    int x;
    int y;
    int speed;
    int dir;
    sprite_t *sprite;
} title_truck_t;

static int title_frame = 0;
static title_truck_t title_trucks[TITLE_TRUCK_MAX] = {0};
static int title_draw_order[TITLE_TRUCK_MAX] = {0};
static int title_draw_count = 0;

/**
 * @brief title_trucks_spawn: Spawn a random truck.
 */
static void title_trucks_spawn(int screen_w)
{
    int slot = -1;
    for (int i = 0; i < TITLE_TRUCK_MAX; i++)
    {
        if (!title_trucks[i].active)
        {
            slot = i;
            break;
        }
    }

    if (slot < 0)
        return;

    int dir = (rand() & 1) ? TITLE_DIR_E : TITLE_DIR_W;
    int color_index = rand() % TITLE_TRUCK_VARIANTS;
    sprite_t *sprite = character_get_vehicle_sprite(color_index, dir);
    if (!sprite)
        return;

    int start_x = (dir == TITLE_DIR_E) ? -sprite->width : screen_w + sprite->width;
    int speed = TITLE_TRUCK_MIN_SPEED + (rand() % (TITLE_TRUCK_MAX_SPEED - TITLE_TRUCK_MIN_SPEED + 1));

    title_trucks[slot].active = true;
    title_trucks[slot].x = start_x;
    title_trucks[slot].y = (rand() % 10) * 3 - 10;
    title_trucks[slot].speed = speed;
    title_trucks[slot].dir = (dir == TITLE_DIR_E) ? 1 : -1;
    title_trucks[slot].sprite = sprite;
}

/**
 * @brief title_update: Update truck positions and spawning.
 */
void title_update(int screen_w)
{
    title_frame++;

    if (title_frame % TITLE_TRUCK_SPAWN_FRAMES == 0)
        title_trucks_spawn(screen_w);

    for (int i = 0; i < TITLE_TRUCK_MAX; i++)
    {
        if (!title_trucks[i].active)
            continue;

        title_trucks[i].x += title_trucks[i].dir * title_trucks[i].speed;

        if (title_trucks[i].x < -title_trucks[i].sprite->width * 2 ||
            title_trucks[i].x > screen_w + title_trucks[i].sprite->width * 2)
        {
            title_trucks[i].active = false;
        }
    }

    title_draw_count = 0;
    for (int i = 0; i < TITLE_TRUCK_MAX; i++)
    {
        if (!title_trucks[i].active || !title_trucks[i].sprite)
            continue;

        title_draw_order[title_draw_count++] = i;
    }

    for (int i = 0; i < title_draw_count - 1; i++)
    {
        for (int j = i + 1; j < title_draw_count; j++)
        {
            int a = title_draw_order[i];
            int b = title_draw_order[j];

            if (title_trucks[a].y > title_trucks[b].y)
            {
                title_draw_order[i] = b;
                title_draw_order[j] = a;
            }
        }
    }
}

/**
 * @brief title_draw: Render trucks on the band.
 */
void title_draw(int screen_w, int screen_h)
{
    for (int i = 0; i < title_draw_count; i++)
    {
        int index = title_draw_order[i];
        int screen_x = title_trucks[index].x;
        int screen_y = screen_h / 2 - title_trucks[index].sprite->height + ISO_H_HALF + title_trucks[index].y - 1;

        rdpq_sprite_blit(title_trucks[index].sprite, screen_x, screen_y, NULL);
    }
}
