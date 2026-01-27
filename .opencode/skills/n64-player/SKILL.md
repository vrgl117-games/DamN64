---
name: n64-player
description: Control N64 games in ares emulator - take screenshots, send inputs, navigate autonomously
---

## Overview
Play N64 ROMs in ares by taking screenshots to see the game state and sending controller inputs. Use this to test ROMs after code changes.

## Requirements
- cliclick (`brew install cliclick`)
- Accessibility permissions for your terminal

## Launch ROM
```bash
make ares 2>&1 | tee /tmp/ares.log &
sleep 3  # wait for boot
```

## Take screenshot
```bash
screencapture -l $(osascript -e 'tell app "ares" to id of window 1') /tmp/game.png
```

Fallback if window ID fails:
```bash
osascript -e 'tell application "ares" to activate'
screencapture -x /tmp/game.png  # capture frontmost
```

## Send inputs
```bash
cliclick kp:return        # Start / confirm
cliclick kp:arrow-up      # D-pad up
cliclick kp:arrow-down    # D-pad down
cliclick kp:arrow-left    # D-pad left
cliclick kp:arrow-right   # D-pad right
cliclick kp:a             # A button
cliclick kp:b             # B button
cliclick kp:l             # L button
cliclick kp:r             # R button
cliclick kp:z             # Z button
cliclick kp:plus          # C-Up
cliclick kp:equal         # C-Down
```

Hold for duration:
```bash
cliclick kd:arrow-right && sleep 2 && cliclick ku:arrow-right
```

## Gameplay loop
1. Screenshot
2. Look at the image — what's on screen?
3. Decide next input
4. Send input
5. Wait 0.5-1s
6. Repeat

## Focus window
```bash
osascript -e 'tell application "ares" to activate'
```

## Check for crash
```bash
grep -qiE "signal|abort|fault|exception" /tmp/ares.log && cat /tmp/ares.log
```

## Exit
```bash
osascript -e 'tell application "ares" to quit'
```

Note: After completing a test run, quit ares to avoid background processes.

## Force quit if stuck
```bash
pkill -9 -f "/Applications/ares.app/Contents/MacOS/ares"
```
