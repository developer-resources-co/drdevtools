#!/usr/bin/env python3
"""Render an ANSI-escaped tmux pane dump to a trimmed PNG using Pillow.

Usage: render_ansi.py input.ansi output.png [scale]
  scale: pixel multiplier (default 2 for crisp 2x rendering)

Trims to the window bounding box by detecting the desktop background colour
from the top-left cell of the content area (row 1, col 0), then cropping to
the smallest rectangle of non-desktop cells in rows 1..(n-2), skipping the
menu bar (row 0) and status bar (last row).
"""
import sys
from PIL import Image, ImageDraw, ImageFont

FONT_PATH = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"
FONT_SIZE = 14

# Standard 16-colour ANSI palette (CGA-compatible)
PALETTE = [
    (0, 0, 0),      (170, 0, 0),    (0, 170, 0),    (170, 170, 0),
    (0, 0, 170),    (170, 0, 170),  (0, 170, 170),  (170, 170, 170),
    (85, 85, 85),   (255, 85, 85),  (85, 255, 85),  (255, 255, 85),
    (85, 85, 255),  (255, 85, 255), (85, 255, 255), (255, 255, 255),
]
DEFAULT_FG = PALETTE[7]
DEFAULT_BG = PALETTE[0]


def parse_ansi(text):
    """Parse ANSI-escaped terminal text into a grid.
    Returns list of rows; each row is a list of (char, fg_rgb, bg_rgb).
    """
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

    # Normalise all rows to the same column count
    n_cols = max((len(r) for r in rows), default=80)
    desktop_bg = rows[1][0][2] if len(rows) > 1 and rows[1] else DEFAULT_BG
    padded = [list(r) + [(' ', DEFAULT_FG, desktop_bg)] * (n_cols - len(r))
              for r in rows]

    # Find the bounding box of non-desktop cells, ignoring menu bar (row 0)
    # and status bar (last row) so we crop to just the open window.
    r1 = r2 = c1 = c2 = None
    for ri in range(1, len(padded) - 1):
        for ci in range(n_cols):
            if padded[ri][ci][2] != desktop_bg:
                if r1 is None:
                    r1 = ri
                r2 = ri
                if c1 is None or ci < c1:
                    c1 = ci
                if c2 is None or ci > c2:
                    c2 = ci

    if r1 is None:
        # No window open — fall back to full content area
        r1, r2, c1, c2 = 1, len(padded) - 2, 0, n_cols - 1

    crop = [padded[ri][c1:c2 + 1] for ri in range(r1, r2 + 1)]
    n_rows = len(crop)
    n_cols_out = c2 - c1 + 1

    img = Image.new("RGB", (n_cols_out * cell_w, n_rows * cell_h), desktop_bg)
    draw = ImageDraw.Draw(img)

    for ri, row in enumerate(crop):
        for ci, (ch, fg, bg) in enumerate(row):
            x = ci * cell_w
            y = ri * cell_h
            draw.rectangle([x, y, x + cell_w, y + cell_h], fill=bg)
            if ch != ' ':
                # default anchor "la" (left-ascender): top of the em-square at y
                draw.text((x, y), ch, font=font, fill=fg)

    if scale != 1:
        img = img.resize((img.width * scale, img.height * scale), Image.NEAREST)

    img.save(out_file)
    print(f"  {ansi_file} → {out_file}  ({n_cols_out}×{n_rows} cells, "
          f"{img.width}×{img.height}px)")


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} input.ansi output.png [scale]")
        sys.exit(1)
    render(sys.argv[1], sys.argv[2], scale=int(sys.argv[3]) if len(sys.argv) > 3 else 2)
