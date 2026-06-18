#!/usr/bin/env bash
# make-fixture.sh — (re)generate the Phase-B/C DWARF fixtures:
#   a16local.sfc       — the SNES ROM (what MAME runs)
#   a16local.sfc.elf   — the DWARF companion (what drmon loads for symbols)
#
# KEY FACT: llvm-mos's ld.lld, when the platform link.ld ends with the custom
# `OUTPUT_FORMAT { FULL(rom) }`, writes BOTH the flat ROM (-o foo.sfc) AND a full
# ELF-with-DWARF companion at `<output>.elf` (foo.sfc.elf). So a normal `-g` build
# already emits the debug ELF — nothing special is needed. The .sfc and .sfc.elf
# come from the SAME link, so their addresses are identical by construction (this
# is the address-consistency the Phase-C end-to-end test relies on).
#
# Both outputs are COMMITTED so the loader/e2e tests run without the llvm-mos tree.
# Set MOS_ROOT to point at a different llvm-mos-65816 checkout if needed.
set -euo pipefail

usage() { echo "Usage: $0   # regenerate a16local.sfc + a16local.sfc.elf (needs a built llvm-mos-65816)"; exit 0; }
[ "${1-}" = "-h" ] || [ "${1-}" = "--help" ] && usage

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DRDEV_ROOT="$(cd "$HERE/../../../../.." && pwd)"
# llvm-mos-65816 is assumed a sibling of the drdevtools repo; override with MOS_ROOT.
MOS_ROOT="${MOS_ROOT:-$DRDEV_ROOT/../llvm-mos-65816}"
TOOL="$MOS_ROOT/build/llvm-mos-install/bin"
CFG="$MOS_ROOT/build/install/bin/mos-snes.cfg"

for p in "$TOOL/mos-clang" "$CFG"; do
    [ -e "$p" ] || { echo "FATAL: missing $p — build llvm-mos-65816 first (dev/run.sh toolchain + build), or set MOS_ROOT"; exit 1; }
done

# Build IN the fixture dir with relative source + -fdebug-compilation-dir=. so the
# committed ELF's DWARF paths are clean (DW_AT_comp_dir=".", name="a16local.c") — no
# homedir leak, byte-reproducible across checkouts.
echo "==> build a16local.sfc (+ .sfc.elf DWARF companion) — LTO, -g"
( cd "$HERE" && "$TOOL/mos-clang" --config "$CFG" -mcpu=mosw65816 \
    -Xclang -target-feature -Xclang +mos-a16 -g -Os \
    -fdebug-compilation-dir=. -ffile-prefix-map="$HERE"=. \
    -o a16local.sfc a16local.c )

echo "==> verify DWARF companion"
"$TOOL/llvm-dwarfdump" --verify "$HERE/a16local.sfc.elf" >/dev/null && echo "    --verify OK"
"$TOOL/llvm-nm" "$HERE/a16local.sfc.elf" | grep -iE ' main$| _start$' || true
echo "Wrote $HERE/a16local.sfc ($(stat -c%s "$HERE/a16local.sfc") bytes) + a16local.sfc.elf ($(stat -c%s "$HERE/a16local.sfc.elf") bytes)"
echo "    (addresses: main \$8059, line 13 \$805b, line 17 \$8074 — used by test_symbols.py / test_dap.py)"
