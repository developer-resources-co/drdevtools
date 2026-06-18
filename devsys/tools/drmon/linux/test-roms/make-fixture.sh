#!/usr/bin/env bash
# make-fixture.sh — (re)generate a16local-debug.elf: a real llvm-mos SNES `-g`
# ELF with DWARF, used as the committed fixture for the drmon ELF/DWARF loader
# (dap/test_symbols.py) and the Phase-C end-to-end test.
#
# WHY a hand-rolled link: the SNES platform's link.ld ends with llvm-mos's custom
# `OUTPUT_FORMAT { FULL(rom) }`, which makes ld.lld emit only the flat .sfc ROM —
# the DWARF is computed but never written to disk. We recover a DWARF-bearing ELF
# by compiling NON-LTO (`-c -g`, so the object carries real machine code + DWARF)
# and linking with that OUTPUT_FORMAT block stripped. Real $8000 LoROM addresses;
# `main` at $802f, line 13 (`t = a16v + b16v`) at $8031.
#
# The resulting .elf is COMMITTED so the loader test runs without the llvm-mos
# tree. Set MOS_ROOT to point at a different llvm-mos-65816 checkout if needed.
set -euo pipefail

usage() { echo "Usage: $0   # regenerate test-roms/a16local-debug.elf (needs a built llvm-mos-65816)"; exit 0; }
[ "${1-}" = "-h" ] || [ "${1-}" = "--help" ] && usage

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DRDEV_ROOT="$(cd "$HERE/../../../../.." && pwd)"
# llvm-mos-65816 is assumed a sibling of the drdevtools repo; override with MOS_ROOT.
MOS_ROOT="${MOS_ROOT:-$DRDEV_ROOT/../llvm-mos-65816}"
TOOL="$MOS_ROOT/build/llvm-mos-install/bin"
SNESLIB="$MOS_ROOT/build/install/mos-platform/snes/lib"
COMMONLIB="$MOS_ROOT/build/install/mos-platform/common/lib"

for p in "$TOOL/mos-clang" "$TOOL/ld.lld" "$SNESLIB/link.ld"; do
    [ -e "$p" ] || { echo "FATAL: missing $p — build llvm-mos-65816 first (dev/run.sh toolchain + build), or set MOS_ROOT"; exit 1; }
done

SRC="$HERE/a16local.c"
OBJ="$(mktemp /tmp/a16local-XXXX.o)"
ELFLD="$(mktemp /tmp/link-elf-XXXX.ld)"
OUT="$HERE/a16local-debug.elf"
trap 'rm -f "$OBJ" "$ELFLD"' EXIT

echo "==> compile (non-LTO, -g) $SRC"
# -fdebug-compilation-dir=. keeps the committed ELF's DWARF paths relative (no homedir leak).
( cd "$HERE" && "$TOOL/mos-clang" --target=mos -mcpu=mosw65816 \
    -Xclang -target-feature -Xclang +mos-a16 -g -Os \
    -fdebug-compilation-dir=. -c a16local.c -o "$OBJ" )

echo "==> strip OUTPUT_FORMAT{FULL(rom)} from link.ld → ELF output"
awk 'BEGIN{p=1} /^OUTPUT_FORMAT \{/{p=0} p{print}' "$SNESLIB/link.ld" > "$ELFLD"

echo "==> link → $OUT"
"$TOOL/ld.lld" --gc-sections --sort-section=alignment \
    -L"$SNESLIB" -L"$COMMONLIB" \
    -l:crt0.o "$OBJ" -lcrt0 -lcrt -lc \
    -T "$ELFLD" -o "$OUT"

echo "==> verify DWARF"
"$TOOL/llvm-dwarfdump" --verify "$OUT" >/dev/null && echo "    --verify OK"
"$TOOL/llvm-nm" "$OUT" | grep -iE ' main$| _start$' || true
echo "Wrote $OUT ($(stat -c%s "$OUT") bytes)"
