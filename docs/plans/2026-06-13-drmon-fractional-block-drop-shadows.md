# Fractional-block drop shadows for drmon windows

## Context

Every drmon window/menu/dialog (`_window` → `_shadowedLayer`) casts a drop shadow.
The user wants that shadow drawn with **fractional (half / quarter) block characters**
instead of the current solid look.

**Why it looks like a "full block" today:** the shadow doesn't actually draw a block
glyph at all. `_shadowedLayer::RenderShadow` (`devsys/tools/drmon/layer.cpp:294`) takes
the window's footprint, shifts it by `(xShadow=2, yShadow=1)` (`config.cpp:294`), and for
every cell in that shifted rect it **keeps the underlying character but darkens its colour
attribute** through `shadowTable` (`layer.cpp:288`). The desktop is the blue light‑shade
field (`config.cpp:215`: `backgroundChar='\xB0'` = `░`, `backgroundAttr=ATTR_BLUE`).
`shadowTable` maps blue→black and black→black, so over the desktop the `░` becomes
**black‑on‑black = a solid black 2×1 L‑slab** — read as a "full block."

**Goal:** replace that solid slab with a thin half‑cell drop shadow drawn from the
half‑block glyphs `▀`/`▌`, with a pixel‑perfect quarter‑block (`▘`) corner.

### Rendering pipeline & glyph availability (verified against the repo)

The build is the Linux ncurses port (DOS is dead — not a concern). The video buffer is a
grid of `{char, attr}` cells (`screen.hpp:5`, `charSize 2`) where the char byte indexes a
256‑entry `kCp437[256]` table (`linux/ncurses_io.cpp:248`) that maps it to Unicode for
`mvadd_wch`. Every glyph drmon can draw is whatever those 256 entries map to.

- The table already maps the **half‑blocks** `▀`(0xDF) `▌`(0xDD) `▄`(0xDC) `▐`(0xDE) and
  **shades** `░ ▒ ▓` — so `▀`/`▌` need no table change.
- It does **not** map any **quadrant** `▖▗▘▝…` (U+2596…U+259F) — confirmed by
  `grep '0x259[6-9A-F]' ncurses_io.cpp` → no matches (quadrants were never in CP437). So to
  draw `▘`, claim one free byte (`0xFF`, the unused nbsp slot → `U+00A0`) and remap that one
  `kCp437` entry to `U+2598`. One‑line change.

## Design — thin half‑block "hug" shadow with a quarter‑block corner

The shadow ink **hugs** the window edges (its near edge touches the border; it extends
outward by half a cell, encoded in the glyph orientation):

- **Bottom strip** — row directly below the window, glyph `▀` (upper‑half, 0xDF): dark half
  at the *top* of the cell, touching the window's bottom border. Stops under the window's
  right edge (does not run into the corner column).
- **Right strip** — column directly right of the window, glyph `▌` (left‑half, 0xDD): dark
  half at the *left* of the cell, touching the window's right border. Stops at the window's
  bottom edge (does not run into the corner row).
- **Corner** — single cell at the window's lower‑right diagonal, glyph `▘` (upper‑left
  quarter, private byte `0xFF` → `U+2598`): the mathematically exact corner of a half‑cell
  offset shadow. No overhang, no nub.
- **Ink colour** — dark grey on black (CGA attr `0x08`). On ncurses, fg index 8 →
  `COLOR_BLACK` + bold = grey (`ncurses_io.cpp:283`, the `0x08`→`A_BOLD` path).
- **Directional offset** — strips inset one cell at the top‑right and bottom‑left (right
  strip starts one row below the window top; bottom strip starts one col right of the
  window left), preserving the "light from upper‑left" look.
- **Over other windows** — where the shadow falls on a window behind it (cell char ≠
  `backgroundChar`), keep the existing behaviour: darken the attribute via `shadowTable`,
  leaving the character intact, so it reads as a shadow cast onto that window rather than
  stamping grey blocks over its content. The block glyphs are only written over desktop
  background cells.

### Before / after (window over the blue `░` desktop)

Current — solid black 2×1 L‑slab (`█` = the black‑on‑black cells the user sees):

```
░┌──────────┐░░░░░
░│ drmon    │██░░░
░│ ──────   │██░░░
░└──────────┘██░░░
░░░████████████░░░
░░░░░░░░░░░░░░░░░░
```

New — thin half‑block hug, `▘` quarter‑block corner:

```
░┌──────────┐░░░░░
░│ drmon    │▌░░░░
░│ ──────   │▌░░░░
░└──────────┘▌░░░░
░░▀▀▀▀▀▀▀▀▀▀▀▘░░░░
░░░░░░░░░░░░░░░░░░
```

(The `▀` run stops under the window's right edge; the right `▌` stops at the bottom edge;
the lone corner cell is `▘` — a perfect upper‑left quarter, no overhang.)

### Geometry (screen coords; window left=L, right=R, top=T, bottom=B)

- Right strip: column `R+1`, rows `T+1 … B`, glyph `▌` (0xDD).
- Bottom strip: row `B+1`, columns `L+1 … R`, glyph `▀` (0xDF).
- Corner: cell `(R+1, B+1)`, glyph `▘` (private byte `0xFF` → `U+2598`).
- Each cell clipped to `[0,screenWidth) × [0,screenHeight)`.

## Implementation

Two files.

### 1. `devsys/tools/drmon/linux/ncurses_io.cpp` — one table entry

Remap the unused `0xFF` slot in `kCp437` (line 265, last entry) from `0x00A0` (nbsp) to
`0x2598` (`▘`), with a comment noting it's drmon's private quarter‑block code for the
shadow corner. (No other byte changes.)

### 2. `devsys/tools/drmon/layer.cpp` — the shadow

No header signature change — `RenderShadow` keeps its
`(buffer, xOffset, yOffset, screenWidth, screenHeight)` signature; only its body and the
meaning of the passed offset change.

1. **`_shadowedLayer::Render`** (`layer.cpp:72`): pass the **un‑shifted** offset to
   `RenderShadow` (strips are computed from the window edges, not a shifted footprint), and
   guard the documented "0 == none": skip the shadow when `xShadow == 0 && yShadow == 0`.

   ```cpp
   void _shadowedLayer::Render(char* buffer, int xOffset, int yOffset, int sw, int sh) {
       if (xShadow || yShadow)
           RenderShadow(buffer, xOffset, yOffset, sw, sh);
       _layer::Render(buffer, xOffset, yOffset, sw, sh);
   }
   ```

2. **`_shadowedLayer::RenderShadow`** (`layer.cpp:294`): rewrite to draw the two strips +
   corner. Add named constants near the existing `shadowTable` (`layer.cpp:288`):

   ```cpp
   static const ubyte kShadowBottom = 0xDF;  // ▀ upper-half block
   static const ubyte kShadowRight  = 0xDD;  // ▌ left-half block
   static const ubyte kShadowCorner = 0xFF;  // ▘ upper-left quarter (private kCp437 code)
   static const ubyte kShadowInk    = 0x08;  // dark grey fg on black bg
   ```

   Per target cell `(cx, cy)` with a chosen glyph (bounds‑clipped):

   ```cpp
   if (cx < 0 || cx >= screenWidth || cy < 0 || cy >= screenHeight) return;
   ubyte* cell = (ubyte*)dest + (cy*screenWidth + cx)*charSize;   // [char][attr]
   if (cell[0] == (ubyte)backgroundChar) {        // over the desktop → draw a block
       cell[0] = glyph;
       cell[1] = kShadowInk;
   } else {                                       // over a window behind → darken as before
       ubyte a = cell[1];
       cell[1] = shadowTable[a & 0xf] | (shadowTable[a>>4] << 4);
   }
   ```

   - Compute `L=xPos+xOffset`, `T=yPos+yOffset`, `R=L+xSize-1`, `B=T+ySize-1`.
   - Right strip: `for (cy=T+1; cy<=B; ++cy) put(R+1, cy, kShadowRight);`
   - Bottom strip: `for (cx=L+1; cx<=R; ++cx) put(cx, B+1, kShadowBottom);`
   - Corner: `put(R+1, B+1, kShadowCorner);`

   Keep `shadowTable` (still used by the over‑window path).

### Config

`xShadow`/`yShadow` (`config.cpp:294`, defaults `2`/`1`; `.ini` keys `SHADOWXOFFSET`/
`SHADOWYOFFSET`) are retained as the on/off control — any non‑zero value draws the thin
shadow, `0/0` disables it. The magnitude no longer affects thickness (the half‑block shadow
is inherently one cell); leave the defaults unchanged so existing `.ini` files keep
working, and note this in the comment at `config.hpp:13`.

### Notes / minor edge cases

- A window whose *content* legitimately contains a `0xB0` (`░`) char in a shadowed cell
  would get a block drawn over it instead of darkened — rare (content is text) and
  cosmetically negligible; documented rather than guarded.
- All shadowed surfaces (windows, menus, dialogs, file requester, about box) inherit this
  from `_shadowedLayer`, so the one change covers every drop shadow.

## Verification

Run from the repo root (build/run happen in the throwaway Docker toolchain image):

1. **Build cleanly**
   ```
   task build
   ```
   Expect: `ninja` links `snesmon` and `genmon` with no warnings/errors.

2. **Render every window type to PNG and inspect the shadow**
   ```
   task screenshots
   ```
   Expect: PNGs in `devsys/tools/drmon/docs/img/`. In each, the drop shadow is a thin grey
   band hugging the window's bottom (`▀`) and right (`▌`) edges, with a clean `▘`
   quarter‑block at the bottom‑right corner (no slab, no overhang) and the classic inset at
   top‑right / bottom‑left.

3. **Headless text dump (glyph‑level check)**
   ```
   task shot
   ```
   Expect: the dumped TUI shows `▀`/`▌`/`▘` at the window edges (not `█` or blanks),
   confirming both the half‑block mapping and the new `0xFF`→`▘` entry render correctly.

4. **Shadow over an overlapping window** — in `task run`, open two overlapping windows so
   one shadow falls on the other. Expect: the shadow over the other window darkens its cells
   (text still legible, just dimmed), not blocks stamped over its content.

5. **Smoke test (no regressions / crashes)**
   ```
   task smoke
   ```
   Expect: every window opens without crashing.

Paste raw output under each step and mark PASS/FAIL when executing.

## Follow‑ups on execution (project conventions)

- Per the project's plan‑first rule, on implementation also create
  `docs/plans/2026-06-13-fractional-block-window-shadows.md` (this plan, repo‑local) and a
  `TODO.md` entry, then a `[verify]` item linked to the verification steps above.
- Commit touches only `layer.cpp`, the one `kCp437` entry in `ncurses_io.cpp`, the one‑line
  `config.hpp:13` comment, and the plan/TODO docs.
