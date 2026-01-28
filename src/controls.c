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
 * @brief controls_read_port: Fill control state for one controller port.
 */
static void controls_read_port(control_t *keys, int port)
{
    if (!keys)
        return;

    joypad_buttons_t down = joypad_get_buttons_pressed(port);
    joypad_buttons_t pressed = joypad_get_buttons(port);
    int stick_x = joypad_get_axis_pressed(port, JOYPAD_AXIS_STICK_X);
    int stick_y = joypad_get_axis_pressed(port, JOYPAD_AXIS_STICK_Y);

    keys->rumble = (joypad_get_accessory_type(port) == JOYPAD_ACCESSORY_TYPE_RUMBLE_PAK);

    if (down.a)
        keys->A = true;
    if (down.b)
        keys->B = true;
    if (down.l)
        keys->L = true;
    if (down.z)
        keys->z = true;

    if (down.d_up || stick_y > 0 || pressed.d_up)
        keys->direction |= d_up;
    if (down.d_down || stick_y < 0 || pressed.d_down)
        keys->direction |= d_down;
    if (down.d_left || stick_x < 0 || pressed.d_left)
        keys->direction |= d_left;
    if (down.d_right || stick_x > 0 || pressed.d_right)
        keys->direction |= d_right;
    if (down.start)
        keys->start = true;
}

/**
 * @brief controls_get_keys: Poll controller input for both ports.
 * [0] is always player 1, [1] is NULL when controller 2 is not connected.
 */
control_t **controls_get_keys(void)
{
    static control_t keys[2];
    static control_t *out[2];

    memset(keys, 0, sizeof(keys));
    out[0] = &keys[0];
    out[1] = NULL;

    joypad_poll();

    bool p2_connected = joypad_is_connected(JOYPAD_PORT_2);

    controls_read_port(&keys[0], JOYPAD_PORT_1);

    if (p2_connected)
    {
        controls_read_port(&keys[1], JOYPAD_PORT_2);
        out[1] = &keys[1];
    }

    return out;
}
