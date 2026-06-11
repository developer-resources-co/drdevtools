# TODO

drmon = the Developer Resources Monitor, a source-level debugger being brought from 1990s DOS to Linux.
See [docs/plans/2026-06-10-port-drmon-linux.md](docs/plans/2026-06-10-port-drmon-linux.md)
for the phased roadmap.

**Status markers:** `[ ]` open · `[wip]` in progress · `[verify]` implemented, verification
not yet run+recorded (run the linked plan's verification steps, paste raw output + PASS/FAIL
back into the plan, then promote to `[x]`) · `[x]` done (moved to DONE, one tight line).
Plan-first: non-trivial work gets a `docs/plans/YYYY-MM-DD-<topic>.md` and a TODO entry.


## DRMON — DEBUGGER BACKEND

- [ ] **Genesis non-CPU state (VDP/CRAM/VSRAM/Z80) still stubbed.** genmon's transport is
  MAME's native GDB RSP, which only reaches the M68K bus — VDP VRAM/CRAM/VSRAM and the Z80
  are unreachable there, so `ReadSlaveVDP`/`ReadSlaveCRAM`/`ReadSlaveVSRAM` stay zero-stubbed
  in `sliogdb.cpp`. Recommended approach: a **side-channel Lua companion socket** alongside
  the gdbstub — keep GDB RSP for M68K/step/breakpoints, add a small `-autoboot_script` Lua
  opening a 2nd socket for `:vdp`/`:z80snd` device reads (the SNES bridge proves the
  device-access path). Deferred from the
  [SNES stub-lift](docs/plans/2026-06-11-lift-snes-out-of-scope-stubs.md); revisit on concrete need.
- [ ] **SPC700 (SNES audio CPU) debugging — needs a UI surface first.** MAME-side wiring is
  trivial (`:soundcpu` exposes program/data spaces, regs `PC S P A X Y`, and the `:aram` 64K
  RAM share — confirmed by the spike), but there is *no* UI consumer in the Linux build (only
  protocol enums, gated behind `#ifdef SPC700`). Build a CPU-selector / SPC register + APU-RAM
  window, then wire the (easy) bridge read path.
  [plan](docs/plans/2026-06-11-lift-snes-out-of-scope-stubs.md).
- [wip] **Phase 3 — DAP front end Tier 1 built; Tiers 2–3 remain.** `drmon-dap-snes` / `drmon-dap-gen`
  DAP adapters ship alongside `snesmon`/`genmon` (two binaries in parallel; TUI stays indefinitely).
  Tier 1 (done 2026-06-12): initialize/attach/continue/pause/step/registers/readMemory/instruction
  breakpoints; SNES 65816 step engine with correct branch targets. Tier 2: disassembly view (wire
  `dis816.cpp` via a stub preamble). Tier 3: symbol file loading (`.sld`/COFF). Verification items
  3–8 (connected session, breakpoint, registers, memory, disassembly, regression) need live MAME.
  [plan](docs/plans/2026-06-12-phase-3-dap.md) (see also plan in `~/.claude/plans/`).


## DRMON — UI / UX

- [ ] **Support multiple "monitors"** (as windows)

## DRMON — CLEANUP

- [ ] **Finish dropping Borland / Watcom / OS2 / DOS-extender support.** `compat.hpp` is
  Linux-only now, but legacy `#ifdef __BORLANDC__` / `__WATCOMC__` / `__OS2__` / `DOSX286` /
  `__MSDOS__` arms remain scattered across the tree. Strip them as files are touched so the
  source carries a single modern/Linux path.


## DRMON — DOCS

_(none)_


## DRMON — INVESTIGATIONS

- [ ] **IDA Pro + Ghidra 65816 RSP against MAME gdbstub — get real answers.**
  [idapro_m6502](https://github.com/LucienMP/idapro_m6502) shows IDA's gdb client consuming MAME gdbstub XML for m6502.
  Verify the same pipeline for 65816: does a 65816 processor module exist for IDA; does IDA's gdb client
  work with a hand-authored `target.xml`; does Ghidra's RSP debugger path connect without a custom
  agent? Needs the MAME Lua 65816 map (Tier 1 of [65816 gdbstub investigation](docs/investigations/2026-06-11-65816-gdbstub.md))
  as the test endpoint. Write findings to `docs/investigations/`.
- [ ] **Upstream MAME 65816/5A22 register map to `debuggdbstub.cpp`** (~35 lines; prove via Lua plugin
  first; propose arch name `w65c816`). See [65816 gdbstub investigation](docs/investigations/2026-06-11-65816-gdbstub.md).
- [ ] **Add ca65 `.dbg` + WLA-DX `.sym` symbol importers to drmon-core** — table stakes for the current
  homebrew community (every active SNES toolchain outputs one of these; drmon today speaks only `.sld`/COFF/zardoz).
  Join `sld.cpp`/`coff.cpp` behind the existing symbol layer. See [competitive analysis](docs/investigations/2026-06-11-mesen2-bsnes-plus-vs-drmon.md).


## PACKAGING

_(none)_


## VERIFY
### implemented; run the plan's verification steps + record, then promote to DONE

_(none)_


## DONE

- [x] 2026-06-12 — Revive `EvalCommand`: the whole command-verb dispatcher (OPEN/CLOSE/RUN/STOP/STEP/OVER/RESET/BSET/SET/LOAD/SAVE/macros/…) shipped `#if 0`'d since the 2003 DOS import, so every typed command + script was a no-op; un-`#if 0`'d `EvalCommand`+`GetNumbers`, routed expr/number args through the live flex/bison `DoExp` (not the obsolete hand-written eval); zero signature drift; `OPEN MEMORY`/`OPEN SEARCHLIST` open windows, smoke clean — [plan](docs/plans/2026-06-12-revive-evalcommand.md)
- [x] 2026-06-12 — Revive scrollable mem-search results window: hits land in a `_searchList` list-rect window (mirrors symbol/break), Enter/Ctrl-G → Memory window at the hit, Del clears; `OpenMemoryAt()` helper; build + smoke clean (visual = manual via MemOps→Search) — [plan](docs/plans/2026-06-12-search-results-window.md)
- [x] 2026-06-12 — Package drmon as `drmon_1.0.1_amd64.deb` (snesmon + genmon, cmake/debhelper, lintian-clean PIE ELFs) in foundry-apt — [plan](docs/plans/2026-06-12-package-drmon.md)
- [x] 2026-06-11 — Lift SNES out-of-scope stubs: `ReadSlavePPU` reads VRAM via MAME save-item (`RP`), write-protect/BRK-on-write → MAME watchpoints (`WP±`/`BW±`), ROM-write warning (readback compare), client-side mem-search (MemOps→Search…); also fixed a latent `mame_cmd` reply stack-overflow; 24/24 bridge tests — [plan](docs/plans/2026-06-11-lift-snes-out-of-scope-stubs.md)
- [x] 2026-06-11 — Phase 2 — MAME backend: snesmon (Lua bridge :41816) + genmon (GDB RSP gdbstub); 19/19 SNES + 11/11 GEN protocol tests; EOF recovery, orphan immunity, disconnected TUI, fresh-clone all verified — [plan](docs/plans/2026-06-11-drmon-mame-backend.md)
- [x] 2026-06-11 — Genesis target (SYSTEM=GEN) bring-up — folded into Phase 2 MAME backend — [plan](docs/plans/2026-06-11-drmon-mame-backend.md)
- [x] 2026-06-11 — drmon user docs (manual + install + config) under `devsys/tools/drmon/docs/`, seeded from `snesmon.hlp` but reconciled against source (keys/commands/expr verified vs `monkeys.hpp`/`command.cpp`/`expr.*`); target-dependent features flagged Phase 2 — [plan](docs/plans/2026-06-10-drmon-user-manual-installation-configuration-docs.md)
- [x] 2026-06-10 — Viewport fill: size the CGA screen to the terminal's COLS×LINES (chrome fills, windows fixed) + live `KEY_RESIZE` re-fill; root-caused the width≥83 garble/crash to a dormant 64-bit `CopyScreen`/`CopyMem` `len/=4` long-copy (2× over-copy) — fixed w/ `memcpy`; ASan now on by default — [plan](docs/plans/2026-06-10-drmon-viewport-fill.md) · [BUGS.md](docs/BUGS.md)
- [x] 2026-06-10 — Handle Ctrl+C + restore terminal on signal-death (port gap: DOS `ctrlbrk` was never ported): ignore SIGINT (DOS parity, drmon keeps running) + emergency terminal-restore handler for SIGSEGV/etc. so a crash/kill no longer leaks xterm mouse modes; `task signals` regression guard
- [x] 2026-06-10 — Fix Alt+E (and Alt+K / file requester) SIGSEGV: string gadgets edited their text in place but were seeded with a read-only string literal (writable on 1990s DOS compilers); add `SetGadgString` writable-buffer helper + `task smoke` regression guard — [BUGS.md](docs/BUGS.md)
- [x] 2026-06-10 — Fix stray window-number `1` on dropdown menus (~30-yr-dormant cosmetic bug): ctor painted auto-assigned `windowNum` before `menu.cpp` could zero it; now passed to ctor — [BUGS.md](docs/BUGS.md)
- [x] 2026-06-10 — Wire the mouse (ncurses `getmouse`/`MEVENT`): clicks + drag + hover-highlight; root cause of dead hover was no terminfo `XM` → force xterm 1003h/1006h; verified live — [plan](docs/plans/2026-06-10-drmon-mouse.md)
- [x] 2026-06-10 — Convert all CP437 high bytes → UTF-8-safe (`\xNN` literals / UTF-8 comments), 14 files; verified no-op (binary differs only in build timestamp+build-id) — [plan](docs/plans/2026-06-10-cp437-ascii-conversion.md)
- [x] 2026-06-10 — Fix borders: force UTF-8 locale + restore menu.cpp's Edit-corrupted CP437 border string; menus/About render correct box-drawing
- [x] 2026-06-10 — Wire Alt-combos (alt-Q/X/M/…) + wide-ncurses Unicode box-drawing (CP437→Unicode); fixes ACS-fallback borders
- [x] 2026-06-10 — Phase 1.5: drmon **runs** on Linux — ncurses TUI (menu bar, windows, status line, clock) + keyboard input; disconnected (no target) — [plan](docs/plans/2026-06-10-port-drmon-linux.md)
- [x] 2026-06-10 — Phase 1: drmon **compiles + links** on Linux (SYSTEM=SNES) via Dockerized g++/CMake; 52 TUs → x86-64 ELF; verification recorded — [plan](docs/plans/2026-06-10-port-drmon-linux.md)
- [x] 2026-06-10 — Add Taskfile (`task image/build/run/debug/shot/clean/rebuild`) capturing the Dockerized workflow
- [x] 2026-06-10 — Post SourceForge "moved to GitHub" notice (project status `moved` + description) — [text](docs/sourceforge-notice.md)
- [x] 2026-06-10 — Migrate drdevtools CVS → [github.com/developer-resources-co/drdevtools](https://github.com/developer-resources-co/drdevtools) (cvs-fast-export, history preserved, public); verification recorded — [plan](docs/plans/2026-06-10-migrate-cvs-to-github.md)
- [x] 2026-06-10 — Investigate hosting: SourceForge-only, no prior GitHub migration — [investigation](docs/investigations/2026-06-10-hosting-sourceforge-vs-github.md)
