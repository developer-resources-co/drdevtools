# TODO

## drmon — Linux port

See [docs/plans/2026-06-10-port-drmon-linux.md](docs/plans/2026-06-10-port-drmon-linux.md).

- [ ] **Drop Borland / Watcom / OS2 / DOS-extender (PharLap) support** — make the tree
  Linux / modern-C++ only; strip the `__BORLANDC__` / `__WATCOMC__` / `__OS2__` /
  `DOSX286` / `__MSDOS__` `#ifdef` arms as files are touched.
- [ ] **Phase 2 — MAME backend**: replace the SLIO transport stub with a bridge to MAME's
  Lua debugger / gdbstub, so drmon drives a SNES game running in MAME.
- [ ] **Phase 3 — DAP front end** (confirm approach first): expose drmon-core as a DAP
  server (cppdap) so VS Code / nvim-dap is the UI — likely instead of porting the ncurses TUI.
- [ ] **Docs**: USER's manual (seed from `snesmon.hlp`/`genmon.hlp`), installation, configuration.

## Done

- [x] 2026-06-10 — Phase 1.5: drmon **runs on Linux** — ncurses front end renders the TUI (menu bar, windows, status line, clock) + keyboard input (F10/arrows/F-keys); runs disconnected (no target). See [linux/README.md](devsys/tools/drmon/linux/README.md).
- [x] 2026-06-10 — Phase 1: drmon compiles + links on Linux (SYSTEM=SNES) via Dockerized g++/CMake; 52 TUs → x86-64 ELF. Transport/screen/input stubbed. See [linux/README.md](devsys/tools/drmon/linux/README.md).
