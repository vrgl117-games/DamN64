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
make build && make ares 2>&1 | tee /tmp/ares.log &
sleep 1  # wait for boot
```

## Take screenshot
```bash
osascript -e 'tell application "ares" to activate'
screencapture -x /tmp/game.png  # capture frontmost
```

## Send inputs
```bash
cliclick kp:enter        # Controller 1 Start
cliclick kp:w            # Controller 1 D-pad up
cliclick kp:s            # Controller 1 D-pad down
cliclick kp:a            # Controller 1 D-pad left
cliclick kp:d            # Controller 1 D-pad right
cliclick t:z             # Controller 1 Z button

cliclick kp:arrow-up     # Controller 2 D-pad up
cliclick kp:arrow-down   # Controller 2 D-pad down
cliclick kp:arrow-left   # Controller 2 D-pad left
cliclick kp:arrow-right  # Controller 2 D-pad right
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
5. Wait 0.1s
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
