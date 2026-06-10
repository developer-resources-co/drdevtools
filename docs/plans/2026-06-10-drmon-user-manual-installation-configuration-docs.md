# drmon: User manual + installation + configuration docs

## Context

drmon now **runs** on Linux (TUI, resize-aware, signal-safe, ASan-clean) but is undocumented.
TODO.md → DRMON–DOCS: *"USER's manual + installation + configuration. Seed the key reference
from the plaintext `snesmon.hlp`…"*. The port plan
([docs/plans/2026-06-10-port-drmon-linux.md](../../SRC/drdevtools/docs/plans/2026-06-10-port-drmon-linux.md):87–121,
"Documentation deliverables") specifies the exact deliverables: three docs under
**`devsys/tools/drmon/docs/`** — `drmon-manual.md`, `install.md`, `configuration.md` — seeded
from the shipped `.hlp` files and source, with everything that works *disconnected* written
now and target-dependent features flagged **"needs a backend (Phase 2+)"**.

Outcome: a new user can install, launch, navigate, and use the expression/command/key UI of
drmon from these docs, and understands honestly what does/doesn't work without a live target.

## Key principle: source is ground truth, `.hlp` is a prose seed

The `.hlp` files are ~1994 and **drifted** from the code. Confirmed mismatches:
- `.hlp` "String Requesters: alt-A Cut / alt-C Copy / alt-P Paste" — but `monkeys.hpp:14-16`
  defines `CMD_CUT=Alt+U`, `CMD_COPY=Alt+C`, `CMD_PASTE=Alt+V` (and `Alt+P` is *log-window*).
- `.hlp` "F8: Activate Local Menu" — but `monkeys.hpp` has `CMD_STEPOVER=F8`; local menu is
  `Ctrl+F10`.

So: seed *descriptions/wording* from the `.hlp`, but derive every **binding, command name,
operator, env var, and default from source** and silently correct stale entries. Also consult
`history.txt` (feature changelog) and `snesmon.scr` (example startup script) as seeds.

## Ground-truth sources (cite while writing; don't invent)

- Keys: `monkeys.hpp` (global F-keys, Alt-combos, per-window Ctrl-keys, movement mode).
- Menus: `monmenu.cpp` (File / Control / Windows / Macros / Rate / Settings / Help).
- Console commands: `command.cpp` `RESVDWORD[]`/`WORDTYPE[]` (~lines 38–198) + dispatch
  (`EvalCommand`, `ParseHardWindow`); macro `name: …`, symbol `name = val`, `@addr=val`.
- Expression language: `expr.l` (bases: dec, `$`/`0x` hex, `%`/`0b` binary) + `expr.y`
  (operators, `@n(addr)` memory read, symbols, `numberofones()`); tokens in `express.hpp`.
- Windows/views: `reg.cpp`, `memory.cpp`/`memops.cpp`, `break.cpp`, `watch.cpp`, `symbol.cpp`,
  `console.cpp`, `expr.cpp`, `source.cpp`, `textview.cpp`, `charts.cpp`, `info.cpp`, `about.cpp`.
- Concepts/transport: `break.hpp`, `watch.hpp`, `control.hpp`, `send.hpp`, `symbol.cpp`,
  `sld.hpp`, `coff.hpp`, `dis816.cpp`, `slaveio.hpp`, `linux/slio_stub.cpp` (the Phase-2 seam).
- Install/config: root `Taskfile.yml` (tasks), `linux/README.md`, `linux/Dockerfile`,
  `drmon.cpp` `Init()`/`LoadEnvVariables()`/`DoStartupExec()` (argv, env, `.scr` discovery),
  `board.cpp:36-44` (`DR_SNESPORT`/`DR_SNESMEMBUFFER`), `config.cpp`/`profile.cpp` (`.ini`),
  `version.h` + the on-screen `SNESMon V2.1.30` status string (use the displayed version).

## Deliverables

Create `devsys/tools/drmon/docs/` with three GitHub-flavored-markdown files:

**1. `install.md`** — Prerequisites (Docker only; nothing on host). The `task` workflow
(`image` → `build` → `run`), where the binary lives (`/tmp/drmon-build/drmon`), TTY ≥ 80×25,
the ASan-by-default build + `ASAN_OPTIONS` log path, the `drmon-build` image. Other tasks
(`debug`/`shot`/`smoke`/`signals`/`clean`/`rebuild`). Future: `.deb` for the foundry apt repo.
Seed from `linux/README.md`.

**2. `configuration.md`** —
- **Environment:** `DR_SNESPORT` (dev-link port base, default `0x318`), `DR_SNESMEMBUFFER`
  (comram segment); generic `DR<PLATFORM>PORT`/`…MEMBUFFER` form; hex parsing; *legacy hardware
  knobs, replaced by the Phase 2 MAME bridge endpoint*.
- **Startup scripts (`.scr`):** auto-discovery order (`argv[1]` → `argv[1].SCR` → `PROGNAME.scr`),
  syntax (`;` comment, `\` continuation, else a console command), worked example
  (`open reg` / `pcl = $C00000` / `open memory dynamicrunning pcl`).
- **Saved state (`.ini`):** what persists (colors/attrs, display mode, run mode, layout) via
  `config.cpp`/`profile.cpp`, file discovery (`PROGNAME.ini`), Settings-menu Load/Save.
- **Build variant:** SNES default; Genesis (`-DGENESIS`) is future.

**3. `drmon-manual.md`** (the main doc) —
1. What drmon is (65816/68000 source-level monitor; 1994 DOS lineage; Linux/ncurses port).
2. Getting started — launch, the desktop + menu bar + status/clock line; **honest disconnected
   caveat** up front (no live target until Phase 2).
3. Concepts — breakpoints (once/count/conditional), watches (expressions), step/over/run,
   symbols & debug info (`.sld` + COFF), the 65816 disassembler, the SLIO dev-link
   (Phase-2 seam) — each target-dependent item flagged **"needs a backend (Phase 2+)"**.
4. Windows / views — one short section per window (what it shows + how to interact), per the
   inventory above; note `*` single-instance windows.
5. Key reference — full table from `monkeys.hpp` (globals, window opens, per-window Ctrl-keys,
   movement mode), reconciled with the `.hlp`.
6. Menu reference — the 7 menus from `monmenu.cpp`.
7. Console command language — the `command.cpp` command set + macro/symbol/`@`-write forms.
8. Expression language — number bases, operators, `@n(addr)`, symbols, `numberofones()`.
9. Typical workflow — load symbols → open Memory/Source → set breakpoint → run → step →
   inspect (framed as the intended flow, with the Phase-2 caveat).
10. Limitations & Phase 2 — consolidated list of what needs the MAME backend.

Also: add a short **"Documentation"** section to `linux/README.md` linking the three docs
(the plan calls for "plus a README section"), and flip the TODO item to `[x]` (DONE) with a
one-line summary.

## Out of scope

No code changes. No `docs/plans` entry (this *is* doc work) and nothing for `BUGS.md`. Genesis
specifics and live-target screenshots are deferred to Phase 2.

## Verification

1. After writing each `.md`, run `task md -- devsys/tools/drmon/docs/<file>.md` to preview
   (browser render); fix any list/table/wrap issues per the SRC writing conventions.
2. Cross-check accuracy against source (the reconciliation is the main risk):
   - every Alt-key / F-key / Ctrl-key in the manual matches `monkeys.hpp`;
   - every console command matches `command.cpp` `RESVDWORD[]`;
   - every operator / number base matches `expr.l`/`expr.y`;
   - env vars/defaults match `board.cpp`/`drmon.cpp`; `.scr` discovery matches `DoStartupExec`.
3. Sanity-run the UI to confirm documented entry points exist: `task run` (open a few windows
   via their documented Alt-keys), or rely on `task smoke` (which already opens every window by
   Alt-key) as evidence the key reference is real.
4. Confirm every target-dependent feature carries a visible "needs a backend (Phase 2+)" note.
5. `task md` the updated `linux/README.md`; verify the three doc links resolve.
