# Visible text-field caret (with insert/overwrite distinction) — ncurses

**Status:** ✅ implemented + verified 2026-06-12. A *feature*, follow-on from the dead-arm cleanup's
`SetCursorStartStop` ripout ([plan](2026-06-12-drop-dead-dos-compiler-ifdef-arms.md)).

## Context

The ncurses port has **no visible caret in string gadgets** (Expression/Command/file-name fields).
The terminal hardware cursor is hidden for the whole session (`curs_set(0)` in `drmon_nc_init`),
`drmon_nc_blit` draws no caret, and `DrawGadgets` only paints the field text. The DOS build showed
the caret via the hardware text cursor, whose *shape* (`SetCursorStartStop` + the layer
`cStartLine`/`cStopLine` = `4` insert / `6` overwrite) gave insert-vs-overwrite feedback — but that
was BIOS `INT 10h/AH=01h` and already non-functional on Linux, so it was removed as dead.

This adds a **self-drawn caret** (Approach B — fully portable, no DECSCUSR): keep `curs_set(0)` and
overlay the caret as an ncurses attribute in the blit. Fixes the missing-caret gap *and* restores the
insert/overwrite distinction.

## Why the blit, not the framebuffer

`UpdateScreen()` regenerates the front buffer every flush (`CopyScreen(screen, scrBuffer, …)` then
`drmon_nc_blit(screen, …)`), so a caret overlaid at blit time needs **no save/restore / erase** — the
next flush starts clean. And the CGA attr byte can't express underline (CGA color text has no
underline bit), but **ncurses can** — so the caret is applied as a real ncurses attribute at render
time, not encoded into the CGA cell:
- **overwrite** → `A_REVERSE` (block-style highlight, matching the dump-window line highlight idiom)
- **insert** → `A_UNDERLINE` (thin underline; widely supported, degrades to nothing if a terminal lacks it)

## Design / changes

- **`linux/ncurses_io.cpp` `drmon_nc_blit`** — add params `int caretX, int caretY, int caretMode`
  (`-1` none, `0` overwrite, `1` insert). In the render loop, when `caretMode>=0 && x==caretX &&
  y==caretY`, OR `a |= (caretMode==1 ? A_UNDERLINE : A_REVERSE)` before `setcchar`. Update the
  prototype in `linux/include/ncurses_io.h`.
- **`display.cpp` `UpdateScreen`** — compute `caretMode = curOn ? (cursorInsert ? 1 : 0) : -1` and
  pass `cursorX, cursorY, caretMode` to the blit. `curOn`/`cursorX`/`cursorY` are the existing
  screen-level cursor globals (set by `PositionCursor`); `curOn` is true only while a string gadget is
  active, so the caret never shows in dump/other windows.
- **`screen.cpp` / `screen.hpp`** — add `int cursorInsert;` global (+ extern), the slim modern
  replacement for the removed `cStartLine`/`cStopLine` (a mode bit, not DOS scanlines).
- **`gadget.cpp`** — set `cursorInsert = (gPtr->mode & STRGADF_MODE_INSERT) ? 1 : 0;` exactly where
  the old `cStartLine = 4/6` was: in `ActivateStrGadget` and the `KEY_INSERT` toggle (the
  `STRGADF_MODE_INSERT` flag itself already drives insert-vs-overwrite text editing — this just
  mirrors it to the caret shape).

Non-blinking (a static reverse/underline cell, redrawn each `UpdateScreen`). Primary screen only
(the secondary mono screen isn't blitted to ncurses). Bounds are free — the blit already clips to
`COLS`/`LINES`, and an off-screen caret position simply matches no cell.

## Verification

1. `task build` — `snesmon` + `genmon` clean.
2. `task smoke SYS=snes` / `SYS=gen` — both PASS.
3. **Visual (headless capture):** open Expression (Alt+E), type `12+34`, capture with
   `tmux capture-pane -e` (includes attributes). **PASS** — the cell after the text carries the caret
   attribute, and `Insert` flips it:
   ```
   before Insert:  …12+34^[[7m …   (reverse block = overwrite)
   after  Insert:  …12+34^[[4m …   (underline = insert)
   ```
4. Caret gated by `curOn` (only active string gadgets), so it doesn't leak into dump/other windows.
