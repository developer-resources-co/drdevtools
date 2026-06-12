# drmon Option A — one process, N terminals, one game

**Date:** 2026-06-12
**Branch:** `feature/drmon-option-a-multiterm`
**Status:** phases 1–3 implemented + verified (single-process multi-terminal works end-to-end)
**Spike proof:** `devsys/tools/drmon/linux/spikes/multiterm_spike.c` (architecture confirmed)
**Supersedes (if it lands):** the multi-*process* spawn tagged `drmon-multiwindow-v1`

## Context

"Multiple monitors" = spread drmon's sub-windows (Memory, Register, …) across several OS terminal
windows, **all viewing one running game**. The shipped multi-*process* spawn can't do this: the MAME
bridge is single-client, so separate processes can't share one debug session. The fix is to make **one**
drmon process drive **N** terminals: one MAME connection + one debug/breakpoint state (already global,
already shared — no change), with **N independent "desktops"** (each its own window set + framebuffer +
ncurses SCREEN). The spike proved the three mechanics (independent `newterm()` SCREENs, `poll()` input
mux, `xterm -S` real windows). This plan integrates them into drmon.

## Key design decision: virtualize globals by NAME, not by save/restore

drmon keeps its whole UI in ~global state. ~15 of those globals are **per-desktop** (window list,
object list, front window, framebuffer, cursor, console history, per-window singleton flags); the rest
(MAME socket, run-state, breakpoints, registers, config) are **shared** and stay exactly as-is.

`layBase`, `statTextBase`, `cmdTextBase` are **intrusive list heads**: heap nodes store the head's
*address* in their `prev`. So a "copy the globals in/out on switch" scheme would corrupt them (nodes
would point at a head that now holds another desktop's data). Instead, give each desktop's state a
**stable address** inside a `Desktop` struct and redirect the global *names*:

```c
struct Desktop { _layerBase layBase; _object *pObjBase, *frontObj; char far *screen,*scrBuffer; … int nc; };
extern Desktop *g_desk;                 // the active desktop
#define layBase  (g_desk->layBase)      // every existing reference now hits the active desktop
#define pObjBase (g_desk->pObjBase)     // …no per-call-site rewrite, no save/restore, stable addresses
```

`switchDesktop(d)` is then just `g_desk = d; drmon_nc_select(d->nc);`. The macros live in one header
included early (via `moninc.hpp`); the old definitions/externs of those globals are removed (their
storage now lives in `Desktop`). To keep terminals uniform (so `screenWidth/Height` can stay shared),
spawned xterms are fixed to the primary's size for v1.

## Phases (each builds + smoke-tests + commits)

### Phase 1 — ncurses layer goes multi-SCREEN  *(self-contained; keeps N=1 identical)*
`linux/ncurses_io.cpp` + `include/ncurses_io.h`: replace the single-`stdscr` statics with a small
per-SCREEN context array.
- `drmon_nc_init()` keeps one-time global setup (locale, signals, `atexit`) but creates SCREEN 0 via
  `newterm(NULL, stdout, stdin)` (== what `initscr` did) and runs the per-SCREEN setup.
- new `int drmon_nc_open(int out_fd, int in_fd)` → `newterm` on a PTY, per-SCREEN setup, returns a handle.
- new `void drmon_nc_select(int handle)` → `set_term(ctx->sp)` + point the "current ctx" at it.
- new `int drmon_nc_infd(int handle)` → the PTY read fd, for `poll()`.
- `blit/getbyte/keyready/size/resized/getmouse` operate on the current ctx + current SCREEN
  (`COLS/LINES` track the current SCREEN after `set_term`). `shift` stays one global (pointer is cached
  by `input.cpp`; only the active terminal's modifiers matter at a time).
- `drmon_nc_shutdown`/emergency restore tear down all SCREENs.
**Verify:** `task build`; `task smoke` (single-terminal path unchanged).

### Phase 2 — `Desktop` struct + global virtualization  *(compiles, still single-desktop at runtime)*
- new `desktop.hpp` (struct + `g_desk` + the `#define` redirects) and `desktop.cpp` (`Desktop *g_desk`,
  `MakeDesktop()`, `switchDesktop()`); include `desktop.hpp` from `moninc.hpp`.
- Move the per-desktop globals' **storage** into `Desktop`; delete their old definitions in
  `layer.cpp/object.cpp/drmon.cpp/display.cpp/screen.cpp/menu.cpp/manager.cpp/command.cpp/console.cpp/`
  `reg.cpp/break.cpp/symbol.cpp/source.cpp/monmenu.cpp`, and the matching `extern`s in the headers.
- `Init()` allocates desktop 0 (= primary), points `g_desk` at it, runs existing init into it.
**Verify:** `task build`; `task smoke` — behaviour identical with exactly one desktop.

### Phase 3 — main loop over desktops + input mux + New Window  *(delivers the feature)*
- `MainLoop()` (drmon.cpp): iterate the desktop list — `switchDesktop(d); pObjBase->Update();
  UpdateScreen();` per desktop. Input becomes `poll()` over every desktop's `drmon_nc_infd()`; a
  readable fd → `switchDesktop(d)` then pump (existing `Manager`/`InputPending` path, unchanged).
- Replace `SpawnNewWindow()` (multi-process) with `NewDesktop()`: `openpty` (primary's winsize) →
  fork `xterm -S<master>` (CPR-handshake to confirm attach, per the spike) → `drmon_nc_open(slave…)` →
  `MakeDesktop()` → run the per-desktop init (InitScreen/Display/Manager/menu) into it. `monmenu.cpp`
  `MenuNewWindow` calls `NewDesktop()`. Closing a desktop's window tears it down (and never the last).
**Verify:** `task build`; `task smoke`; **live**: `task mame` + `task run`, open Memory in terminal 1
and Register in terminal 2, step the CPU from either — both update; one `ss` connection to the bridge.

## Files

- New: `desktop.hpp`, `desktop.cpp`, `NewDesktop()` (replacing `linux/spawn_window.*`).
- Heavy edits: `linux/ncurses_io.cpp`/`.h`, `drmon.cpp` (MainLoop, Init), `monmenu.cpp`.
- Mechanical (remove def/extern of a virtualized global): `layer.cpp/.hpp`, `object.cpp/.hpp`,
  `display.cpp/.hpp`, `screen.cpp/.hpp`, `menu.cpp`, `manager.cpp`, `command.cpp`, `console.cpp`,
  `reg.cpp`, `break.cpp`, `symbol.cpp`, `source.cpp`, `global.hpp`.

## Verification (done)

1. **Build** — `task build SYS=snes` and `SYS=gen` both link (snesmon, genmon, drmon-dap-*). PASS.
2. **Smoke** — `task smoke` SYS=snes and SYS=gen: open every window via Alt-keys, type, no SIGSEGV.
   Single-desktop behaviour unchanged. PASS.
3. **Live New Window** — run snesmon (primary in a tmux pane) with X passthrough, drive
   F10 → Windows → New Window. Result: a second `xterm -title "drmon #1"` appears on the host X,
   rendering a complete independent desktop (own menu bar + status line), while the primary stays
   alive and intact — all from **one** snesmon process (`pgrep -c snesmon` = 1). ASan clean. PASS.
   - Two bugs found + fixed during this step: (a) heap-use-after-free in `InitMessageBar` — the
     `messageLayer` global wasn't virtualized, so a new desktop's init freed desktop 0's message bar
     (plus 5 more missed per-desktop globals: aboutObjPtr/exprObjPtr/searchObjPtr/spcRegObjPtr +
     open flags, pGadgDown); (b) the new terminal rendered blank-blue because `refreshEnable`
     (statically TRUE in layer.cpp) is value-initialized to FALSE in a fresh Desktop — set in
     `InitDesktop`.
4. **One game, one connection (live MAME)** — `task mame`-style headless SNES (drmon-test.sfc) +
   container snesmon connected. The primary connects (status **Running**); F3 from the primary halts
   the CPU (primary Register shows a live `PC:9A24 SP:01FF`) and the **New Window desktop reads
   "Stopped"** — i.e. a run-state change in terminal 1 is seen in terminal 2. Bridge connections
   (drmon→:port, ESTABLISHED) measured **1** at 1 desktop, **1** at 1 desktop+Register, and **1** at
   2 desktops — one process, one client serving both views. PASS.
   - Harness gotcha (not a product bug): the bridge is single-client, so a `/dev/tcp` readiness probe
     steals the one slot and the real client is then refused. Don't pre-probe; connect once.

## Code footprint — chunky & isolated, not strewn

**~293 net LOC** (526 added / 233 removed) across 11 files. The shape matters more than the count:

- **Two new, self-contained files carry the feature**: `desktop.hpp` (67) + `desktop.cpp` (243) =
  **310 lines** — the whole desktop abstraction (struct, `desktop_save`/`desktop_load`,
  `switchDesktop`, `MakeDesktop`, `InitDesktop`, `NewDesktop`).
- **One self-contained rewrite**: `linux/ncurses_io.cpp` (+191 / −94) — single-`stdscr` → N-`SCREEN`,
  entirely within the ncurses layer (+`ncurses_io.h` +5/−1).
- **Surgical taps into the 30-year-old TUI core** (clean seams, ~20 lines total): `moninc.hpp` **+1**
  (one `#include`), `drmon.cpp` **+9/−4** (the MainLoop desktop loop + one line creating desktop 0),
  `monmenu.cpp` **+3/−3** (New Window callback), `CMakeLists.txt` **+2/−2**.
- **Removed**: `spawn_window.cpp/.hpp` (−127) — the old multi-process path.

The headline: **every one of the ~18 files that *define* a virtualized per-desktop global is byte-for-byte
unchanged** — `layer.cpp`, `screen.cpp`, `display.cpp`, `object.cpp`, `reg.cpp`, `break.cpp`,
`symbol.cpp`, `command.cpp`, `console.cpp`, `menu.cpp`, `manager.cpp`, `source.cpp`, `about.cpp`,
`expr.cpp`, `search.cpp`, `gadget.cpp`, `spc_reg.cpp`, `debug.hpp`. The save/restore design
`extern`-references and swaps those globals from `desktop.cpp`, so the legacy core logic never had to be
touched. (The macro-virtualization alternative would have edited the declaration of every one of them —
that's the ~2× churn this approach avoided.) Net effect: the feature reads as a bolt-on module plus a
few one-line seams, not a refactor spread through the codebase.

## Non-goals / risks

- Per-terminal *different sizes* deferred (v1 fixes spawned terminals to the primary's size so
  `screenWidth/Height` stay shared). Resize of a non-primary terminal deferred.
- No change to the MAME bridge, breakpoints, or run-state (shared by design).
- Risk: missing a per-desktop global in the virtualization → cross-desktop bleed. Mitigated by the
  macro approach (a missed global is a *compile error* on the deleted symbol, not silent bleed).
- Reversible: all on a branch; if shelved, delete the branch (main untouched, `drmon-multiwindow-v1`
  still tags the shipped multi-process feature).
