#!/usr/bin/env bash
# test_bridge.sh — launch headless MAME, run the protocol test suite, clean up.
# Usage: bash linux/test_bridge.sh [snes|gen]
# Requires: mame on PATH; called by 'task test-bridge' inside the drmon build container.
# SNES: mame_bridge.lua on 127.0.0.1:41816 (or $DRMON_MAME_ADDR).
# GEN:  MAME -debugger gdbstub on 127.0.0.1:23946 (or $DRMON_GDB_ADDR).
set -euo pipefail

usage() { echo "Usage: $0 [snes|gen]"; exit 0; }
[ "${1-}" = "-h" ] || [ "${1-}" = "--help" ] && usage

SYS="${1:-snes}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LUA="$SCRIPT_DIR/mame_bridge.lua"

case "$SYS" in
  snes) DRIVER="snes" ;;
  gen)  DRIVER="genesis" ;;
  *)    echo "Unknown SYS '$SYS'; use snes or gen"; exit 1 ;;
esac

# Port used by this backend; kill any stale MAME that still holds it.
if [ "$SYS" = "gen" ]; then
    PORT="${DRMON_GDB_ADDR##*:}"
    PORT="${PORT:-23946}"
else
    PORT="${DRMON_MAME_ADDR##*:}"
    PORT="${PORT:-41816}"
fi

# Kill any stale process holding the port so the test gets a clean socket.
fuser -k "${PORT}/tcp" 2>/dev/null || true
sleep 0.2

# Locate a test ROM.
REPO_ROOT="$(cd "$SCRIPT_DIR/../../../.." && pwd)"
ROM_DIR="$SCRIPT_DIR/test-roms"

if [ "$SYS" = "snes" ]; then
    ROM=$(find "$REPO_ROOT/roms/snes" "$ROM_DIR" -maxdepth 1 \( -name "*.sfc" -o -name "*.smc" \) 2>/dev/null | head -1 || true)
    if [ -z "$ROM" ]; then
        echo "No SNES ROM found in $REPO_ROOT/roms/snes or $ROM_DIR (*.sfc / *.smc)."
        echo "Generate one: python3 $ROM_DIR/generate_test_roms.py"
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
if [ "$SYS" = "gen" ]; then
    mame "$DRIVER" \
        -cart "$ROM" \
        -debug -debugger gdbstub \
        -debugger_port "$PORT" \
        -video none \
        -nothrottle \
        &>/tmp/mame_bridge_test.log &
else
    mame "$DRIVER" \
        -cart "$ROM" \
        -debug -debugger none \
        -autoboot_script "$LUA" \
        -video none \
        -nothrottle \
        &>/tmp/mame_bridge_test.log &
fi
MAME_PID=$!

cleanup() {
    kill "$MAME_PID" 2>/dev/null || true
    wait "$MAME_PID" 2>/dev/null || true
    fuser -k "${PORT}/tcp" 2>/dev/null || true
}
trap cleanup EXIT

# Run the protocol test suite.
if [ "$SYS" = "gen" ]; then
    python3 "$SCRIPT_DIR/test_gdb.py"
else
    python3 "$SCRIPT_DIR/test_bridge.py" "$SYS"
fi

echo "=== test_bridge.sh: PASS ==="
