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

// dam_init: Build wall list and schedule first break.
void dam_init(void);
// dam_update: Break wall sections over time.
void dam_update(void);
// dam_get_broken_sections: Return number of broken wall sections.
int dam_get_broken_sections(void);
// dam_draw_breach_bar: Draw breach level progress bar.
void dam_draw_breach_bar(int screen_w);
// dam_repair_wall: Restore a broken wall section to intact.
void dam_repair_wall(int grid_x, int grid_y);
// dam_is_spreading: Return true if any water spread is active.
bool dam_is_spreading(void);

#endif //__DAM_H__
