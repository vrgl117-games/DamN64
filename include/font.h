/* font.h -- font helpers header
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef __FONT_H__
#define __FONT_H__

#include <libdragon.h>

#define FONT_DEBUG 1
#define FONT_PIXEL_SQUARE 2
#define FONT_PIXEL 3

// font_init: Load fonts and register them with rdpq.
void font_init();

#endif //__FONT_H__
