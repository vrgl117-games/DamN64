/* bgm.c -- bgm helpers implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "bgm.h"

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#define BGM_CHANNEL 0
#define GAME_VOLUME 1.0f
#define PAUSE_VOLUME 0.33f

static wav64_t bgm_wav;
static bool bgm_started = false;
static bgm_track_t current_track = BGM_TRACK_INTRO_PAUSE;

void bgm_init()
{
    audio_init(22050, 4);
    mixer_init(1);

    wav64_init_compression(0);
}

static const char *bgm_track_path(bgm_track_t track)
{
    switch (track)
    {
    case BGM_TRACK_JOY:
        return "rom:/sfx/bgms/DamN64_joy_LITE_22k.wav64";
    case BGM_TRACK_WEIRD:
        return "rom:/sfx/bgms/DamN64_weird_LITE_22k.wav64";
    case BGM_TRACK_WEIRD2:
        return "rom:/sfx/bgms/DamN64_weird2_LITE_22k.wav64";
    case BGM_TRACK_YOULOSE:
        return "rom:/sfx/bgms/DamN64_youlose_LITE_22k.wav64";
    case BGM_TRACK_INTRO_PAUSE:
    default:
        return "rom:/sfx/bgms/DamN64_intro_pause_screen_22k.wav64";
    }
}

static void bgm_open_track(bgm_track_t track)
{
    const char *path = bgm_track_path(track);

    mixer_ch_stop(BGM_CHANNEL);
    wav64_close(&bgm_wav);

    wav64_open(&bgm_wav, path);
    wav64_set_loop(&bgm_wav, true);
    wav64_play(&bgm_wav, BGM_CHANNEL);
    mixer_ch_set_vol(BGM_CHANNEL, GAME_VOLUME, GAME_VOLUME);
}

void bgm_pause()
{
    if (!bgm_started)
        return;

    bgm_set_volume(PAUSE_VOLUME);
}

void bgm_play()
{
    if (!bgm_started)
        return;

    bgm_set_volume(GAME_VOLUME);
}

void bgm_mute()
{
    if (!bgm_started)
        return;

    bgm_set_volume(0.0f);
}

void bgm_set_track(bgm_track_t track)
{
    if (current_track == track && bgm_started)
        return;

    current_track = track;

    if (!bgm_started)
        return;

    float pos = 0.0f;
    if (mixer_ch_playing(BGM_CHANNEL))
        pos = mixer_ch_get_pos(BGM_CHANNEL);

    bgm_open_track(track);

    int len = bgm_wav.wave.len;
    if (len > 0 && len != WAVEFORM_UNKNOWN_LEN && pos >= len)
    {
        pos = fmodf(pos, (float)len);
    }

    if (pos > 0.0f)
        mixer_ch_set_pos(BGM_CHANNEL, pos);
}

void bgm_set_volume(float volume)
{
    mixer_ch_set_vol(BGM_CHANNEL, volume, volume);
}

void bgm_start()
{
    if (bgm_started)
        return;

    bgm_open_track(current_track);
    bgm_started = true;
}

void bgm_stop()
{
    mixer_ch_stop(BGM_CHANNEL);
    wav64_close(&bgm_wav);

    bgm_started = false;

    mixer_close();
    audio_close();
}

void bgm_update()
{
    if (audio_can_write())
    {
        int16_t *buf = audio_write_begin();
        mixer_poll(buf, audio_get_buffer_length());
        audio_write_end();
    }
}
