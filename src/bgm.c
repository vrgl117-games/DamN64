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

#include <libdragon.h>

#define BGM_CHANNEL 0
#define GAME_VOLUME 1.0f
#define PAUSE_VOLUME 0.33f

static wav64_t bgm_wav;
static bool bgm_started = false;

void bgm_init()
{
    audio_init(44100, 4);
    mixer_init(1);

    wav64_init_compression(0);
}

static void bgm_open_track()
{
    const char *path = "rom:/sfx/bgms/DamN64_intro_pause_screen.wav64";

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

    mixer_ch_set_vol(BGM_CHANNEL, PAUSE_VOLUME, PAUSE_VOLUME);
}

void bgm_play()
{
    if (!bgm_started)
        return;

    mixer_ch_set_vol(BGM_CHANNEL, GAME_VOLUME, GAME_VOLUME);
}

void bgm_start()
{
    if (bgm_started)
        return;

    bgm_open_track();
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
