# Mesen2 and bsnes-plus vs drmon — killer features, gaps, and the path forward

**Date:** 2026-06-11
**Prompted by:** the [65816 gdbstub investigation](2026-06-11-65816-gdbstub.md)'s passing
claim that "for interactive debugging, Mesen2 and bsnes-plus native GUIs are already
excellent." Review asked: what exactly are their killer features, what would drmon have to
do to compete with each, and what's the recommended path?

## Verdict (TL;DR)

Their killer features are **emulator-integrated visualization** — frame-timeline event
viewers, live PPU/tile/sprite/palette viewers, profilers — which only an emulator that
*is* the debugger can do cheaply, plus zero-setup polish. drmon should not chase that;
rebuilding a GUI emulator suite inside a TUI attached over a socket is a losing game.
drmon's winnable ground is everything the GUI emulators structurally *can't* do:
**editor-first source-level debugging (the Phase 3 DAP play — neither competitor offers
in-editor debugging), SNES + Genesis in one tool (neither covers Genesis), an
emulator-agnostic backend, and terminal/remote/CI workflows.** One feature is
non-negotiable table stakes, though: **modern symbol import (ca65 `.dbg`, WLA-DX `.sym`)**
— without it drmon only serves the legacy spasm/`.sld` workflow and is invisible to
today's homebrew community. Timing is also unusually favorable: **Mesen2 was archived on
2026-06-04** (development moved to the young
[MesenCE](https://github.com/nesdev-org/MesenCE) fork) and **bsnes-plus hasn't shipped a
release since v05 (2019)** — the "already excellent" incumbents are both in maintenance
limbo.

## The contenders

### Mesen2 — the polished integrated suite

Multi-system emulator (NES, SNES, GB/GBA, PC Engine, SMS/GG, WonderSwan — **no Genesis**),
C++ core with C#/Avalonia UI, GPL-3, Windows/Linux/macOS. Archived 2026-06-04 at v2.1.1;
successor fork [MesenCE](https://github.com/nesdev-org/MesenCE) is active (pushed this
week) but small (~200 stars).

Debugger windows, enumerated from
[the source](https://github.com/SourMesen/Mesen2/tree/master/UI/Debugger/Windows):
Debugger (disassembly + stepping), Breakpoint editor, **Break In / Break On**
(run-to-target / break-on-event), Watch, Memory Tools + Memory Search + Find-all,
**Event Viewer** (per-frame timeline of register writes, IRQ/NMI, DMA), **Tile / Tilemap /
Sprite / Palette viewers** plus a **Tile Editor**, Register Viewer, **Profiler**
(function-level cycle counts), **Trace Logger**, **Assembler** (patch code live),
**Script window** (Lua API with memory callbacks and on-screen drawing), Label/Comment
editors, Debug Log.

Two structural killer features stand out:

1. **Symbol/source integration** —
   [importers](https://github.com/SourMesen/Mesen2/tree/master/UI/Debugger/Integration)
   for ca65/cc65 `.dbg` (true source-level view), **ELF**, WLA-DX, RGBDS, bass, SDCC,
   pceas, nesasm. Modern homebrew toolchains plug straight in.
2. **Co-processor breadth** — dedicated
   [debuggers](https://github.com/SourMesen/Mesen2/tree/master/Core/SNES/Debugger) for
   SPC700, SA-1, SuperFX/GSU, Cx4, NEC DSP, even the ST018 ARM core; the emulator core
   covers the full enhancement-chip zoo (SA-1, SDD-1, SPC7110, MSU-1, BS-X, …).

### bsnes-plus — the romhacker's workhorse

SNES-only fork of bsnes-classic, C++/Qt, GPL-2, Windows/Linux (macOS limited). Last
release v05 (August 2019); commits trickle (latest March 2025), 90+ open issues —
community interest without momentum.

Killer features ([README](https://github.com/devinacker/bsnes-plus)):

1. **Bus-level breakpoints with mirroring** — break on VRAM/OAM/CGRAM *accesses*, not just
   CPU addresses, with address-mirroring awareness across the whole map.
2. **Usage logging (CDL-style)** — live code/data classification of the ROM, "unexplored
   region" hunting, exportable usage maps that feed disassemblers — the reverse-engineering
   loop bsnes-plus is famous for.
3. **Co-processor debugging** — SA-1 and SuperFX disassembly/stepping with bus viewers and
   usage logging; SPC dumping/visualization.
4. Accuracy lineage from bsnes — behavior you debug is behavior you can trust.

## Compare and contrast

| | Mesen2 (→ MesenCE) | bsnes-plus | drmon today | drmon after Phase 2+3 |
|---|---|---|---|---|
| Systems | SNES + 6 others, **no Genesis** | SNES only | SNES + Genesis (UI builds; no target) | **SNES + Genesis, live** |
| Architecture | debugger *is* the emulator | debugger *is* the emulator | standalone TUI, stubbed link | standalone, **emulator-agnostic bridge** (MAME first) |
| Source-level | yes (ca65 `.dbg`, ELF, WLA-DX…) | symbols (WLA-DX style), weaker source view | yes — but legacy `.sld`/spasm only | yes; **add modern importers** (table stakes) |
| In-editor (DAP) | no | no | no | **yes — Phase 3, unique** |
| CPU debugging | step/bp/watch/trace, run-to | step/bp incl. bus bps | bp/conditional bp/watch/expr engine | same, over MAME native bp/step |
| Co-processors | SPC700, SA-1, GSU, Cx4, DSP, ST018 | SA-1, GSU, SPC | none | none initially (protocol reserves it; MAME enhancement-chip coverage needs per-cart verification) |
| PPU/VRAM visualization | full viewer suite + tile editor + event viewer | full viewer suite + usage map | none | **cede** — point at MAME's built-in graphics viewer |
| Profiler / trace logger | both | trace, usage logging | neither | not planned |
| Scripting/automation | Lua window | limited | `.scr` command scripts | `.scr` + scriptable protocol (CI-able headless MAME) |
| Remote/terminal use | no | no | yes (TUI) | yes — ssh-able, CI-able |
| Maintenance | archived; fork young | stalled since 2019 | active (this repo) | — |

Genesis context (since neither competitor plays there): the natives are
[Exodus](https://github.com/RogerSanders/Exodus) (Windows-only, heavyweight) and
[BlastEm](https://www.retrodev.com/blastem/)'s built-in 68k debugger + **GDB remote**
([gdb_remote.c](https://github.com/libretro/blastem/blob/libretro/gdb_remote.c)) — usable
from m68k-elf-gdb and even VS Code, though with gaps (no watchpoints). A genmon with
source-level debugging and a real UI would be competitive on Genesis **immediately** —
the field is much thinner than SNES.

## What drmon would have to do to compete head-on

**With Mesen2** (the full-suite bar): PPU/tile/sprite/palette viewers, event viewer,
profiler, trace logger, Lua, live assembler, six co-processor debuggers, and a GUI. That
is years of work replicating an emulator-integrated architecture drmon deliberately
doesn't have — and the parts people love most (event viewer, viewers) depend on being
*inside* the frame loop. Not the game to play. Partial substitution comes free from the
architecture instead: MAME's own debugger UI and built-in graphics/tile viewer run
*alongside* drmon on the same machine instance, so "drmon + MAME's viewers" covers a
useful slice of the suite without drmon writing a pixel.

**With bsnes-plus** (the RE-workflow bar): bus-level VRAM/OAM/CGRAM breakpoints (the
bridge could later back these with MAME watchpoints on the PPU spaces — protocol headroom
already reserved), usage/CDL logging (would need MAME-side execution tracing; significant),
SA-1/SuperFX stepping (gated on MAME's enhancement-chip emulation quality — verify
per-cart before promising anything). Realistic to approach incrementally; not Phase 2.

**Table stakes against both** — the one gap that actually disqualifies drmon today:
modern symbol formats. drmon's parsers speak 1994 (`.sld`, COFF, zardoz). Mesen2 reads
the output of every toolchain the community uses. A ca65 `.dbg` + WLA-DX `.sym` importer
pair in drmon-core (joining `sld.cpp`/`coff.cpp` behind the existing symbol layer) makes
drmon usable by every current SNES homebrew project — small, bounded, high-leverage.

## Recommended path forward

1. **Ship Phase 2 as planned** ([plan](../plans/2026-06-11-drmon-mame-backend.md)) — both
   consoles. Nothing else matters until drmon debugs a live target.
2. **Phase 3 (DAP) is the strategic differentiator, not a nice-to-have.** In-editor
   debugging (VS Code / nvim-dap) is the one modality neither Mesen2 nor bsnes-plus offers
   and structurally won't — their UI *is* their product. drmon-core behind cppdap makes
   "set a breakpoint in your editor, step through your SNES/Genesis source" a unique
   offering, with the incumbents in maintenance limbo.
3. **Add modern symbol importers (ca65 `.dbg`, WLA-DX `.sym`) to drmon-core** — table
   stakes; candidate TODO item (not yet filed, pending approval).
4. **Cede visualization.** Document MAME's built-in graphics viewer + debugger as the
   companion tools; revisit the reserved PPU/VDP protocol ops only when a concrete user
   need shows up (bus-breakpoints via MAME watchpoints would be the first, cheapest step
   into bsnes-plus territory).
5. **Watch the incumbents.** If MesenCE stumbles, the editor-first niche widens further;
   if it thrives, items 2–3 are still differentiated. Either way no plan change — just
   don't burn effort cloning features whose owners are a week-old fork and a 2019 release.

## Sources

- [Mesen2 repo](https://github.com/SourMesen/Mesen2) (archived 2026-06-04; v2.1.1) ·
  [debugger windows](https://github.com/SourMesen/Mesen2/tree/master/UI/Debugger/Windows) ·
  [symbol importers](https://github.com/SourMesen/Mesen2/tree/master/UI/Debugger/Integration) ·
  [SNES co-processor debuggers](https://github.com/SourMesen/Mesen2/tree/master/Core/SNES/Debugger) ·
  [MesenCE fork](https://github.com/nesdev-org/MesenCE)
- [bsnes-plus repo](https://github.com/devinacker/bsnes-plus) (v05 2019; last commit
  2025-03-22 via GitHub API)
- Genesis landscape: [BlastEm](https://www.retrodev.com/blastem/) ·
  [BlastEm gdb_remote.c](https://github.com/libretro/blastem/blob/libretro/gdb_remote.c) ·
  [genesis-code docs on gdb debugging](https://zerasul.github.io/genesis-code-docs/debug/) ·
  [Exodus](https://github.com/RogerSanders/Exodus)
- In-repo: drmon feature set per `snesmon.hlp`, `history.txt` (conditional breakpoints,
  watch, fill memory), `sld.cpp`/`coff.cpp`/`zardoz.cpp` (symbol parsers)
