/* sprite.c -- sprite helpers implementation
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */

#include <stdarg.h>
#include <stdio.h>

#include "sprite.h"

// sprite_loadf: Format sprite path and load it.
sprite_t *sprite_loadf(const char *const format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    return sprite_load(buffer);
}
