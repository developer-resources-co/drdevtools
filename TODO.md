# TODO

drmon = the Developer Resources Monitor, a source-level debugger being brought from 1990s DOS to Linux.
See [docs/plans/2026-06-10-port-drmon-linux.md](docs/plans/2026-06-10-port-drmon-linux.md)
for the phased roadmap.

**Status markers:** `[ ]` open · `[wip]` in progress · `[verify]` implemented, verification
not yet run+recorded (run the linked plan's verification steps, paste raw output + PASS/FAIL
back into the plan, then promote to `[x]`) · `[x]` done (moved to DONE, one tight line).
Plan-first: non-trivial work gets a `docs/plans/YYYY-MM-DD-<topic>.md` and a TODO entry.


## DRMON — DEBUGGER BACKEND

- [wip] **Phase 3 — DAP front end (Tiers 1–3 complete; live-MAME V3–V6 PASS; only VS Code GUI panes remain).**
  `drmon-dap-snes` / `drmon-dap-gen` DAP adapters: attach/continue/pause/step/registers/readMemory/instruction
  breakpoints/disassembly/symbol-file loading (binary `.sld` + Sierra COFF). VS Code setup doc at
  `docs/dap-setup.md`. **Live-MAME V3–V6 now automated + PASS (2026-06-18, `task test-dap` →
  `linux/test_dap.sh` + `dap/test_dap.py`): connected session, breakpoint fires, registers + memory,
  each cross-checked against a direct bridge read; 3/3 runs 11/11.** Remaining: the two VS Code *GUI pane*
  confirmations only (Tier 2 disassembly pane, Tier 3 source highlight) — manual, GUI views of
  already-verified protocol features.
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

- [ ] **IDA Pro + Ghidra 65816 RSP against MAME gdbstub — BLOCKED (commercial license).** The
  free part is done: the connection recipe + wire-format proof (`target.xml` + `g`-packet layout
  match what IDA's gdb client consumes, per [idapro_m6502](https://github.com/LucienMP/idapro_m6502))
  are written up in the investigation's `clients.md`. The remaining "does IDA actually attach" GUI
  step needs **IDA Pro Essential (~$1,099/yr)** + a third-party 65816 processor module (IDA
  Free/Home can't load custom procmods); **Ghidra** needs a custom TraceRMI/SLEIGH connector (real
  plugin work, not config). Unblocks only with an IDA Pro license — recipe is ready if that happens.
  Depends on the Tier‑1 endpoint below for a live target.
  [Plan](docs/plans/2026-06-13-65816-rsp-clients-ida-ghidra.md) · [investigation](docs/investigations/2026-06-11-65816-gdbstub.md)
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


## SPC700 IPL — CLEAN-ROOM REIMPLEMENTATION

- [ ] **Clean-room a 64-byte SPC700 IPL boot ROM for legal MAME use + public release.**
  Independently re-create the SNES APU boot ROM (Chinese-wall: interface-derived spec →
  untainted implementer → behavioural verification in MAME) so the bytes + source can be
  committed to a public repo and dropped into MAME. **Assembler = WLA-DX `wla-spc700`** (FOSS,
  Linux-native, publicly reproducible; `bass` cross-check). `tools/spasm`'s SPC700 backend was
  *verified to exist* (`opcode70.asm`/`cam700.asm`, we own it) but is **not used** — 16-bit DOS,
  weakens independence. Research-grounded: **no hardware checksum on the IPL** (snes9x/bsnes/ares
  do no check) — boot needs behavioural equivalence; byte-identity is forced only by **MAME's
  romset hash** (CRC 44bb3a40), and convergence is *merger* evidence (NEC v. Intel). **Gated:**
  Person B must be untainted (the LLM and likely the original author are not; an untainted human is
  the gold standard). **Phase 0 ✅** (2026-06-19): wla-spc700 v10.6 (primary) + bass v18
  smoke-tested, agree byte-for-byte; bass uses a 6502-style dialect (`spcasm` for a same-source
  cross-check). **NEXT (re-sequenced 2026-06-19): IP counsel FIRST (§0)** — the project turns on
  "is a 64-byte forced boot ROM copyrightable?"; send the EFF (info@eff.org) / IP counsel the §0.A packet, act on
  §0.B. Blocks Phases 1–6 (recruiting Person B is premature until then). **Stretch goals
  (2026-06-19):** become the **legal precedent** (a 64-byte forced ROM is non-protectable /
  a documented clean room suffices) AND the first **ML-agent independent-creation precedent**
  (provably-clean *open* model + two-model wall; closed frontier models incl. Claude
  disqualified) — §5B/§5B.1; the precedent track runs parallel to the build.
  [Plan](docs/plans/2026-06-19-spc700-ipl-cleanroom.md).


## VERIFY
### implemented; run the plan's verification steps + record, then promote to DONE

_(none)_


## DONE

- 2026-06-14 — [genesis-lua-bridge] Folded the Genesis M68K into the Lua bridge (dropped gdbstub) so VDP/CRAM/VSRAM/Z80 read at a breakpoint: shared `mame_cpu_bridge.lua` core + thin SNES/Genesis wrappers, genmon unified on `sliomame.cpp`, native 68000 single-step. Verified: SNES 27/27 (no regression), Genesis 15/15 + non-zero VDP/Z80 data + native step holds. Branch `genesis-lua-bridge-m68k` pushed — [plan](docs/plans/2026-06-14-genesis-backend-fold-the-m68k-into-the-lua-bridge.md)
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


## Inbox — auto-captured plan deferrals

_Auto-added from plan "Out of scope"/"Deferred" sections at commit time. Triage each into M1/M2/etc. and delete it here — it will not come back._

<!-- BEGIN auto-captured-deferrals (managed by audit-plan-deferrals.sh — triage these into the curated sections above; the fingerprint ledger means a deleted item is NOT re-added) -->
- [verify] **2026-06-19-spc700-ipl-counsel-email** — Verification section present but no PASS recorded — run + record the steps. _from [2026-06-19-spc700-ipl-counsel-email.md](docs/plans/2026-06-19-spc700-ipl-counsel-email.md)_  <!-- fp:73bd340fc329f9b0 -->
<!-- END auto-captured-deferrals -->
