/* fps.h -- fps counter header
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef __FPS_H__
#define __FPS_H__

#ifndef NDEBUG
// fps_tick: Update FPS counter using libdragon ticks.
void fps_tick(void);
// fps_draw: Render FPS value in the top-right corner.
void fps_draw(void);
#else
static inline void fps_tick(void)
{
}

static inline void fps_draw(void)
{
}
#endif

#endif //__FPS_H__
