#!/usr/bin/env bash
# connected_shot.sh — capture a CONNECTED snesmon TUI as text.
#
# Launches headless MAME on the host (SDL offscreen) and runs snesmon in the build
# container with --network=host so it reaches the Lua bridge on :41816, drives it
# over tmux, and dumps the screen with capture-pane. Proves the Phase 2 pipeline
# end-to-end through the real binary (status "Running", live target memory).
#
# Usage:
#   linux/spikes/connected_shot.sh [ROM] [AUTOBOOT]
#     ROM       cart path (default: test-roms/drmon-test.sfc, rel. to linux/)
#     AUTOBOOT  -autoboot_script (default: mame_bridge.lua; use spikes/verify_ppu.lua
#               to seed VRAM A0..AF for a PPU-window check)
#   KEYS env: ';'-separated `tmux send-keys` arg-lists, eval'd in order, 0.6s apart
#             e.g. KEYS='M-m;C-b' opens a Memory window in byte mode.
#
# Requires: mame 0.277 on PATH, the build image + tree (`task build`), Docker.
# NOTE: menu walks (PPU via Type submenu, MemOps -> Search, F10 -> Control) need the
# local menu (right-click) / reverse-video menu-bar highlight and don't render well
# in capture-pane -p — drive those interactively (`task run SYS=snes`).
set -euo pipefail

if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ]; then
    sed -n '2,21p' "$0" | sed 's/^# \{0,1\}//'
    exit 0
fi

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LINUX="$(dirname "$HERE")"
ROOT="$(cd "$LINUX/../../../.." && pwd)"
BUILD="${DRMON_BUILD:-/tmp/drmon-build}"
IMAGE="${DRMON_IMAGE:-drmon-build}"
PORT=41816

ROM="${1:-test-roms/drmon-test.sfc}"
AUTOBOOT="${2:-mame_bridge.lua}"

cleanup() { pkill -9 mame 2>/dev/null || true; fuser -k "${PORT}/tcp" 2>/dev/null || true; }
trap cleanup EXIT
cleanup; sleep 0.5

( cd "$LINUX" && env SDL_VIDEODRIVER=offscreen SDL_AUDIODRIVER=dummy \
    mame snes -cart "$ROM" -debug -debugger none -autoboot_script "$AUTOBOOT" \
    -video none -nothrottle &>/tmp/connected_shot_mame.log & )
for _ in $(seq 40); do fuser "${PORT}/tcp" &>/dev/null && break; sleep 0.25; done

docker run --rm --network=host -v "$ROOT":/src -v "$BUILD":/build -w /build \
  -e TERM=xterm-256color -e DRMON_MAME_ADDR="127.0.0.1:${PORT}" \
  -e ASAN_OPTIONS=halt_on_error=0:detect_leaks=0:abort_on_error=0 \
  -e KEYS="${KEYS:-}" "$IMAGE" bash -uc '
    tmux new-session -d -s d -x 80 -y 25
    tmux send-keys -t d "./snesmon 2>/dev/null" Enter
    sleep 3
    IFS=";" read -ra K <<< "${KEYS:-}"
    for k in "${K[@]}"; do
      [ -z "$k" ] && continue
      eval tmux send-keys -t d $k
      sleep 0.6
    done
    sleep 0.5
    tmux capture-pane -t d -p
    tmux kill-server 2>/dev/null || true
  '
