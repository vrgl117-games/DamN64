/* bgm.h -- bgm helpers header
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef __BGM_H__
#define __BGM_H__

#include <libdragon.h>

typedef enum
{
    BGM_TRACK_INTRO_PAUSE = 0,
    BGM_TRACK_JOY = 1,
    BGM_TRACK_WEIRD = 2,
    BGM_TRACK_WEIRD2 = 3,
    BGM_TRACK_YOULOSE = 4
} bgm_track_t;

void bgm_init();
void bgm_pause();
void bgm_play();
void bgm_mute();
void bgm_set_track(bgm_track_t track);
void bgm_set_volume(float volume);
void bgm_start();
void bgm_stop();
void bgm_update();

#endif //__BGM_H__
