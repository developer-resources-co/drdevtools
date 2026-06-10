#!/usr/bin/env python3
"""Render an ANSI-escaped tmux pane dump to a trimmed PNG using Pillow.

Usage: render_ansi.py input.ansi output.png [scale]
  scale: pixel multiplier (default 2 for crisp 2x rendering)

Returns (prints) the recommended HTML display width on stdout alongside the
normal progress line so callers can embed correct <img width="…"> tags.

Crop strategy:
  - drmon's desktop is always ANSI blue (0,0,170); hard-coded — sampling from a
    single cell fails when the window sits at row 1 col 0 (cells are window-bg).
  - Crop bounding box = smallest rectangle of non-desktop cells in rows 1..n-2,
    skipping the menu bar (row 0) and status bar (last row).
  - Drop shadow (1-2 cells of all-black+space on the right and bottom edges of
    the crop) is stripped after the initial bounding-box pass.
"""
import sys
from PIL import Image, ImageDraw, ImageFont

FONT_PATH = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"
FONT_SIZE = 14

PALETTE = [
    (0, 0, 0),      (170, 0, 0),    (0, 170, 0),    (170, 170, 0),
    (0, 0, 170),    (170, 0, 170),  (0, 170, 170),  (170, 170, 170),
    (85, 85, 85),   (255, 85, 85),  (85, 255, 85),  (255, 255, 85),
    (85, 85, 255),  (255, 85, 255), (85, 255, 255), (255, 255, 255),
]
DEFAULT_FG  = PALETTE[7]
DEFAULT_BG  = PALETTE[0]           # black  — window/title background
DESKTOP_BG  = PALETTE[4]           # blue   — drmon desktop, always this colour


def parse_ansi(text):
    rows, row = [], []
    fg, bg, bold = DEFAULT_FG, DEFAULT_BG, False
    i = 0
    while i < len(text):
        c = text[i]
        if c == '\x1b' and i + 1 < len(text) and text[i + 1] == '[':
            j = i + 2
            while j < len(text) and (text[j].isdigit() or text[j] == ';'):
                j += 1
            if j < len(text):
                params_str = text[i + 2:j]
                if text[j] == 'm':
                    parts = ([int(x) if x else 0 for x in params_str.split(';')]
                             if params_str else [0])
                    for p in parts:
                        if p == 0:
                            fg, bg, bold = DEFAULT_FG, DEFAULT_BG, False
                        elif p == 1:
                            bold = True
                        elif p == 22:
                            bold = False
                        elif 30 <= p <= 37:
                            fg = PALETTE[(p - 30) + (8 if bold else 0)]
                        elif p == 39:
                            fg = DEFAULT_FG
                        elif 40 <= p <= 47:
                            bg = PALETTE[p - 40]
                        elif p == 49:
                            bg = DEFAULT_BG
                        elif 90 <= p <= 97:
                            fg = PALETTE[p - 90 + 8]
                        elif 100 <= p <= 107:
                            bg = PALETTE[p - 100 + 8]
                i = j + 1
                continue
            i += 1
            continue
        if c == '\x1b':
            i += 2
            continue
        if c == '\r':
            i += 1
            continue
        if c == '\n':
            rows.append(row)
            row = []
            i += 1
            continue
        row.append((c, fg, bg))
        i += 1
    if row:
        rows.append(row)
    return rows


def _is_shadow_strip(cells):
    """True if every cell is black+space — the drmon drop-shadow pattern."""
    return all(bg == DEFAULT_BG and ch == ' ' for ch, _, bg in cells)


def render(ansi_file, out_file, scale=2):
    with open(ansi_file, 'r', encoding='utf-8', errors='replace') as f:
        text = f.read()

    rows = parse_ansi(text)
    if not rows:
        print(f"  {ansi_file}: no content, skipping")
        return

    font = ImageFont.truetype(FONT_PATH, FONT_SIZE)
    ascent, descent = font.getmetrics()
    cell_h = ascent + descent
    cell_w = round(font.getlength("M"))

    n_cols = max((len(r) for r in rows), default=80)
    padded = [list(r) + [(' ', DEFAULT_FG, DESKTOP_BG)] * (n_cols - len(r))
              for r in rows]

    # --- bounding box of non-desktop cells (skip menu bar row 0 + status bar) ---
    r1 = r2 = c1 = c2 = None
    for ri in range(1, len(padded) - 1):
        for ci in range(n_cols):
            if padded[ri][ci][2] != DESKTOP_BG:
                if r1 is None:
                    r1 = ri
                r2 = ri
                if c1 is None or ci < c1:
                    c1 = ci
                if c2 is None or ci > c2:
                    c2 = ci

    if r1 is None:
        r1, r2, c1, c2 = 1, len(padded) - 2, 0, n_cols - 1

    # --- strip drop shadow (all-black+space strips on right and bottom edges) ---
    while r2 > r1 and _is_shadow_strip(
            [padded[r2][ci] for ci in range(c1, c2 + 1)]):
        r2 -= 1
    while c2 > c1 and _is_shadow_strip(
            [padded[ri][c2] for ri in range(r1, r2 + 1)]):
        c2 -= 1

    crop = [padded[ri][c1:c2 + 1] for ri in range(r1, r2 + 1)]
    n_rows_out = len(crop)
    n_cols_out = c2 - c1 + 1

    img = Image.new("RGB", (n_cols_out * cell_w, n_rows_out * cell_h), DESKTOP_BG)
    draw = ImageDraw.Draw(img)

    for ri, row in enumerate(crop):
        for ci, (ch, fg, bg) in enumerate(row):
            x = ci * cell_w
            y = ri * cell_h
            draw.rectangle([x, y, x + cell_w, y + cell_h], fill=bg)
            if ch != ' ':
                draw.text((x, y), ch, font=font, fill=fg)

    if scale != 1:
        img = img.resize((img.width * scale, img.height * scale), Image.NEAREST)

    img.save(out_file)

    display_w = min(img.width // scale, 700)
    print(f"  {ansi_file} → {out_file}  "
          f"({n_cols_out}×{n_rows_out} cells, {img.width}×{img.height}px, "
          f"display_w={display_w})")
    return display_w


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} input.ansi output.png [scale]")
        sys.exit(1)
    render(sys.argv[1], sys.argv[2], scale=int(sys.argv[3]) if len(sys.argv) > 3 else 2)
