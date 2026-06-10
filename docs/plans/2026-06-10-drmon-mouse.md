# drmon mouse support (Linux ncurses)

**Date:** 2026-06-10
**Status:** Done — mouse wired; hover-highlight verified live

## Context

drmon's UI already fully handles the mouse: `InputPending`
([`devsys/tools/drmon/input.cpp`](../../devsys/tools/drmon/input.cpp) ~217-262) polls
`GetMouse(&fineX,&fineY) → buttonmask` every frame, divides fine coords by 8 for cell
coords, edge-detects button/position changes, and emits `INP_MOUSE_LEFTBUTTON_DOWN/UP`,
`RIGHTBUTTON_*`, `*_SHIFTDOWN`, `INP_MOUSEMOVE` — which windows/gadgets/menus already
consume (`GadgetHit` in `gadget.cpp`, menu-bar clicks in `menubar.cpp`, title-bar drag).
Only the bottom layer is missing: on Linux `CheckMouse`/`GetMouse`/`SetMouseBounds`
([`mouse.cpp`](../../devsys/tools/drmon/mouse.cpp)) sit behind `#ifdef DO_MOUSE` (undefined)
→ the `#else` returns 0 → `haveMouse=false`. Wire ncurses `getmouse()` into those three and
the whole click/drag/menu UI lights up.

## Approach

drmon **polls** mouse state each frame; ncurses delivers it as discrete `KEY_MOUSE` events
via `getch`. So the ncurses backend keeps a **persistent** mouse state (cell x/y + button
mask), updated whenever a `KEY_MOUSE` event arrives in `pump()`, and `GetMouse` returns that
snapshot. ncurses gives cell coords; return `fineX=x*8, fineY=y*8` so drmon's existing `/8`
recovers the cell — **no change to `input.cpp`**. Button state is **sticky** (set on PRESS,
cleared on RELEASE) so it survives motion → drmon's edge detection yields DOWN/UP/drag.

## Changes (3 files; all UTF-8-safe)

1. **`linux/ncurses_io.cpp`**
   - `drmon_nc_init`: `mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL)`,
     `mouseinterval(0)` (raw press/release, no click-synthesis), enable button-drag + SGR
     reporting (`\e[?1002;1006h`; `\e[?1003h` for hover-follow pointer). `drmon_nc_shutdown`
     disables them (`\e[?1002;1003;1006l`).
   - `pump()`: `getch()==KEY_MOUSE` → `getmouse(&ev)`; `g_mx=ev.x`, `g_my=ev.y`;
     `BUTTON1_PRESSED`→`g_mbtn|=0x01`, `BUTTON1_RELEASED`→`&=~0x01`, `BUTTON3_*`→`0x02`. No key byte.
   - New: `drmon_nc_havemouse()` → 1; `drmon_nc_getmouse(short*fx,short*fy)` → `*fx=g_mx*8`,
     `*fy=g_my*8`, return `g_mbtn`.
2. **`linux/include/ncurses_io.h`** — declare the two new fns.
3. **`mouse.cpp`** — add `#elif defined(__GNUC__)` branches to `CheckMouse`
   (→ `drmon_nc_havemouse()?2:0`), `GetMouse` (→ `drmon_nc_getmouse`), `SetMouseBounds`
   (no-op), beside the existing `#ifdef DO_MOUSE … #else return 0`. Include the backend header.

`MOUSEF_BLEFT=0x01`/`MOUSEF_BRIGHT=0x02` and `INP_MOUSE_*` are already in `input.hpp`.

## Root-cause finding (2026-06-10): hover-highlight needs mode 1003

After the first cut, **clicks worked but hovering over a menu item did not highlight it.**
That highlight is an *original* drmon feature — [`menu.cpp:328`](../../devsys/tools/drmon/menu.cpp)
handles `INP_MOUSEMOVE` by un-highlighting the old option and highlighting whichever item the
pointer is over. So this was a port bug, not a missing feature: pure pointer **motion** (no
button held) was never reaching drmon.

Cause: the container's `xterm-256color` terminfo has **no `XM` capability**
(`infocmp` shows only `kmous=\E[<`). With `XM` absent, ncurses 6.4 falls back to its built-in
default which enables **only xterm mode 1000** (button press/release). `REPORT_MOUSE_POSITION`
in the `mousemask` makes ncurses willing to *return* motion events, but the terminal is never
told to *send* them — no `\E[?1003h` is emitted. Hence clicks (1000) worked, hover (1003) didn't.

Fix: after `mousemask`, explicitly `fputs("\033[?1003h\033[?1006h", stdout)` (any-motion +
SGR coords, matching `kmous=\E[<`); `drmon_nc_shutdown` emits `\033[?1003l\033[?1006l` so no
stray reporting leaks to the shell. (The implementation had dropped the explicit enable the
"Changes" section above specified; this restores it.)

## Verification

**Harness note:** the `task shot` tmux harness **cannot** verify the mouse. `tmux send-keys`
does not deliver mouse SGR sequences to the inner app's ncurses (they're swallowed/mangled by
tmux's own input parser — confirmed: injected `\E[<…M` never produces a `KEY_MOUSE` in `pump()`,
it leaks through as a bare `ESC` that aborts the open menu). `task run` is **tmux-less**
(`docker run -it … ./drmon` straight to the real terminal), which is why the mouse works there
but not under tmux. So mouse verification is **live, by hand**; only build + keyboard-regression
are headless.

1. **Build** via the Docker toolchain — 0 errors; links. — **PASS** (clean build; only the
   pre-existing `tmpnam` linker warning from `profile.cpp`).
2. **Keyboard no-regression** (headless `task shot` path): F10 → Enter opens the File dropdown
   (Load… / Execute Script… / Exit) and arrows highlight items. — **PASS** (verified during
   diagnosis; dropdown renders correctly and the default highlight tracks arrow keys).
3. **[live] Click test** — `task run`; click "File" in the menu bar → dropdown opens; click an
   item → it runs. — **PASS** (user: "it opens the menu for me just fine and i can choose menu items").
4. **[live] Hover-highlight** — with a dropdown open, move the mouse over items → the highlight
   follows the pointer (the fix above). — **PASS** (user, 2026-06-10: "it works now, highlight
   follows the mouse").
5. **[live] Drag test** — press on a window title bar, move to a new column, release → window moves.
   — not separately exercised; rides the same now-verified pipeline (button events + motion both
   flow), so expected to work. Revisit if a drag issue surfaces.
6. **[live] Clean exit** — quit drmon; the shell shows no stray mouse escapes (1003l/1006l sent
   on shutdown). — mechanism in place (`drmon_nc_shutdown` emits the disables before `endwin`);
   not separately observed by the user.
