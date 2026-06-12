#!/usr/bin/env bash
#
# Regression check for WLA-DX issue #619 (ROM use reported over 100%).
#
# Assembles main.s with a full-ROM .BACKGROUND plus an OVERWRITE section and
# fails if the verbose ROM-usage summary reports any figure above 100%.
#
# Usage:
#   ./verify.sh                 # expects wla-z80 on PATH
#   WLA=/path/to/wla-z80 ./verify.sh
#
# Exit 0 = no over-100% line (patched / fixed). Exit 1 = bug present.

set -euo pipefail

show_help() {
  sed -n '2,12p' "$0" | sed 's/^# \{0,1\}//'
  exit 0
}
case "${1:-}" in -h|--help) show_help;; esac

WLA="${WLA:-wla-z80}"
cd "$(dirname "$0")"

if ! command -v "$WLA" >/dev/null 2>&1; then
  echo "verify.sh: '$WLA' not found on PATH (set WLA=/path/to/wla-z80)" >&2
  exit 2
fi

# 32 KB background = the full 2-bank ROM. Content is irrelevant; .BACKGROUND
# marks the whole region as used regardless of the bytes.
head -c 32768 /dev/zero > background.bin

# -v makes the assembler print the ROM-usage summary we want to inspect.
out="$("$WLA" -v -o main.o main.s 2>&1)"
echo "$out"

# Every usage line looks like: "... (NN.NN%) used)". Flag any value > 100.00.
over="$(printf '%s\n' "$out" \
  | grep -oE '\(([0-9]+\.[0-9]+)%\) used' \
  | grep -oE '[0-9]+\.[0-9]+' \
  | awk '$1 > 100.0 { print; found=1 } END { exit !found }' || true)"

rm -f background.bin main.o

if [ -n "$over" ]; then
  echo "FAIL: ROM usage reported over 100% -> $over (issue #619 present)" >&2
  exit 1
fi

echo "PASS: ROM usage never exceeds 100% (issue #619 fixed)"
