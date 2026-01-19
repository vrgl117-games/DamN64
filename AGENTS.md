# AGENTS.md - Coding Agent Guidelines

This document provides guidelines for AI coding agents working on this N64 libdragon game project.

## Project Overview

- **Type**: Nintendo 64 game for n64brew game jam
- **SDK**: libdragon (modern open-source N64 SDK)
- **Language**: C (C99)
- **Build**: Docker-based cross-compilation (MIPS)
- **Resolution**: 640x480, 16-bit color, double-buffered

## Build Commands

### Primary Commands

```bash
make build          # Build ROM (uses Docker automatically if available)
make rebuild        # Clean and rebuild ROM
make clean          # Remove build artifacts and generated files
make setup          # Create Docker build environment (first-time setup)
make resetup        # Force recreate Docker environment
```

### Running/Testing

```bash
make ares           # Launch ROM in Ares emulator (macOS)
make flashair       # Flash ROM to EverDrive via FlashAir SD card
make help           # Show all available make targets
```

### Build Output

- ROM file: `TBD-64.z64`
- ELF file: `build/TBD-64.elf`
- Map file: `build/TBD-64.map`
- Filesystem: `TBD-64.dfs`

### Notes

- No unit tests exist; testing is done via emulator or hardware
- Build requires Docker for cross-compilation to MIPS
- LSP should work via `.clangd` config (points to local libdragon clone)
- Agents may verify builds with `make build`
- Agents must not launch `make ares`; human handles emulator testing

## Project Structure

```
├── src/                 # C source files
│   ├── main.c          # Entry point, main loop, system init
│   ├── game.c          # Core game logic, map rendering, camera
│   ├── character.c     # Player/vehicle movement and rendering
│   ├── controls.c      # Controller input handling
│   ├── screens.c       # Screen state machine (intro, title, game)
│   ├── rdpq.c          # RDP graphics helper functions
│   ├── map.c           # Static map tile data
│   └── sprite.c        # Sprite loading utilities
├── include/            # Header files (mirrors src/)
├── resources/          # Source assets
│   ├── gfx/sprites/   # PNG sprite images
│   └── sfx/bgms/      # MP3 audio files
├── filesystem/         # Generated assets (sprites, audio) - gitignored
├── build/              # Build artifacts - gitignored
├── Dockerfile          # libdragon build environment
└── Makefile            # Build system
```

## Code Style Guidelines

### File Header

Every source file must include the standard header:

```c
/* filename.c -- brief description
 *
 * Copyright (C) 2026 VRGL117 Games
 *
 * This software may be modified and distributed under the terms
 * of the Apache license. See the LICENSE file for details.
 */
```

### Include Order

1. Corresponding header file (for .c files)
2. Standard library headers (`<stdlib.h>`, `<string.h>`, `<math.h>`)
3. libdragon headers (`<libdragon.h>`)
4. Project headers (`"game.h"`, `"controls.h"`)

### Naming Conventions

| Type | Convention | Example |
|------|-----------|---------|
| Functions | `snake_case` | `game_update()`, `character_draw()` |
| Variables | `snake_case` | `cam_x`, `player_x`, `split_screen_active` |
| Constants/Macros | `UPPER_SNAKE_CASE` | `MAP_WIDTH`, `ISO_H`, `TILE_COUNT` |
| Types (typedef) | `snake_case_t` | `control_t`, `map_t`, `direction_t` |
| Enums | `UPPER_SNAKE_CASE` values | `TILE_BASE`, `CAR_DIR_N` |
| Static globals | `snake_case` | `static int cam_x = 0;` |

### Function Documentation

Use Doxygen-style brief comments:

```c
/**
 * @brief function_name: Brief one-line description.
 */
static void function_name(int param)
{
    // implementation
}
```

For simpler functions, single-line comments are acceptable:

```c
// function_name: Brief description.
void function_name(void)
{
}
```

### Formatting

- **Indentation**: 4 spaces (no tabs)
- **Braces**: Allman style (opening brace on new line for functions, same line for control structures)
- **Line length**: No strict limit, but keep reasonable (~100 chars)
- **Pointer style**: `type *name` (space before asterisk)

```c
void game_update(control_t keys)
{
    if (condition)
    {
        // code
    }
    else
    {
        // code
    }

    for (int i = 0; i < count; i++)
    {
        // code
    }
}
```

### Header Guards

Use double-underscore style:

```c
#ifndef __FILENAME_H__
#define __FILENAME_H__

// declarations

#endif //__FILENAME_H__
```

### Error Handling

- N64 has limited error handling capabilities
- Use `return` with sentinel values for failure (`false`, `-1`, `NULL`)
- Validate array bounds before access
- Check for `NULL` pointers when loading resources

```c
sprite_t *sprite = sprite_load("rom:/path/to/sprite.sprite");
if (!sprite)
    return;  // Handle gracefully, don't crash
```

### Memory Management

- Sprites loaded with `sprite_load()` must be freed with `sprite_free()`
- Prefer static allocation where possible (N64 has 4MB RAM)
- Avoid dynamic allocation in game loops

### libdragon-Specific Patterns

**Display/Rendering Loop**:
```c
surface_t *disp = display_get();
rdpq_attach_clear(disp, NULL);
// ... rendering code ...
rdpq_detach_show();  // or rdpq_detach() + display_show()
```

**Sprite Rendering**:
```c
rdpq_set_mode_standard();
rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);  // For transparency
rdpq_sprite_blit(sprite, x, y, NULL);
```

**Controller Input**:
```c
joypad_poll();
joypad_buttons_t buttons = joypad_get_buttons_pressed(JOYPAD_PORT_1);
```

## Asset Pipeline

### Sprites

- Source: `resources/gfx/sprites/*.png`
- Output: `filesystem/gfx/sprites/*.sprite`
- Vehicle sprites use `-f RGBA16` for transparency
- Loaded at runtime: `sprite_load("rom:/gfx/sprites/name.sprite")`

### Audio

- Source: `resources/sfx/bgms/*.mp3`
- Output: `filesystem/sfx/bgms/*.wav64`
- Converted with compression level 3

## Architecture Notes

### Coordinate Systems

- **Screen coords**: Pixel position on display (0,0 top-left)
- **World coords**: Absolute pixel position in game world
- **Grid coords**: Tile indices (0 to MAP_WIDTH-1, 0 to MAP_HEIGHT-1)
- **Isometric projection**: Diamond-shaped tiles (64x32 logical footprint)

### Camera System

- Horizontal scrolling only (no vertical)
- Split-screen when players are far apart horizontally
- Camera clamped to map bounds

### Game Loop

```
main() → game_init() → while(true) { controls → update → draw }
```

State machine: `intro` → `title` → `game`

## N64 Performance Guidelines

The N64 has limited CPU (93.75 MHz MIPS) and GPU (RDP) resources. Follow these practices:

### RDP Mode Changes

- **Set RDP modes once before loops, not inside them**
- `rdpq_set_mode_standard()`, `rdpq_mode_blender()`, `rdpq_set_mode_copy()` are expensive
- Bad: calling `rdpq_mode_blender()` for every tile in a 27x27 map (729 calls)
- Good: call once before the tile loop (1 call)

### Rendering

- Use `rdpq_set_mode_copy(true)` for opaque sprites (faster than standard mode)
- Use `rdpq_set_mode_standard()` + `rdpq_mode_blender()` only when alpha blending needed
- Implement view frustum culling - skip drawing off-screen tiles/sprites

### Text Rendering

- `rdpq_text_printf()` is expensive - avoid in hot paths
- Debug overlays should be toggle-able (use L button in this project)

### General

- Cache values like `display_get_width()` / `display_get_height()` if called repeatedly
- Prefer integer math over floating point
- Hoist loop-invariant calculations outside loops
- Division by powers of 2 can use bit shifts (compiler often optimizes this)

## Controls

| Button | Action |
|--------|--------|
| D-Pad / Stick | Move active vehicle |
| C-Up / C-Down | Switch active vehicle |
| L | Toggle debug overlay |
