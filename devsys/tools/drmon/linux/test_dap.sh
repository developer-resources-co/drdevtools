#!/usr/bin/env bash
# test_dap.sh — live-MAME DAP verifications (Phase 3, V3–V6).
#
# Brings up headless MAME + the SNES Lua bridge on the host, then runs
# dap/test_dap.py *inside the build container* (--network=host, so the
# in-container drmon-dap-snes reaches the host bridge on :41816), and tears
# MAME down on exit. Mirrors test_bridge.sh's MAME launch; differs in that the
# client is the DAP adapter, not a raw bridge client.
#
# Usage: bash linux/test_dap.sh         # (host; needs mame on PATH + a built drmon-dap-snes)
#   or:  task test-dap
set -euo pipefail

usage() { echo "Usage: $0 [phasec]   # default: V3-V6 vs drmon-test.sfc; phasec: end-to-end source breakpoint vs a16local.sfc + DWARF"; exit 0; }
[ "${1-}" = "-h" ] || [ "${1-}" = "--help" ] && usage

MODE="${1:-v3v6}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../../../.." && pwd)"
IMAGE="drmon-build"
BUILD="/tmp/drmon-build"
LUA="$SCRIPT_DIR/mame_bridge.lua"

# Phase C (end-to-end DWARF round-trip): MAME runs a16local.sfc; the DAP loads the
# matching debug ELF (a16local-debug.elf) and sets a SOURCE breakpoint. Both ROM and
# ELF come from make-fixture.sh (same object, same link layout) so addresses agree.
if [ "$MODE" = "phasec" ]; then
    ROM="$SCRIPT_DIR/test-roms/a16local.sfc"
    # The DWARF companion ld.lld auto-emits beside the ROM (same link → same addrs).
    SYMBOLS_CONTAINER="/src/devsys/tools/drmon/linux/test-roms/a16local.sfc.elf"
else
    ROM="$SCRIPT_DIR/test-roms/drmon-test.sfc"
    SYMBOLS_CONTAINER=""
fi

_addr="${DRMON_MAME_ADDR:-127.0.0.1:41816}"
PORT="${_addr##*:}"
export DRMON_MAME_ADDR="127.0.0.1:$PORT"
export DRMON_BRIDGE_DIR="$SCRIPT_DIR"

[ -f "$ROM" ] || { echo "FATAL: missing $ROM (run python3 $SCRIPT_DIR/test-roms/generate_test_roms.py)"; exit 1; }
[ -x "$BUILD/drmon-dap-snes" ] || { echo "FATAL: no drmon-dap-snes in $BUILD (run: task build)"; exit 1; }

# Clean any stale port holder.
fuser -k "${PORT}/tcp" 2>/dev/null || true
sleep 0.2

echo "=== test_dap.sh: launching headless MAME (snes, $(basename "$ROM")) ==="
# -skip_gameinfo is REQUIRED headless: without it MAME stalls on the disclaimer
# screen, emulated time never advances, and the autoboot bridge never binds its
# socket. -sound none + scratch cfg/nvram dirs keep a headless/agent shell clean.
SCRATCH=/tmp/mame-scratch-dap; mkdir -p "$SCRATCH"
# NOTE: run THROTTLED (no -nothrottle). With -nothrottle MAME pegs a core and
# starves the Lua bridge's per-frame socket accept/handshake, so the adapter's
# attach-time connect() hangs. Throttled (60fps) the bridge services the socket
# reliably — verified: attach succeeds throttled, hangs under -nothrottle.
env SDL_VIDEODRIVER=offscreen SDL_AUDIODRIVER=dummy \
mame snes -cart "$ROM" \
    -debug -debugger none \
    -autoboot_script "$LUA" -skip_gameinfo \
    -video none -sound none \
    -cfg_directory "$SCRATCH" -nvram_directory "$SCRATCH" \
    &>/tmp/mame_dap_test.log &
MAME_PID=$!

cleanup() {
    # SIGTERM then SIGKILL the whole MAME process tree — MAME can ignore a plain
    # TERM under -debug, which is how earlier runs leaked headless instances.
    kill "$MAME_PID" 2>/dev/null || true
    sleep 0.5
    kill -9 "$MAME_PID" 2>/dev/null || true
    wait "$MAME_PID" 2>/dev/null || true
}
trap cleanup EXIT

# Settle wait — NOT a probe. The bridge serves ONE client and only reopens its
# listener ~5 s after a peer drops, so a readiness probe that connects-and-drops
# would steal the slot and the DAP would hit a dead reopen window. Instead give
# MAME a few seconds to boot + autoboot-bind (~1.5 s observed), then let the DAP
# be the first and only client.
echo "=== settling 5s for MAME boot + bridge bind on :$PORT ==="
sleep 5
if ! kill -0 "$MAME_PID" 2>/dev/null; then
    echo "FATAL: MAME exited during boot; log:"; tail -20 /tmp/mame_dap_test.log; exit 1
fi

echo "=== running dap/test_dap.py ($MODE) in container (--network=host) ==="
docker run --rm --network=host \
    -v "$REPO_ROOT:/src" -v "$BUILD:/build" \
    -w /src/devsys/tools/drmon \
    -e DRMON_DAP_SNES=/build/drmon-dap-snes \
    -e DRMON_MAME_ADDR="$DRMON_MAME_ADDR" \
    -e DRMON_DAP_SYMBOLS="$SYMBOLS_CONTAINER" \
    "$IMAGE" python3 linux/dap/test_dap.py
