/* controls.c -- controls helpers implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include <string.h>

#include "controls.h"

control_t controls_get_keys()
{
    control_t keys = {0};
    memset(&keys, 0, sizeof(control_t));

    joypad_poll();

    if (!joypad_is_connected(JOYPAD_PORT_1))
        return keys;

    joypad_buttons_t down = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    joypad_buttons_t pressed = joypad_get_buttons(JOYPAD_PORT_1);
    int stick_x = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_X);
    int stick_y = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_Y);

    keys.plugged = true;

    keys.rumble = (joypad_get_accessory_type(JOYPAD_PORT_1) == JOYPAD_ACCESSORY_TYPE_RUMBLE_PAK);

    keys.pressed = true;

    if (down.a)
        keys.A = true;
    else if (down.b)
        keys.B = true;
    else if (down.d_up || down.c_up || stick_y > 0)
        keys.direction = d_up;
    else if (down.d_down || down.c_down || stick_y < 0)
        keys.direction = d_down;
    else if (down.d_left || down.c_left || stick_x < 0)
        keys.direction = d_left;
    else if (down.d_right || down.c_right || stick_x > 0)
        keys.direction = d_right;
    else if (down.start)
        keys.start = true;
    else if (down.l && down.r)
        keys.fps = true;
    else if (down.l && pressed.r)
        keys.fps = true;
    else if (pressed.l && down.r)
        keys.fps = true;
    else
        keys.pressed = false;

    return keys;
}
