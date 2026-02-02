/* title.h -- title screen helpers header
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef __TITLE_H__
#define __TITLE_H__

// title_update: Update truck positions and spawning.
void title_update(int screen_w);
// title_draw: Render trucks on the band.
void title_draw(int screen_w, int screen_h);

#endif //__TITLE_H__
