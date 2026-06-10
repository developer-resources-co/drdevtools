# TODO

drmon = the "DR Monitor" source-level debugger being brought from 1990s DOS to Linux.
See [docs/plans/2026-06-10-port-drmon-linux.md](docs/plans/2026-06-10-port-drmon-linux.md)
for the phased roadmap.

**Status markers:** `[ ]` open · `[wip]` in progress · `[verify]` implemented, verification
not yet run+recorded (run the linked plan's verification steps, paste raw output + PASS/FAIL
back into the plan, then promote to `[x]`) · `[x]` done (moved to DONE, one tight line).
Plan-first: non-trivial work gets a `docs/plans/YYYY-MM-DD-<topic>.md` and a TODO entry.


## DRMON — DEBUGGER BACKEND

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

- [ ] **Viewport fill — size the screen to the terminal.** Replace the hardcoded 80×25 in
  `SetupScreen`'s `__GNUC__` branch with the ncurses `COLS`×`LINES` so the desktop + menu bar fill
  the window (chrome fills, windows fixed); Phase B handles live `KEY_RESIZE`/`SIGWINCH` re-fill.
  — [plan](docs/plans/2026-06-10-drmon-viewport-fill.md)
- [ ] **Genesis target (`SYSTEM=GEN`).** Currently SNES-only; add a `genmon` build variant
  (68000 disassembler, `genmon.prc`, `sliogen.cpp` path) once SNES is solid.


## DRMON — CLEANUP

- [ ] **Finish dropping Borland / Watcom / OS2 / DOS-extender support.** `compat.hpp` is
  Linux-only now, but legacy `#ifdef __BORLANDC__` / `__WATCOMC__` / `__OS2__` / `DOSX286` /
  `__MSDOS__` arms remain scattered across the tree. Strip them as files are touched so the
  source carries a single modern/Linux path.


## DRMON — DOCS

- [ ] **USER's manual + installation + configuration.** Seed the key reference from the
  plaintext `snesmon.hlp`/`genmon.hlp`; document the console command/expression language,
  install (Dockerized build now, `.deb` later), and config (`DR_SNESPORT`/`DR_SNESMEMBUFFER`
  env vars, `.scr` startup scripts, saved layout). Detail in the
  [port plan](docs/plans/2026-06-10-port-drmon-linux.md) "Documentation deliverables".


## PACKAGING

- [ ] **Package drmon as a `.deb`** for the foundry apt repo once it does something useful
  (Phase 2+). Build is reproducible via the Docker toolchain today.


## VERIFY
### implemented; run the plan's verification steps + record, then promote to DONE

_(none)_


## DONE

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
