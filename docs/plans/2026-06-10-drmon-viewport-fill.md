# drmon viewport fill — size the screen to the terminal (Linux ncurses)

**Date:** 2026-06-10
**Status:** Proposed — plan for review before implementation
**Companion to:** [drmon mouse support](2026-06-10-drmon-mouse.md) (this was split off as the separate
"resize" plan). Mouse landed first; this is "chrome fills, windows fixed".

## Context

drmon's Linux screen is **hardcoded to 80×25**. `SetupScreen`'s `__GNUC__` branch
([`screen.cpp:421-437`](../../devsys/tools/drmon/screen.cpp)) sets `screenWidth=80, screenHeight=25`
and `calloc`s the CGA framebuffer to that. So in any terminal bigger than 80×25, drmon paints an
80×25 island in the top-left and leaves the rest blank. Goal: **size the screen to the terminal's
actual `COLS`×`LINES` so the desktop background and the top menu bar fill the whole window**, while
the debugger windows keep their fixed sizes (chrome fills, windows fixed) — plus re-fill live when
the terminal is resized.

## The dimension model (as-is, from the code)

- **`screenWidth`/`screenHeight`** ([`screen.cpp:47`](../../devsys/tools/drmon/screen.cpp)) — the
  physical viewport. `screenSize = screenWidth*screenHeight*charSize` (`charSize`=2: char+attr).
- **`screen`** — the CGA framebuffer (`calloc`'d to `screenSize`), blitted to the terminal by
  `drmon_nc_blit(screen, screenWidth, screenHeight)` ([`display.cpp:100`](../../devsys/tools/drmon/display.cpp)).
  The blit already loops `y<h, x<w`, so it honours any size.
- **`scrBuffer`** — the off-screen composition buffer (`farmalloc`'d to `screenSize` in
  `SetupDisplay`, [`display.cpp:174-175`](../../devsys/tools/drmon/display.cpp)); layers composite
  here, then `CopyScreen(screen,scrBuffer,screenSize)`. **Follows `screenSize` automatically** as
  long as `SetupScreen` runs first.
- **`displayWidth`/`displayHeight`** — the *virtual* desktop. `InitDisplay`
  ([`display.cpp:204-216`](../../devsys/tools/drmon/display.cpp)) sets them to `screenWidth/Height`
  when the `virtualXSize/virtualYSize` config is 0 (the default). `AutoCenter` / window placement
  use these.
- **Chrome:** the menu bar is `new _layer(0,0,screenWidth,1,…)` rebuilt by `GetMenuBarLayer()`
  ([`monmenu.cpp:497`](../../devsys/tools/drmon/monmenu.cpp)) — so it re-spans whenever that's
  re-called. The desktop background fills `screenWidth*screenHeight` via `DisplayLayers`'
  `backgroundChar/Attr`. There is **no persistent bottom status line**; the clock is drawn into the
  menu-bar layer at a **fixed column 55** ([`menubar.cpp:101`](../../devsys/tools/drmon/menubar.cpp)).
- **Init order** ([`drmon.cpp:115-117`](../../devsys/tools/drmon/drmon.cpp)):
  `InitScreen()` → `InitDisplay()` → `SetupDisplay()`. `InitScreen` → `SetupScreen`.
- **Main loop** ([`drmon.cpp:178`](../../devsys/tools/drmon/drmon.cpp) `while(cont)`) calls
  `UpdateScreen()` at :213 — the natural place to poll a resize flag.

Most of the engine already reads `screenWidth/Height` *dynamically* (layer clipping
[`layer.cpp:256-273`](../../devsys/tools/drmon/layer.cpp), `GetScrAddr`
[`display.cpp:110-112`](../../devsys/tools/drmon/display.cpp)), so feeding it a bigger size mostly
"just works" — the hardcode is the only blocker.

## Approach

Two phases: **A** fills at startup (most of the value, simple); **B** adds live resize.

### Phase A — fill at startup

1. **ncurses backend** ([`linux/ncurses_io.cpp`](../../devsys/tools/drmon/linux/ncurses_io.cpp) +
   header): add `void drmon_nc_size(int *cols, int *rows)` returning ncurses `COLS`/`LINES`, clamped
   to a **floor of 80×25** (drmon's layouts assume ≥80×25) and a sane ceiling (e.g. 255×127 — keep
   buffers small; `screenWidth` is `unsigned short` so width itself is unbounded, but no point going
   silly). `initscr()` already ran in `drmon_nc_init`, so `COLS`/`LINES` are valid.
2. **`SetupScreen` `__GNUC__` branch** ([`screen.cpp:421-437`](../../devsys/tools/drmon/screen.cpp)):
   replace the `80/25` literals with `drmon_nc_size(&w,&h); screenWidth=w; screenHeight=h;`. Keep
   `screenWidth2/Height2 = 80×25` (the secondary mono screen — only used when `twoScreen`, which is
   false on Linux). `screen`/`screen2`/`screenSize` allocate to the new dims exactly as today.
3. `scrBuffer` (via `SetupDisplay`) and `displayWidth/Height` (via `InitDisplay`) then follow with
   **no change** — the existing init order already runs `SetupScreen` before both. The menu bar is
   built from `screenWidth` and the desktop fills `screenWidth*screenHeight`, so **chrome fills**.
   Windows keep their fixed sizes; auto-centred windows (`xPos=-1`) simply centre on the larger
   desktop (still "fixed" — never resized).

### Phase B — live resize (`KEY_RESIZE` / `SIGWINCH`)

4. **`pump()`** ([`ncurses_io.cpp`](../../devsys/tools/drmon/linux/ncurses_io.cpp)): when
   `getch()==KEY_RESIZE`, call `resizeterm(0,0)` (lets ncurses re-read the tty size), set a sticky
   `g_resized=1`, emit **no** key byte, and `return` (mirrors the `KEY_MOUSE` branch).
5. Add `int drmon_nc_resized(void)` → returns and **clears** `g_resized`.
6. **New `ReSizeViewport()`** (in `screen.cpp` or `display.cpp`): re-runs the size-dependent setup —
   `SetupScreen` (realloc `screen`/`screen2`, recompute `screenSize`), `SetupDisplay` (realloc
   `scrBuffer`), `InitDisplay` (recompute `displayWidth/Height`), `GetMenuBarLayer()` (re-span the
   menu bar), clamp any window whose origin now sits off-screen back into view (optional polish —
   layer clipping already prevents corruption, this just keeps them reachable), `ReSizeMouse()`,
   `refreshAll=TRUE`.
7. **Hook** into the main loop ([`drmon.cpp:178`](../../devsys/tools/drmon/drmon.cpp), before
   `UpdateScreen()`): `if(drmon_nc_resized()) ReSizeViewport();`.

### Polish (small, do alongside)

- **Right-align the clock**: draw it at `screenWidth - len` instead of the fixed col 55
  ([`menubar.cpp:101`](../../devsys/tools/drmon/menubar.cpp)) so it sits top-right on wide terminals
  instead of stranded mid-bar.
- **Min-size**: if the terminal is < 80×25, keep 80×25 (content clips); optionally a one-line hint.

## Risks / notes

- `screenWidth/Height` are `unsigned short`; cell math is `int`; mouse fine coords are cell×8 with
  cell in `short` — all fine well past any real terminal size.
- `twoScreen` stays false on Linux; `screen2` stays 80×25 (unreferenced unless `twoScreen`).
- Shrinking below open windows: layer clipping already guards the buffer; windows just clip. The
  optional clamp in step 6 keeps them grabbable.
- Unlike the mouse, **this IS headlessly testable** — `tmux new-session -x W -y H` and
  `tmux resize-window` exercise both startup-fill and live-resize through the render path (the part
  tmux mangles is only mouse input).

## Verification

1. **Build** — Docker toolchain, 0 errors; links.
2. **80×25 regression** — `task shot` (tmux 80×25) renders exactly as today (menu bar + windows unchanged).
3. **Startup fill** — `tmux new-session -x 120 -y 40`, run drmon, capture → menu bar spans all 120
   columns, desktop background fills all 40 rows, clock visible (top-right after polish), an opened
   window centres on the larger desktop.
4. **Live resize** — start in tmux at 80×25, `tmux resize-window -x 120 -y 40`, capture → drmon
   re-fills to 120×40 without restart; resize back to 90×30, capture → re-fills again; no crash, no
   stale 80-column island.
5. **Shrink** — resize to 70×20 → no crash; content clips cleanly; resizing back restores full fill.
6. **Keyboard/menus still work** after a resize (F10 → File dropdown) — no regression.
