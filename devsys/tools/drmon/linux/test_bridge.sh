#!/usr/bin/env bash
# test_bridge.sh — launch headless MAME, run the protocol test suite, clean up.
# Usage: bash linux/test_bridge.sh [snes|gen]
# Requires: mame on PATH; called by 'task test-bridge' inside the drmon build container.
# Both platforms: -debugger none + a Lua bridge on 127.0.0.1:41816 (or $DRMON_MAME_ADDR).
#   SNES → mame_bridge.lua + test_bridge.py;  GEN → mame_genesis_bridge.lua + test_genesis_bridge.py.
set -euo pipefail

usage() { echo "Usage: $0 [snes|gen]"; exit 0; }
[ "${1-}" = "-h" ] || [ "${1-}" = "--help" ] && usage

SYS="${1:-snes}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

case "$SYS" in
  snes) DRIVER="snes";    LUA="$SCRIPT_DIR/mame_bridge.lua" ;;
  gen)  DRIVER="genesis"; LUA="$SCRIPT_DIR/mame_genesis_bridge.lua" ;;
  *)    echo "Unknown SYS '$SYS'; use snes or gen"; exit 1 ;;
esac

# Unified -debugger none bridge: both platforms listen on DRMON_MAME_ADDR (default :41816).
# DRMON_BRIDGE_DIR lets the wrapper dofile mame_cpu_bridge.lua regardless of MAME's chunk name.
_addr="${DRMON_MAME_ADDR:-127.0.0.1:41816}"
PORT="${_addr##*:}"
export DRMON_MAME_ADDR="127.0.0.1:$PORT"
export DRMON_BRIDGE_DIR="$SCRIPT_DIR"

# Kill any stale process holding the port so the test gets a clean socket.
fuser -k "${PORT}/tcp" 2>/dev/null || true
sleep 0.2

# Locate a test ROM.
REPO_ROOT="$(cd "$SCRIPT_DIR/../../../.." && pwd)"
ROM_DIR="$SCRIPT_DIR/test-roms"

if [ "$SYS" = "snes" ]; then
    # Prefer the vendored test ROM (deterministic NOP sled); fall back to any SNES ROM.
    ROM=$(find "$ROM_DIR" -maxdepth 1 -name "drmon-test.sfc" 2>/dev/null | head -1 || true)
    if [ -z "$ROM" ]; then
        ROM=$(find "$REPO_ROOT/roms/snes" "$ROM_DIR" -maxdepth 1 \( -name "*.sfc" -o -name "*.smc" \) 2>/dev/null | head -1 || true)
    fi
    if [ -z "$ROM" ]; then
        echo "No SNES ROM found in $ROM_DIR (run python3 $ROM_DIR/generate_test_roms.py) or $REPO_ROOT/roms/snes."
        exit 1
    fi
else
    # Explicitly match the vendored Genesis test ROM; avoid matching README.md or other docs.
    ROM=$(find "$ROM_DIR" -maxdepth 1 -name "drmon-test.md" 2>/dev/null | head -1 || true)
    if [ -z "$ROM" ]; then
        echo "No Genesis ROM found: $ROM_DIR/drmon-test.md"
        echo "Generate one: python3 $ROM_DIR/generate_test_roms.py"
        exit 1
    fi
fi

echo "=== test_bridge.sh: SYS=$SYS driver=$DRIVER ROM=$(basename "$ROM") ==="

# Start MAME headless in the background.
# SDL_VIDEODRIVER=offscreen prevents MAME from creating a real SDL window even
# with -video none (which suppresses game rendering but not the host window).
# SDL_AUDIODRIVER=dummy avoids PulseAudio/ALSA errors on headless hosts.
env SDL_VIDEODRIVER=offscreen SDL_AUDIODRIVER=dummy \
mame "$DRIVER" \
    -cart "$ROM" \
    -debug -debugger none \
    -autoboot_script "$LUA" \
    -video none \
    -nothrottle \
    &>/tmp/mame_bridge_test.log &
MAME_PID=$!

cleanup() {
    kill "$MAME_PID" 2>/dev/null || true
    wait "$MAME_PID" 2>/dev/null || true
    fuser -k "${PORT}/tcp" 2>/dev/null || true
}
trap cleanup EXIT

# Run the protocol test suite.
if [ "$SYS" = "gen" ]; then
    python3 "$SCRIPT_DIR/test_genesis_bridge.py"
else
    python3 "$SCRIPT_DIR/test_bridge.py" "$SYS"
fi

echo "=== test_bridge.sh: PASS ==="
