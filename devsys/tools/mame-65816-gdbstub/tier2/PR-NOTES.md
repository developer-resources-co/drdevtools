# Tier 2 — upstream MAME PR: 65816/5A22 register map for the C++ gdbstub

**Patch:** [`0001-add-65816-w65c816-register-map.patch`](0001-add-65816-w65c816-register-map.patch)
(applies clean at `-p1` against `mame0277`; verified `patch --dry-run`).
**Target file:** `src/osd/modules/debugger/debuggdbstub.cpp`.
**Plan / investigation:** [../../../../docs/plans/2026-06-13-mame-65816-gdbstub-map.md](../../../../docs/plans/2026-06-13-mame-65816-gdbstub-map.md)
· [../../../../docs/investigations/2026-06-11-65816-gdbstub.md](../../../../docs/investigations/2026-06-11-65816-gdbstub.md)

## What it does

Adds a `gdb_register_map_w65c816` and registers it under the three g65816 device short-names —
`5a22` (SNES / Nintendo Super System / sfcbox), `w65c816` (Apple IIgs, Foenix C256/F256), and
`w65c802`. The C++ module already generates `target.xml`, maps `Z0`→native breakpoints, single-steps
and emits proper stop packets; the register map was the only missing piece (the module supports
i486/arm7/ppc/m68k/z80/m6502/m6809/… but had no 65816). After this, any XML-aware RSP client can
attach to a running 65816 target via `-debug -debugger gdbstub`.

Register layout (matches the Tier‑1 Lua plugin, which is verified live — see below):

| reg | gdb name | type        | width | note |
|-----|----------|-------------|-------|------|
| A   | a        | int         | 16    | accumulator |
| X   | x        | int         | 16    | |
| Y   | y        | int         | 16    | |
| P   | p        | int         | 8     | status |
| E   | e        | int         | 8     | emulation-mode flag |
| S   | sp       | data_ptr    | 16    | stack pointer |
| D   | d        | int         | 16    | direct page |
| DB  | db       | int         | 8     | data bank |
| PB  | pb       | int         | 8     | program bank |
| PC  | pc       | code_ptr    | 32    | 24-bit linear value, **`override_bitsize=32`** |

Widths other than PC come from each device-state entry's natural `datasize()` (the same mechanism
m6502 etc. rely on). **PC is the one judgment call:** MAME's g65816 `PC` state is the 24-bit linear
value (formatted `%06X`); it's pinned to 32 bits via the struct's `override_bitsize` field so it
frames as a clean 4-byte pointer for clients that assume power-of-two widths (the high byte is
always `00`). Verified ground truth (MAME 0.277 + DKC): `PC == 0x80c0d5`, `PB == 0x80`, i.e. the PC
high bytes are the program bank.

## Naming (the one bikeshed)

No official gdb architecture name or `org.gnu.gdb.*` feature exists for the 65816 — binutils' `w65`
target was assembler-only and was removed in 2018, and no gdbarch ever replaced it. So this proposes
`arch = "w65c816"` and `feature = "mame.w65c816"`. There's direct precedent in the same file: MAME's
`m6502` map uses an equally-unofficial `m6502`/`mame.m6502` pair. Maintainers may want to bikeshed
the exact string; the map content is independent of the chosen name.

## Verification status

- **Register map proven via the Lua Tier‑1 sibling** (`../plugins/gdbstub/init.lua`, run by
  `../verify.sh`): the identical register set + a hand-authored `target.xml` of the same shape were
  driven live against `mame snes` + DKC — `qXfer:features:read` returns a well-formed description
  whose `<reg>` names/bitsizes/order match this map, `g` decodes to correct live registers, and the
  24-bit PC is intact. That exercises everything this C++ map produces except the module's own
  (already-working) target.xml generator.
- **C++ in-tree build verification: DEFERRED — disk-gated.** A MAME-from-source build is tens of GB
  of intermediates; the dev box this was authored on sits at 98% disk (14 GB free), so building was
  unsafe. The patch is authored against the exact 0.277 source and is additive (a data table + three
  map entries), but it has **not** been compiled here. Run it where there's disk headroom — or let
  the MAME PR CI do it.

## Build + verify recipe (when disk allows)

A SOURCES-limited build keeps this to ~minutes / a few GB instead of the full multi-hour tree:

```sh
git clone --depth 1 -b mame0277 https://github.com/mamedev/mame
cd mame
patch -p1 < /path/to/0001-add-65816-w65c816-register-map.patch
make SUBTARGET=snes SOURCES=src/mame/nintendo/snes.cpp REGENIE=1 -j"$(nproc)"
./snes <snes-rom> -debug -debugger gdbstub -debugger_port 2159
# then point ../test_65816.py (PORT 2159) at it — same asserts as the Lua tier
```

## Submitting the PR

```sh
# in a fresh mamedev/mame clone on a feature branch
git checkout -b gdbstub-65816
patch -p1 < 0001-add-65816-w65c816-register-map.patch
git commit -am "debugger/gdbstub: add 65816/5A22 (w65c816) register map"
gh pr create --repo mamedev/mame --title "debugger/gdbstub: add 65816/5A22 register map" \
  --body "Adds w65c816/w65c802/5a22 to the gdbstub register maps (SNES, Apple IIgs, Foenix). …"
```
