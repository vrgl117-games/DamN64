/* controls.c -- controls helpers implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include <string.h>

#include "controls.h"

/**
 * @brief controls_get_keys: Poll controller input and build control state.
 */
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

    if (down.a)
        keys.A = true;
    if (down.b)
        keys.B = true;
    if (down.l)
        keys.L = true;
    if (down.c_up)
        keys.c_up = true;
    if (down.c_down)
        keys.c_down = true;

    if (down.d_up || stick_y > 0 || pressed.d_up)
        keys.direction |= d_up;
    if (down.d_down || stick_y < 0 || pressed.d_down)
        keys.direction |= d_down;
    if (down.d_left || stick_x < 0 || pressed.d_left)
        keys.direction |= d_left;
    if (down.d_right || stick_x > 0 || pressed.d_right)
        keys.direction |= d_right;
    if (down.start)
        keys.start = true;

    return keys;
}
