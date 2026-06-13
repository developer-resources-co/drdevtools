# TODO

drmon = the Developer Resources Monitor, a source-level debugger being brought from 1990s DOS to Linux.
See [docs/plans/2026-06-10-port-drmon-linux.md](docs/plans/2026-06-10-port-drmon-linux.md)
for the phased roadmap.

**Status markers:** `[ ]` open · `[wip]` in progress · `[verify]` implemented, verification
not yet run+recorded (run the linked plan's verification steps, paste raw output + PASS/FAIL
back into the plan, then promote to `[x]`) · `[x]` done (moved to DONE, one tight line).
Plan-first: non-trivial work gets a `docs/plans/YYYY-MM-DD-<topic>.md` and a TODO entry.


## DRMON — DEBUGGER BACKEND

- [verify] **Genesis: fold the M68K into the Lua bridge (drop gdbstub).** The gdbstub(:23946) +
  companion(:41817) split can't read VDP/CRAM/VSRAM/Z80 at a breakpoint (the Lua pump only runs
  while the M68K executes) — proven live in step 4. Replaced with one `-debugger none` bridge:
  shared `mame_cpu_bridge.lua` core + thin SNES/Genesis wrappers, genmon via `sliomame.cpp`,
  native 68000 single-step (`T`). **Built + smoke/lua/script-lint clean; awaiting desktop
  verify** — SNES 19/19 regression, `task verify-genesis-bridge`, and live VDP-at-breakpoint +
  single-step (falls back to a software next-PC decoder if native step doesn't hold). Supersedes
  the connected-mode fix below. [plan](docs/plans/2026-06-14-genesis-backend-fold-the-m68k-into-the-lua-bridge.md)
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
  **Ghidra finding:** a working Ghidra path would need a custom connector (Ghidra's TraceRMI / a bespoke
  RSP agent) that speaks our `target.xml` directly and bridges to Ghidra's own 65816 SLEIGH language —
  i.e. real plugin work, not a configuration.
  [Plan](docs/plans/2026-06-13-65816-rsp-clients-ida-ghidra.md) (depends on the Tier‑1 endpoint below).
- [wip] **Upstream MAME 65816/5A22 register map to `debuggdbstub.cpp`** (~35 lines; prove via Lua plugin
  first; propose arch name `w65c816`). See [65816 gdbstub investigation](docs/investigations/2026-06-11-65816-gdbstub.md).
  [Plan](docs/plans/2026-06-13-mame-65816-gdbstub-map.md) — Tier 1 (Lua, no rebuild) runnable now; Tier 2 (C++) authored, build‑verify disk‑gated.
- [wip] **llvm-mos 65816 C compiler backend — upstream contribution.** Now its own repo:
  **[`wbniv/llvm-mos-65816`](https://github.com/wbniv/llvm-mos-65816)** (private; SNES platform +
  containerized harness + living roadmap). llvm-mos ships a production 65816 *assembler + linker* but
  no C codegen and has **no active implementer**; the SNES SDK target didn't exist
  ([sdk#415](https://github.com/llvm-mos/llvm-mos-sdk/issues/415)). One track, three milestones:
  **M0** SNES SDK target on the existing 6502 backend (65816 boots in emulation mode) → **M1**
  [#320](https://github.com/llvm-mos/llvm-mos/issues/320) (24-bit far pointers) → **M2**
  [#321](https://github.com/llvm-mos/llvm-mos/issues/321) (16-bit A + REP/SEP). **M0 build done**
  (2026-06-13): `mos-snes-clang` produces a bootable `.sfc` from C — reset→crt0, `main()` compiled,
  header/checksum valid; remaining M0 = live emulator boot test + regression corpus. Low-effort
  no-code win: surface the documented WDC816CC/ORCA-C ABI prior art in #320/#321.
  [Plan](docs/plans/2026-06-13-llvm-mos-65816-backend.md) ·
  [investigation](docs/investigations/2026-06-13-llvm-mos-65816-backend.md).


## VERIFY
### implemented; run the plan's verification steps + record, then promote to DONE

- [wip] **Genesis non-CPU state (VDP/CRAM/VSRAM/Z80) — connected mode is broken.** Bridge
  protocol (steps 1–3 + 5) PASS — `task verify-genesis-bridge` 10/10 on Aladdin (fixed an
  autoboot init-order bug that kept the :41817 listener from opening). **But step 4 FAILED:**
  with gdbstub + the autoboot bridge co-loaded, genmon reads `Slave Dead`. A/B proved gdbstub
  *alone* works (M68K run/stop/start fine); adding `-autoboot_script mame_genesis_bridge.lua`
  breaks the gdb link. Likely genmon-side: `br_maybe_reconnect()` runs before the gdb
  reconnect and `br_cmd("V")` stalls ~1 s on a bridge that can't answer while the M68K is
  halted. **Candidate fix applied** (`99ca973`: reconnect gdb first; only handshake the
  bridge while `g_gdb_running`) — build + smoke PASS, **not yet retested on live MAME**.
  **Next:** `task mame SYS=gen CART="roms/genesis/Aladdin (USA).md"` + `task run SYS=gen` →
  expect `Stopped` on connect, F2 to run, VDP/Z80 windows fill. If still dead, run
  `linux/test_gdb.py` to confirm genmon-side vs MAME-side.
  [plan §Verification step 4](docs/plans/2026-06-13-genesis-non-cpu-state-vdp-cram-vsram-z80-lua-side.md)


## DONE

- 2026-06-13 — [ca65-sym] ca65 `.dbg` + WLA-DX `.sym` importers: shared `symfmt.{hpp,cpp}` parser behind DAP `SymbolTable` + TUI `_symbolList`; `task test-symbols` 12+11 tests PASS — [plan](docs/plans/2026-06-11-ca65-dbg-wla-dx-sym-symbol-importers-for-drmon.md)
- 2026-06-13 — [genesis-bridge] Genesis non-CPU state: `mame_genesis_bridge.lua` + second socket in `sliogdb.cpp`; `MTYPE_Z80` window; build + smoke PASS — [plan](docs/plans/2026-06-13-genesis-non-cpu-state-vdp-cram-vsram-z80-lua-side.md)
- 2026-06-13 — [vendor-calypsi] Archived Calypsi 5.17 (65816/68000/6502/nut): all packages + PDFs + release notes in `vendor/calypsi/5.17/`; mirror clone at `vendor/calypsi/tool-chains.git`; `fetch.sh` + SHA256SUMS committed.
- 2026-06-13 — [overflow] Fix DOS 8.3 filename/path buffer overflows: sized path buffers to `_MAX_PATH` + `snprintf`-bounded copies; `tmpnam(szTempName[13])` bug via Settings→Save fixed; `task overflow` FAILS pre / PASSES post — [plan](docs/plans/2026-06-13-dos-83-buffer-overflow-fix.md)
- 2026-06-13 — [far-shims] Strip `far`/`near`/`huge`/`cdecl`/`pascal` keyword shims: 16 `#define`s + ~318 pointer decls across 46 files removed; 124/124 TUs clean, smoke snes/gen PASS — [plan](docs/plans/2026-06-13-strip-far-cdecl-keyword-shims.md)
- 2026-06-13 — [dead-ifdef] Drop dead DOS/compiler `#ifdef` arms (Borland/Watcom/OS2/DOSX286/MSDOS/SC/TurboC) + `MASTERSYSTEM`/`NES` stubs; deleted `input` + `drmon.mak` — [plan](docs/plans/2026-06-12-drop-dead-dos-compiler-ifdef-arms.md)
- 2026-06-13 — [multi-terminal] Multiple monitors (Option A): one process drives N in-process xterms, per-terminal SCREEN, Windows▸New Window; ~293 LOC; all verifications passed — [plan](docs/plans/2026-06-12-drmon-option-a-single-process-multi-terminal.md)
- 2026-06-12 — [dap-tier3] DAP symbol loading: `SymbolTable` (`.sld` + Sierra COFF); `--symbols` flag; disassemble labels, evaluate, setBreakpoints, loadedSources; 5/5 verifications — [plan](docs/plans/2026-06-12-phase-3-drmon-dap-tier-3-symbol-loading.md)
- 2026-06-12 — [dap-tier2] DAP disassembly view: `dis816_dap.cpp`/`dis68000_dap.cpp` wrappers; `procMode` from live FLAGS; 5/5 verifications — [plan](docs/plans/2026-06-12-phase-3-drmon-dap-tier-2-disassembly-view.md)
- 2026-06-12 — [spc700] SPC700 register window (Alt-N) + APU-RAM memory window (`MTYPE_SPC`, Ctrl-R); 27/27 bridge tests, smoke + connected TUI clean — [plan](docs/plans/2026-06-12-spc700-window.md)
- 2026-06-12 — [evalcmd] Revive `EvalCommand`: un-`#if 0`'d verb dispatcher; routed args through live `DoExp`; `OPEN MEMORY`/`OPEN SEARCHLIST` work — [plan](docs/plans/2026-06-12-revive-evalcommand.md)
- 2026-06-12 — [search-win] Revive scrollable mem-search results window: `_searchList` list-rect, Enter→Memory, Del clears; `OpenMemoryAt()` — [plan](docs/plans/2026-06-12-search-results-window.md)
- 2026-06-12 — [deb] Package drmon as `drmon_1.0.1_amd64.deb` (snesmon + genmon, cmake/debhelper, lintian-clean PIE ELFs) in foundry-apt — [plan](docs/plans/2026-06-12-package-drmon.md)
- 2026-06-11 — [snes-stubs] Lift SNES stubs: `ReadSlavePPU` via MAME save-item (`RP`), write-protect → watchpoints, ROM-write warning, client-side mem-search; fixed `mame_cmd` reply stack-overflow; 24/24 bridge tests — [plan](docs/plans/2026-06-11-lift-snes-out-of-scope-stubs.md)
- 2026-06-11 — [mame-backend] Phase 2 MAME backend: snesmon (Lua :41816) + genmon (GDB RSP :23946); 19/19 SNES + 11/11 GEN protocol tests; EOF recovery, orphan immunity verified — [plan](docs/plans/2026-06-11-drmon-mame-backend.md)
- 2026-06-11 — [user-docs] drmon user docs (manual + install + config) under `devsys/tools/drmon/docs/`, reconciled against source — [plan](docs/plans/2026-06-10-drmon-user-manual-installation-configuration-docs.md)
- 2026-06-10 — [viewport] Viewport fill: size CGA screen to COLS×LINES + live `KEY_RESIZE`; fixed width≥83 garble from dormant 64-bit `CopyScreen` long-copy — [plan](docs/plans/2026-06-10-drmon-viewport-fill.md)
- 2026-06-10 — [signals] Handle Ctrl+C + terminal restore on signal-death; `task signals` regression guard.
- 2026-06-10 — [gadgets] Fix Alt+E/Alt+K SIGSEGV: string gadgets seeded with read-only literals; `SetGadgString` writable-buffer helper — [BUGS.md](docs/BUGS.md)
- 2026-06-10 — [winnum] Fix stray window-number `1` on dropdown menus: ctor painted before menu.cpp zeroed it.
- 2026-06-10 — [mouse] Wire ncurses mouse: clicks + drag + hover-highlight; fixed dead hover (missing terminfo `XM`) — [plan](docs/plans/2026-06-10-drmon-mouse.md)
- 2026-06-10 — [cp437] Convert CP437 high bytes → UTF-8-safe `\xNN` literals, 14 files; verified no-op — [plan](docs/plans/2026-06-10-cp437-ascii-conversion.md)
- 2026-06-10 — [borders] Fix borders: force UTF-8 locale + restore Edit-corrupted CP437 border string.
- 2026-06-10 — [alt-keys] Wire Alt-combos + wide-ncurses Unicode box-drawing (CP437→Unicode).
- 2026-06-10 — [phase-1.5] Phase 1.5: drmon runs on Linux — ncurses TUI + keyboard; disconnected — [plan](docs/plans/2026-06-10-port-drmon-linux.md)
- 2026-06-10 — [phase-1] Phase 1: drmon compiles + links on Linux via Dockerized g++/CMake; 52 TUs → x86-64 ELF — [plan](docs/plans/2026-06-10-port-drmon-linux.md)
- 2026-06-10 — [taskfile] Add Taskfile (`task image/build/run/debug/tui-shot/clean/rebuild`) capturing the Dockerized workflow.
- 2026-06-10 — [sourceforge] Post SourceForge "moved to GitHub" notice — [text](docs/sourceforge-notice.md)
- 2026-06-10 — [migrate] Migrate drdevtools CVS → [github.com/developer-resources-co/drdevtools](https://github.com/developer-resources-co/drdevtools) (cvs-fast-export, history preserved) — [plan](docs/plans/2026-06-10-migrate-cvs-to-github.md)
- 2026-06-10 — [hosting] Investigate hosting: SourceForge-only, no prior GitHub migration — [investigation](docs/investigations/2026-06-10-hosting-sourceforge-vs-github.md)
