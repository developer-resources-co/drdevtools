#!/usr/bin/env bash
# Launch MAME with the repo-local 65816 gdbstub Lua, exposing a GDB RSP socket on
# 127.0.0.1:2159. Fully headless (no X / no display): mirrors drmon's bridge launch,
# i.e. -video none + -autoboot_script. The autoboot shim (gdbstub_autoboot.lua) loads the
# SAME protocol logic as the installable plugin (plugins/gdbstub/init.lua), so there's one
# source of truth. (-window crashes MAME's GL init on some headless hosts; bare -video none
# without an autoboot script exits immediately — the periodic the shim registers keeps it live.)
#
# Usage: run-mame.sh [SYSTEM] [CART]
#   SYSTEM defaults to "snes"; CART defaults to the DKC test ROM in the repo.
#
# To use the *installable plugin form* instead (needs a display), drop plugins/gdbstub over
# MAME's pluginspath and run: mame snes -cart ROM -debug -debugger none -plugin gdbstub -window
set -euo pipefail

usage() { sed -n '2,13p' "$0"; exit 0; }
[[ "${1:-}" == "-h" || "${1:-}" == "--help" ]] && usage

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/../../.." && pwd)"

SYSTEM="${1:-snes}"
CART="${2:-$REPO/roms/snes/Donkey Kong Country (U) (V1.2).smc}"
MAME="${MAME:-/usr/games/mame}"
OUT="${OUT:-/tmp/mame-65816-out}"
mkdir -p "$OUT"

if [[ ! -f "$CART" ]]; then
	echo "ROM not found: $CART" >&2
	echo "Pass a cartridge path as arg 2." >&2
	exit 1
fi

# The autoboot shim reads this to dofile the plugin's protocol logic.
export GDBSTUB_INIT="$HERE/plugins/gdbstub/init.lua"

# -debug enables the debugger core; -debugger none suppresses the interactive UI
# (same combo drmon's SNES bridge uses). -video none + -autoboot_script = stable headless.
exec "$MAME" "$SYSTEM" -cart "$CART" \
	-debug -debugger none \
	-autoboot_script "$HERE/gdbstub_autoboot.lua" \
	-skip_gameinfo -video none -sound none -nothrottle \
	-homepath "$OUT" -cfg_directory "$OUT" -nvram_directory "$OUT" \
	-snapshot_directory "$OUT" -diff_directory "$OUT" -comment_directory "$OUT"
