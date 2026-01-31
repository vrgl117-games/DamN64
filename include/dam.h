/* dam.h -- dam break logic
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef __DAM_H__
#define __DAM_H__

#include <stdbool.h>

void dam_init(void);
void dam_update(void);
int dam_get_broken_sections(void);
void dam_draw_breach_bar(int screen_w);

#endif //__DAM_H__
