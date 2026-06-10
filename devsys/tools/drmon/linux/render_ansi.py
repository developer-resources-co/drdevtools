#!/usr/bin/env python3
"""Render an ANSI-escaped tmux pane dump to a trimmed PNG using Pillow.

Usage: render_ansi.py input.ansi output.png [scale]
  scale: pixel multiplier (default 2 for crisp 2x rendering)
"""
import sys, re
from PIL import Image, ImageDraw, ImageFont

# Standard 16-colour ANSI palette (CGA-compatible)
PALETTE = [
    (0,0,0),       (170,0,0),     (0,170,0),     (170,170,0),
    (0,0,170),     (170,0,170),   (0,170,170),    (170,170,170),
    (85,85,85),    (255,85,85),   (85,255,85),    (255,255,85),
    (85,85,255),   (255,85,255),  (85,255,255),   (255,255,255),
]
DEFAULT_FG = PALETTE[7]
DEFAULT_BG = PALETTE[0]


def parse_ansi(text):
    """Parse ANSI-escaped terminal text.
    Returns a list of rows; each row is a list of (char, fg_rgb, bg_rgb).
    """
    rows = []
    row = []
    fg, bg, bold = DEFAULT_FG, DEFAULT_BG, False

    i = 0
    while i < len(text):
        c = text[i]

        if c == '\x1b' and i + 1 < len(text) and text[i+1] == '[':
            # CSI sequence — find the final byte
            j = i + 2
            while j < len(text) and (text[j].isdigit() or text[j] == ';'):
                j += 1
            if j < len(text):
                final = text[j]
                params_str = text[i+2:j]
                if final == 'm':
                    parts = [int(x) if x else 0 for x in params_str.split(';')] if params_str else [0]
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
            # Non-CSI ESC sequence — skip next char
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


def is_blank_row(row, desktop_bg):
    return all(bg == desktop_bg and ch in (' ', '') for ch, fg, bg in row)


def trim_rows(rows, desktop_bg):
    """Remove leading and trailing rows that are entirely desktop_bg."""
    first, last = 0, len(rows)
    while first < last and is_blank_row(rows[first], desktop_bg):
        first += 1
    while last > first and is_blank_row(rows[last - 1], desktop_bg):
        last -= 1
    return rows[first:last]


def trim_cols(rows):
    """Remove leading and trailing columns that are entirely default BG."""
    if not rows:
        return rows
    max_cols = max(len(r) for r in rows)
    # pad to uniform width
    padded = [r + [(' ', DEFAULT_FG, DEFAULT_BG)] * (max_cols - len(r)) for r in rows]

    first_col = 0
    while first_col < max_cols and all(bg == DEFAULT_BG and ch == ' ' for ch, fg, bg in [r[first_col] for r in padded]):
        first_col += 1

    last_col = max_cols
    while last_col > first_col and all(bg == DEFAULT_BG and ch == ' ' for ch, fg, bg in [r[last_col-1] for r in padded]):
        last_col -= 1

    return [r[first_col:last_col] for r in padded]


def render(ansi_file, out_file, scale=2, pad=4):
    with open(ansi_file, 'r', encoding='utf-8', errors='replace') as f:
        text = f.read()

    rows = parse_ansi(text)
    if not rows:
        print(f"  {ansi_file}: no content, skipping")
        return

    # Determine the desktop background: most-common bg in the last row
    # (status bar or blank desktop row)
    last_row = rows[-1] if rows else []
    bg_counts = {}
    for _, _, bg in last_row:
        bg_counts[bg] = bg_counts.get(bg, 0) + 1
    desktop_bg = max(bg_counts, key=bg_counts.get) if bg_counts else DEFAULT_BG

    rows = trim_rows(rows, desktop_bg)
    if not rows:
        print(f"  {ansi_file}: all blank after trim, skipping")
        return

    font_path = "/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf"
    base_size = 14
    font = ImageFont.truetype(font_path, base_size)

    # Measure a single cell using 'M' (should be monospaced)
    bbox = font.getbbox("M")
    cell_w = bbox[2] - bbox[0]
    cell_h = int(base_size * 1.35)      # line height with a bit of leading
    baseline_off = -bbox[1]             # positive offset to correct for ascender

    # Normalise rows to same column count
    n_cols = max(len(r) for r in rows)
    for r in rows:
        while len(r) < n_cols:
            r.append((' ', DEFAULT_FG, desktop_bg))

    n_rows = len(rows)
    w = n_cols * cell_w + pad * 2
    h = n_rows * cell_h + pad * 2

    img = Image.new("RGB", (w, h), desktop_bg)
    draw = ImageDraw.Draw(img)

    for ri, row in enumerate(rows):
        for ci, (ch, fg, bg) in enumerate(row):
            x = pad + ci * cell_w
            y = pad + ri * cell_h
            draw.rectangle([x, y, x + cell_w, y + cell_h], fill=bg)
            if ch and ch != ' ':
                draw.text((x, y + baseline_off), ch, font=font, fill=fg)

    if scale != 1:
        img = img.resize((img.width * scale, img.height * scale), Image.NEAREST)

    img.save(out_file)
    print(f"  {ansi_file} → {out_file}  ({n_cols}×{n_rows} cells, {img.width}×{img.height}px)")


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} input.ansi output.png [scale]")
        sys.exit(1)
    scale = int(sys.argv[3]) if len(sys.argv) > 3 else 2
    render(sys.argv[1], sys.argv[2], scale=scale)
