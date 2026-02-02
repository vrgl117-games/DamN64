/* controls.h -- controls helpers header
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#ifndef __CONTROLS_H__
#define __CONTROLS_H__

#include <libdragon.h>

typedef enum direction
{
    d_none = 0,
    d_up = 1 << 0,
    d_down = 1 << 1,
    d_left = 1 << 2,
    d_right = 1 << 3
} direction_t;

typedef struct
{
    bool A;
    bool B;
    bool L;
    bool z;
    bool start;

    bool rumble;

    direction_t direction;

} control_t;

// controls_get_keys: Poll controller input for both ports.
control_t **controls_get_keys(void);

#endif //__CONTROLS_H__
