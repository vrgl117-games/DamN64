/* rdpq.c -- rdpq helpers implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include "rdpq.h"

// rdpq_draw_faded_sprite: Draw sprite with a fade alpha.
void rdpq_draw_faded_sprite(sprite_t *sp, int x, int y, uint8_t alpha)
{
    rdpq_set_mode_standard();
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY_CONST);

    rdpq_set_fog_color(RGBA32(255, 255, 255, alpha));

    rdpq_sprite_blit(sp, x, y, NULL);
}
