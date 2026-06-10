# TODO

drmon = the Developer Resources Monitor, a source-level debugger being brought from 1990s DOS to Linux.
See [docs/plans/2026-06-10-port-drmon-linux.md](docs/plans/2026-06-10-port-drmon-linux.md)
for the phased roadmap.

**Status markers:** `[ ]` open · `[wip]` in progress · `[verify]` implemented, verification
not yet run+recorded (run the linked plan's verification steps, paste raw output + PASS/FAIL
back into the plan, then promote to `[x]`) · `[x]` done (moved to DONE, one tight line).
Plan-first: non-trivial work gets a `docs/plans/YYYY-MM-DD-<topic>.md` and a TODO entry.


## DRMON — DEBUGGER BACKEND

- [ ] **PPU/CRAM/OBJRAM reads return zeros.** `ReadSlavePPU` is stubbed safe
  (zeros, no crash); real PPU/CRAM/OBJ reads need the MAME backend (Phase 2+).
- [ ] **SPC700 ops are stubbed / no-op.** Audio CPU (SPC700) memory and register
  ops are not wired; needs Phase 2 MAME bridge.
- [ ] **Write-protect and BRK-on-write are no-ops.** The stub accepts the protocol
  command but doesn't enforce protection; needs a live target (Phase 2+).
- [ ] **SNES_SEARCH not implemented.** Memory-search command is stubbed; needs
  Phase 2 backend to read target memory.
- [ ] **ROM-region writes won't stick.** MAME maps SNES ROM as read-only; writes
  to ROM addresses ($008000+) are silently dropped. WRAM ($7E0000+) writes work.
  Document prominently in the manual's Phase 2 section when the backend lands.
- [ ] **Phase 2 — MAME backend.** Replace the stubbed SLIO transport
  ([`devsys/tools/drmon/linux/slio_stub.cpp`](devsys/tools/drmon/linux/slio_stub.cpp) — the
  designed seam) with a bridge that translates drmon's opcode protocol (read/write mem+regs,
  set/clear breakpoint, step, continue, async-exception) to MAME debugger ops over its
  [Lua debugger API](https://docs.mamedev.org/luascript/ref-debugger.html) or
  [gdbstub](https://docs.mamedev.org/plugins/gdbstub.html) (`127.0.0.1:2159`). Load a SNES
  game in MAME; drmon drives it. Needs a plan before starting.
- [ ] **Phase 3 — DAP front end (confirm approach first).** Wrap drmon-core (65816/68000
  disassemblers, `.sld`/COFF parsers, breakpoint/expr engine) behind a
  [cppdap](https://github.com/google/cppdap) DAP server so VS Code / nvim-dap is the UI —
  likely *instead of* maintaining the ncurses TUI long-term. Decide TUI-vs-DAP before building.


## DRMON — UI / UX

- [ ] **Genesis target (`SYSTEM=GEN`).** Currently SNES-only; add a `genmon` build variant
  (68000 disassembler, `genmon.prc`, `sliogen.cpp` path) once SNES is solid.
- [ ] **Support multiple "monitors"** (as windows)

## DRMON — CLEANUP

- [ ] **Finish dropping Borland / Watcom / OS2 / DOS-extender support.** `compat.hpp` is
  Linux-only now, but legacy `#ifdef __BORLANDC__` / `__WATCOMC__` / `__OS2__` / `DOSX286` /
  `__MSDOS__` arms remain scattered across the tree. Strip them as files are touched so the
  source carries a single modern/Linux path.


## DRMON — DOCS

_(none)_


## PACKAGING

- [ ] **Package drmon as a `.deb`** for the foundry apt repo once it does something useful
  (Phase 2+). Build is reproducible via the Docker toolchain today.


## VERIFY
### implemented; run the plan's verification steps + record, then promote to DONE

_(none)_


## DONE

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
