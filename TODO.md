# TODO

drmon = the Developer Resources Monitor, a source-level debugger being brought from 1990s DOS to Linux.
See [docs/plans/2026-06-10-port-drmon-linux.md](docs/plans/2026-06-10-port-drmon-linux.md)
for the phased roadmap.

**Status markers:** `[ ]` open · `[wip]` in progress · `[verify]` implemented, verification
not yet run+recorded (run the linked plan's verification steps, paste raw output + PASS/FAIL
back into the plan, then promote to `[x]`) · `[x]` done (moved to DONE, one tight line).
Plan-first: non-trivial work gets a `docs/plans/YYYY-MM-DD-<topic>.md` and a TODO entry.


## DRMON — DEBUGGER BACKEND

- [ ] **Genesis non-CPU state (VDP/CRAM/VSRAM/Z80) still stubbed.** `ReadSlaveVDP`/`ReadSlaveCRAM`/`ReadSlaveVSRAM`
  zero-stubbed in `sliogdb.cpp`; no Z80 window exists. Fix: companion `mame_genesis_bridge.lua` on port
  41817 serves `RV`/`RC`/`RS`/`RZ` commands; second socket in `sliogdb.cpp`; new `MTYPE_Z80` memory window.
  [plan](docs/plans/2026-06-13-genesis-non-cpu-state-vdp-cram-vsram-z80-lua-side.md)
- [wip] **Phase 3 — DAP front end (Tiers 1–3 complete; live-MAME items remain).** `drmon-dap-snes` /
  `drmon-dap-gen` DAP adapters: attach/continue/pause/step/registers/readMemory/instruction
  breakpoints/disassembly/symbol-file loading (binary `.sld` + Sierra COFF). VS Code setup doc at
  `docs/dap-setup.md`. Verification items needing live MAME still open (connected session, breakpoint
  fires, registers/memory match, VS Code pane/source view).
  [Tier 1 plan](docs/plans/2026-06-12-phase-3-dap.md) ·
  [Tier 2 plan](docs/plans/2026-06-12-phase-3-drmon-dap-tier-2-disassembly-view.md) ·
  [Tier 3 plan](docs/plans/2026-06-12-phase-3-drmon-dap-tier-3-symbol-loading.md).


## DRMON — UI / UX

_(none)_

## DRMON — CLEANUP

_(none)_


## DRMON — DOCS

_(none)_


## DRMON — INVESTIGATIONS

- [wip] **IDA Pro + Ghidra 65816 RSP against MAME gdbstub — get real answers.**
  [idapro_m6502](https://github.com/LucienMP/idapro_m6502) shows IDA's gdb client consuming MAME gdbstub XML for m6502.
  Verify the same pipeline for 65816: does a 65816 processor module exist for IDA; does IDA's gdb client
  work with a hand-authored `target.xml`; does Ghidra's RSP debugger path connect without a custom
  agent? Needs the MAME Lua 65816 map (Tier 1 of [65816 gdbstub investigation](docs/investigations/2026-06-11-65816-gdbstub.md))
  as the test endpoint. Write findings to `docs/investigations/`.
  [Plan](docs/plans/2026-06-13-65816-rsp-clients-ida-ghidra.md) (depends on the Tier‑1 endpoint below).
- [wip] **Upstream MAME 65816/5A22 register map to `debuggdbstub.cpp`** (~35 lines; prove via Lua plugin
  first; propose arch name `w65c816`). See [65816 gdbstub investigation](docs/investigations/2026-06-11-65816-gdbstub.md).
  [Plan](docs/plans/2026-06-13-mame-65816-gdbstub-map.md) — Tier 1 (Lua, no rebuild) runnable now; Tier 2 (C++) authored, build‑verify disk‑gated.
- [ ] **llvm-mos 65816 C compiler backend — contribution/funding decision.** Research done
  ([investigation](docs/investigations/2026-06-13-llvm-mos-65816-backend.md)): llvm-mos ships a
  production 65816 *assembler + linker* but no C codegen; the work is two open design-only issues
  ([#320 24-bit address space](https://github.com/llvm-mos/llvm-mos/issues/320),
  [#321 16-bit register mode](https://github.com/llvm-mos/llvm-mos/issues/321)) with **no active
  implementer** since @asiekierka stepped away. Every barrier *around* codegen is now cleared
  (assembler, linker, ELF, DWARF spec landed Dec 2025). Four paths laid out in the investigation,
  cheapest first: (1) feed primary-source Zardoz ABI knowledge into the calling-convention design;
  (2) land the SP=$01FF 8-bit-mode on-ramp; (3) fund @asiekierka/@mysterymath to resume; (4)
  implement #320 → #321 directly. Decision is the user's. See also
  [Zardoz investigation](docs/investigations/2026-06-12-zardoz-65816-compiler.md).


## VERIFY
### implemented; run the plan's verification steps + record, then promote to DONE

_(none)_


## DONE

- [x] 2026-06-13 — ca65 `.dbg` + WLA-DX `.sym` symbol importers: shared `symfmt.{hpp,cpp}` parser (labels + source lines) behind both the DAP `SymbolTable` (`loadCa65Dbg`/`loadWlaSym`) and the TUI `_symbolList` (`LoadSymbol`) + new `tableSld` source backend; `task test-symbols` (12 unit + 11 DAP) + smoke/overflow + headless TUI load all PASS — [plan](docs/plans/2026-06-11-ca65-dbg-wla-dx-sym-symbol-importers-for-drmon.md)
- 2026-06-13 — [vendor-calypsi] Archived Calypsi 5.17 (65816/68000/6502/nut): all packages + PDFs + release notes in `vendor/calypsi/5.17/`; mirror clone at `vendor/calypsi/tool-chains.git`; `fetch.sh` + SHA256SUMS committed.
- [x] 2026-06-13 — Fix DOS 8.3 filename/path buffer overflows: sized `patternString`/`fileReqFileName`/new `fileReqDirName`/`logFileName`/`szTempName` to `_MAX_PATH` + `snprintf`-bounded the requester/log copies; deleted dead `#define FILENAME_SIZE 9`. Reachable bug was `tmpnam(szTempName[13])` via Settings→Save; regression guard `task overflow` (real `profile.cpp` under ASan) FAILS pre-fix / PASSES post-fix; build + smoke snes/gen clean — [plan](docs/plans/2026-06-13-dos-83-buffer-overflow-fix.md)
- [x] 2026-06-13 — Strip `far`/`near`/`huge`/`cdecl`/`pascal` keyword shims: removed the 16 bare `#define`s from `linux_compat.hpp` (kept `farmalloc`/`coreleft` function macros) + the keyword tokens from ~318 `far *` decls across 46 files (2 `*`-anchored sed regexes) + 7 surgical non-pointer sites; build self-checks (any miss = parse error) → 124/124 clean, smoke snes/gen PASS, `gcc -E` token stream byte-identical on 6 compiled TUs — [plan](docs/plans/2026-06-13-strip-far-cdecl-keyword-shims.md)
- [x] 2026-06-13 — Drop dead DOS/compiler `#ifdef` arms (Borland/Watcom/OS2/DOSX286/MSDOS/SC/TurboC) + `MASTERSYSTEM`/`NES` stub hooks; kept `DEBUGZARDOZ`/`EMUL` + far/cdecl shims. Executed bundled in `0be2bcd`; verification recorded (build+smoke snes/gen PASS, `gcc -E` proves arm-removal behavior-neutral); deleted last dead-DOS artifacts `input` (TASM listing) + `drmon.mak` — [plan](docs/plans/2026-06-12-drop-dead-dos-compiler-ifdef-arms.md)
- [x] 2026-06-13 — Support multiple "monitors" (Option A): one drmon process drives N in-process xterms, each an independent desktop (per-terminal SCREEN + virtualized screenWidth/Height/Size) sharing one MAME connection; Windows▸New Window opens an in-process terminal; spawned desktops send Alt as ESC-prefix; ~293 LOC isolated; all verifications passed, merged to main — [plan](docs/plans/2026-06-12-drmon-option-a-single-process-multi-terminal.md)
- [x] 2026-06-12 — Phase 3 Tier 3 — DAP symbol loading: fresh `SymbolTable` (binary `.sld` + Sierra COFF) in `linux/dap/`; `--symbols` CLI flag; `disassemble` label annotation, `evaluate` symbol lookup, `setBreakpoints` source-line resolution, `loadedSources` handler; 5/5 reproducible verifications — [plan](docs/plans/2026-06-12-phase-3-drmon-dap-tier-3-symbol-loading.md)
- [x] 2026-06-12 — Phase 3 Tier 2 — DAP disassembly view: `dis816_dap.cpp`/`dis68000_dap.cpp` wrappers + `disasm_preamble.hpp`/`disasm_stubs.cpp` break `moninc.hpp` dependency; `procMode` from live FLAGS; 5/5 reproducible verifications (VS Code pane needs live MAME) — [plan](docs/plans/2026-06-12-phase-3-drmon-dap-tier-2-disassembly-view.md)
- [x] 2026-06-12 — SPC700 (SNES audio co-CPU) debugging UI: editable **SPC register window** (Alt-N, `PC/A/X/Y/SP/PSW` via bridge `GA`/`PA`) + **APU-RAM memory window** (`MTYPE_SPC`, Type→SPC RAM or Ctrl-R, byte dump via `RA`/`ReadSlaveApuRam`); guarded `#ifdef SPC700` (feature, not platform — SNES block `#define`s it, genmon excludes all of it); caught+fixed a `mTypeText[]` NULL-deref; 27/27 bridge tests, smoke + connected TUI clean — [plan](docs/plans/2026-06-12-spc700-window.md)
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
