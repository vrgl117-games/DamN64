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

// bgm_init: Initialize audio and mixer state.
void bgm_init();
// bgm_pause: Lower music volume for pause.
void bgm_pause();
// bgm_play: Restore normal music volume.
void bgm_play();
// bgm_mute: Mute music output.
void bgm_mute();
// bgm_set_track: Switch background music track.
void bgm_set_track(bgm_track_t track);
// bgm_set_volume: Set music volume level.
void bgm_set_volume(float volume);
// bgm_start: Start background music playback.
void bgm_start();
// bgm_stop: Stop background music playback.
void bgm_stop();
// bgm_update: Feed audio mixer buffers.
void bgm_update();

#endif //__BGM_H__
