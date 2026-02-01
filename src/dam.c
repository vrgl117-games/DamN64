/* dam.c -- dam break logic
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "dam.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <libdragon.h>

#include "font.h"
#include "map.h"
#include "rdpq.h"

typedef enum
{
    WALL_INTACT = 0,
    WALL_BREAKING = 1,
    WALL_BROKEN = 2
} wall_state_t;

typedef struct
{
    int x;
    int y;
    wall_state_t state;
    uint32_t blink_tick;
    uint32_t end_tick;
    bool blink_on;
} wall_section_t;

#define MAX_WALL_TILES 10
#define MIN_BREAK_INTERVAL_SECONDS 10
#define BREAKING_DURATION_SECONDS 3
#define BREAKING_BLINK_TICKS (TICKS_PER_SECOND / 2)
#define TOP_ROW_LIMIT 13

static wall_section_t wall_sections[MAX_WALL_TILES];
static int wall_section_count = 0;
static int wall_intact_indices[MAX_WALL_TILES];
static int wall_intact_count = 0;
static int broken_section_count = 0;
static uint32_t next_break_tick = 0;
static int break_step = 0;
static int breaking_section_count = 0;

/**
 * @brief start_breaking_wall: Set a wall section to breaking state.
 */
static void start_breaking_wall(wall_section_t *section, uint32_t now)
{
    section->state = WALL_BREAKING;
    section->blink_on = true;
    section->blink_tick = now + BREAKING_BLINK_TICKS;
    section->end_tick = now + (BREAKING_DURATION_SECONDS * TICKS_PER_SECOND);
    game_map.tiles[section->y][section->x] = TILE_WALL_BREAKING;
}

/**
 * @brief pick_intact_index: Pick an intact wall index with optional row limit.
 */
static int pick_intact_index(int max_row)
{
    int pick = -1;
    int seen = 0;

    for (int i = 0; i < wall_intact_count; i++)
    {
        int section_index = wall_intact_indices[i];
        int y = wall_sections[section_index].y;
        if (max_row >= 0 && y > max_row)
            continue;

        seen++;
        if ((rand() % seen) == 0)
            pick = i;
    }

    return pick;
}

/**
 * @brief start_breaking_random_walls: Start breaking random wall sections.
 */
static void start_breaking_random_walls(int count, int max_row, uint32_t now)
{
    for (int i = 0; i < count && wall_intact_count > 0; i++)
    {
        int intact_index = pick_intact_index(max_row);
        if (intact_index < 0)
            intact_index = pick_intact_index(-1);
        if (intact_index < 0)
            return;

        int section_index = wall_intact_indices[intact_index];
        wall_section_t *section = &wall_sections[section_index];

        start_breaking_wall(section, now);

        wall_intact_indices[intact_index] = wall_intact_indices[wall_intact_count - 1];
        wall_intact_count--;
    }
}

/**
 * @brief dam_init: Build wall list and schedule first break.
 */
void dam_init(void)
{
    memset(wall_sections, 0, sizeof(wall_sections));
    memset(wall_intact_indices, 0, sizeof(wall_intact_indices));
    wall_section_count = 0;
    wall_intact_count = 0;
    broken_section_count = 0;
    break_step = 0;
    bool skipped_first = false;

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            if (game_map.tiles[y][x] == TILE_WALL && wall_section_count < MAX_WALL_TILES)
            {
                if (!skipped_first)
                {
                    skipped_first = true;
                    continue;
                }

                wall_sections[wall_section_count].x = x;
                wall_sections[wall_section_count].y = y;
                wall_sections[wall_section_count].state = WALL_INTACT;
                wall_sections[wall_section_count].blink_tick = 0;
                wall_sections[wall_section_count].end_tick = 0;
                wall_sections[wall_section_count].blink_on = true;

                wall_intact_indices[wall_intact_count] = wall_section_count;
                wall_intact_count++;
                wall_section_count++;
            }
        }
    }

    if (wall_section_count > 0)
    {
        wall_section_count--;
        wall_intact_count--;
    }

    uint32_t now = get_ticks();
    next_break_tick = now + TICKS_PER_SECOND;
    breaking_section_count = 0;
}

/**
 * @brief dam_update: Break wall sections over time.
 */
void dam_update(void)
{
    uint32_t now = get_ticks();
    breaking_section_count = 0;

    for (int i = 0; i < wall_section_count; i++)
    {
        wall_section_t *section = &wall_sections[i];

        if (section->state != WALL_BREAKING)
            continue;

        breaking_section_count++;

        if (now >= section->end_tick)
        {
            section->state = WALL_BROKEN;
            game_map.tiles[section->y][section->x] = TILE_BROKEN_WALL;
            broken_section_count++;
            continue;
        }

        if (now >= section->blink_tick)
        {
            section->blink_on = !section->blink_on;
            game_map.tiles[section->y][section->x] = section->blink_on ? TILE_WALL_BREAKING : TILE_BROKEN_WALL;
            section->blink_tick += BREAKING_BLINK_TICKS;
        }
    }

    if (wall_intact_count <= 0)
        return;

    while (now >= next_break_tick && wall_intact_count > 0)
    {
        int interval_index = break_step / 2;
        int interval_seconds = 20 - (interval_index * 5);
        if (interval_seconds < MIN_BREAK_INTERVAL_SECONDS)
            interval_seconds = MIN_BREAK_INTERVAL_SECONDS;

        int break_count = 1 + ((break_step - 1) / 2);
        if (break_count < 1)
            break_count = 1;

        if (break_step == 0)
            start_breaking_random_walls(break_count, TOP_ROW_LIMIT, now);
        else
            start_breaking_random_walls(break_count, -1, now);

        break_step++;
        next_break_tick += interval_seconds * TICKS_PER_SECOND;
    }
}

/**
 * @brief dam_draw_breach_bar: Draw breach level progress bar.
 */
void dam_draw_breach_bar(int screen_w)
{
    const int label_x = 8;
    const int label_y = 10;
    const int bar_x = 78;
    const int bar_y = 2;
    const int bar_w = 60;
    const int bar_h = 8;

    int broken = dam_get_broken_sections();
    if (broken < 0)
        broken = 0;
    if (broken > 3)
        broken = 3;

    rdpq_text_print(&(rdpq_textparms_t){.width = screen_w, .align = ALIGN_LEFT},
                    FONT_PIXEL, label_x, label_y, "Breach Level:");

    rdpq_set_mode_fill(RGBA32(0, 0, 0, 255));
    rdpq_fill_rectangle(bar_x, bar_y, bar_x + bar_w, bar_y + bar_h);

    if (broken > 0)
    {
        int solid_w = (bar_w * broken) / 3;
        rdpq_set_mode_fill(RGBA32(255, 90, 90, 255));
        rdpq_fill_rectangle(bar_x + 1, bar_y + 1, bar_x + solid_w - 1, bar_y + bar_h - 1);
    }

    rdpq_set_mode_fill(RGBA32(255, 255, 255, 255));
    rdpq_fill_rectangle(bar_x, bar_y, bar_x + bar_w, bar_y + 1);
    rdpq_fill_rectangle(bar_x, bar_y + bar_h - 1, bar_x + bar_w, bar_y + bar_h);
    rdpq_fill_rectangle(bar_x, bar_y, bar_x + 1, bar_y + bar_h);
    rdpq_fill_rectangle(bar_x + bar_w - 1, bar_y, bar_x + bar_w, bar_y + bar_h);
}

/**
 * @brief dam_repair_wall: Restore a broken wall section to intact.
 */
void dam_repair_wall(int grid_x, int grid_y)
{
    for (int i = 0; i < wall_section_count; i++)
    {
        wall_section_t *section = &wall_sections[i];
        if (section->x != grid_x || section->y != grid_y)
            continue;

        if (section->state == WALL_INTACT)
            return;

        if (section->state == WALL_BROKEN && broken_section_count > 0)
            broken_section_count--;

        section->state = WALL_INTACT;
        section->blink_tick = 0;
        section->end_tick = 0;
        section->blink_on = true;

        if (wall_intact_count < MAX_WALL_TILES)
        {
            wall_intact_indices[wall_intact_count] = i;
            wall_intact_count++;
        }

        game_map.tiles[grid_y][grid_x] = TILE_WALL;
        return;
    }
}

/**
 * @brief dam_get_broken_sections: Return number of broken wall sections.
 */
int dam_get_broken_sections(void)
{
    return broken_section_count;
}
