/* screens.h -- screens helpers header
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef __SCREENS_H__
#define __SCREENS_H__

#include <libdragon.h>

typedef enum screens
{
    intro,
    title,
} screen_t;

void screen_init();
bool screen_intro(display_context_t disp);
void screen_timer_title();
void screen_title(display_context_t disp);

#endif //__SCREENS_H__
