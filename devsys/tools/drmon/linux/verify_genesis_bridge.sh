#!/usr/bin/env bash
# verify_genesis_bridge.sh — drive the Genesis VDP/CRAM/VSRAM/Z80 Lua bridge end-to-end.
#
# Launches headless MAME (genesis) with mame_genesis_bridge.lua on :41817, waits for the
# bridge to listen, runs the test_genesis_bridge.py protocol suite, then tears down.
# This is step 3 of docs/plans/2026-06-13-genesis-non-cpu-state-vdp-cram-vsram-z80-lua-side.md.
#
# Usage:
#   bash linux/verify_genesis_bridge.sh [CART]
#     CART  path to a Genesis ROM (.md/.bin/.gen/.smd) or a .zip containing one.
#           Defaults to the first ROM/zip found in <repo>/roms/genesis/.
#
# Requires: mame on PATH (v0.277+), python3, unzip (only if CART is a .zip).
# Env: DRMON_GEN_BRIDGE_ADDR=host:port overrides the bridge listen address (default :41817).
#
# NOTE: MAME's Lua autoboot engine needs an interactive/desktop session — under a headless
# agent shell it dies with exit 144 (signal 16) and no output. Run this on your desktop.
set -euo pipefail

usage() { sed -n '2,18p' "$0" | sed 's/^# \{0,1\}//'; exit 0; }
[ "${1-}" = "-h" ] || [ "${1-}" = "--help" ] && usage

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../../../.." && pwd)"
LUA="$SCRIPT_DIR/mame_genesis_bridge.lua"
TEST="$SCRIPT_DIR/test_genesis_bridge.py"

_addr="${DRMON_GEN_BRIDGE_ADDR:-127.0.0.1:41817}"
PORT="${_addr##*:}"
# Guard against a bogus DRMON_GEN_BRIDGE_ADDR (e.g. "host" with no port, or ":0"): an
# empty/zero port makes MAME's bridge bind an ephemeral port, so the test can never find
# it on :41817.  Fall back to the default and warn.
if ! [[ "$PORT" =~ ^[0-9]+$ ]] || [ "$PORT" -eq 0 ]; then
    echo "WARNING: DRMON_GEN_BRIDGE_ADDR='${DRMON_GEN_BRIDGE_ADDR:-}' has no usable port; using 41817." >&2
    PORT=41817
    export DRMON_GEN_BRIDGE_ADDR="127.0.0.1:41817"
fi

# ── locate a ROM ────────────────────────────────────────────────────────────────
CART="${1:-}"
if [ -z "$CART" ]; then
    CART=$(find "$REPO_ROOT/roms/genesis" -maxdepth 1 \
        \( -iname '*.md' -o -iname '*.bin' -o -iname '*.gen' -o -iname '*.smd' -o -iname '*.zip' \) \
        2>/dev/null | sort | head -1 || true)
fi
if [ -z "$CART" ] || [ ! -f "$CART" ]; then
    echo "No Genesis ROM given or found in $REPO_ROOT/roms/genesis/."
    echo "Usage: bash $0 <rom.md|rom.zip>"
    exit 1
fi

TMPROM=""
if [[ "$CART" == *.zip ]]; then
    command -v unzip >/dev/null || { echo "unzip not found (needed for .zip ROM)"; exit 1; }
    TMPROM="$(mktemp -d)"
    unzip -o -j "$CART" -d "$TMPROM" >/dev/null
    ROM=$(find "$TMPROM" -maxdepth 1 \( -iname '*.md' -o -iname '*.bin' -o -iname '*.gen' -o -iname '*.smd' \) | sort | head -1 || true)
    [ -n "$ROM" ] || { echo "No ROM file inside $CART"; exit 1; }
else
    ROM="$CART"
fi

echo "=== verify_genesis_bridge.sh: ROM=$(basename "$ROM") bridge=:$PORT ==="

# Clear any stale bridge MAME / listener so the test gets a clean single-client socket.
# (Target only MAME instances running THIS bridge script, not other games the user has open.)
pkill -9 -f mame_genesis_bridge.lua 2>/dev/null || true
fuser -k "${PORT}/tcp" 2>/dev/null || true
sleep 0.5

# ── launch headless MAME with the bridge ────────────────────────────────────────
# -debugger none (not gdbstub): the machine free-runs so the Lua periodic pump services
#   the socket; gdbstub would halt at boot waiting for a client.  SDL_VIDEODRIVER=offscreen
#   keeps MAME from opening a host window even with -video none.
LOG="$(mktemp)"
env SDL_VIDEODRIVER=offscreen SDL_AUDIODRIVER=dummy \
    mame genesis \
        -cart "$ROM" \
        -debug -debugger none \
        -autoboot_script "$LUA" \
        -video none \
        -nothrottle \
        &>"$LOG" &
MAME_PID=$!
# Drop the job from the shell's table so the eventual SIGKILL in cleanup doesn't print a
# scary "<pid> Killed  env … mame …" line to the user's terminal after the test passes.
disown "$MAME_PID" 2>/dev/null || true

cleanup() {
    # MAME ignores SIGTERM during emulation, so SIGKILL it; the pkill is a backstop in case
    # $MAME_PID was a wrapper.  (-f match is safe here: this script's own argv does not
    # contain the lua filename, so pkill won't match itself.)
    kill -9 "$MAME_PID" 2>/dev/null || true
    pkill -9 -f mame_genesis_bridge.lua 2>/dev/null || true
    wait "$MAME_PID" 2>/dev/null || true
    fuser -k "${PORT}/tcp" 2>/dev/null || true
    [ -n "$TMPROM" ] && rm -rf "$TMPROM"
    rm -f "$LOG"
}
trap cleanup EXIT

# ── wait for the bridge to listen ───────────────────────────────────────────────
# Use a PASSIVE listen check (ss/netstat), never a probe connection: MAME's bridge
# socket accepts a single client, so a connect-probe would consume the very slot the
# test needs (and the listener won't reopen until EOF-timeout/BYE).
port_listening() {
    if command -v ss >/dev/null 2>&1; then
        ss -ltnH "sport = :$PORT" 2>/dev/null | grep -q LISTEN
    elif command -v netstat >/dev/null 2>&1; then
        netstat -ltn 2>/dev/null | grep -qE "[:.]$PORT[[:space:]]"
    else
        sleep 5; return 0   # no tool to check; give it time and let the test be the probe
    fi
}

echo "Waiting for bridge on :$PORT ..."
UP=0
for _ in $(seq 1 40); do
    if ! kill -0 "$MAME_PID" 2>/dev/null; then
        echo "FATAL: MAME exited before the bridge came up. MAME log:"
        echo "---------------------------------------------------------------"
        cat "$LOG"; echo "---------------------------------------------------------------"
        exit 1
    fi
    if port_listening; then UP=1; break; fi
    sleep 0.5
done

if [ "$UP" -ne 1 ]; then
    echo "FATAL: MAME is running but the bridge never opened :$PORT after 20s."
    echo "Most likely the autoboot Lua script errored. MAME log:"
    echo "---------------------------------------------------------------"
    cat "$LOG"; echo "---------------------------------------------------------------"
    exit 1
fi

# ── run the protocol suite ──────────────────────────────────────────────────────
python3 "$TEST" "127.0.0.1:$PORT"
echo "=== verify_genesis_bridge.sh: PASS ==="
