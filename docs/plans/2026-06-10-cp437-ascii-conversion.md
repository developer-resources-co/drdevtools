# Convert drmon CP437 high bytes → ASCII escapes / UTF-8

**Date:** 2026-06-10
**Status:** Planned

## Context

Many drmon source files contain **literal IBM CP437 / extended-ASCII bytes** (box-drawing
border glyphs, shades, the `█` logo, the ASCII-chart data table, default config chars, and
box-art in comments). These bytes are **not valid UTF-8**. The Claude Edit/Write tools
round-trip files through UTF-8 and **silently replace every such byte with `U+FFFD` (�)** —
which already happened once (`menu.cpp`'s border string, fixed in the previous commit) and
produced the `∩┐═╜` garbage borders.

Relying on "don't Edit these files" is fragile. Converting the bytes to a UTF-8-safe form
makes the whole tree immune, grep-able, and portable.

## Goal

Every drmon source file becomes **valid UTF-8** so Edit/Write can never silently corrupt it:

- **String literals, char literals, data** → ASCII **`\xNN` escape sequences** (preserves
  exact single-byte semantics — required for `char border[8]` etc.).
- **Comments** → real **UTF-8** (readable box-art, valid encoding).

This is a **pure representation change — zero behavioural change.**

## Survey (high-byte lines by kind)

| kind | lines | files (top) |
|---|---|---|
| string literals | 22 | memory.cpp 12, about.cpp 2, monmenu.cpp 2, gadget.cpp 3, … |
| char literals | 7 | config.cpp 5, charts.cpp 1, memory.cpp 1 |
| data arrays | 70 | charts.cpp 62 (ASCII-chart table), gadget.cpp 8 |
| comments | 21 | sliosnes.cpp 18, memory/gadget/window 1 each |

(`menu.cpp`/`filereq.cpp` borders already handled; `window.cpp`/`window.hpp` default border included.)

## Approach

A **stateful C scanner** (Python, byte-safe `rb`/`wb` — never the Edit tool) walks each file
tracking lexical state: `code` / `string` / `char` / `line-comment` / `block-comment`
(handling `\` escapes inside literals and multi-line block comments). For each byte ≥ 0x80:

- in a **string/char literal** → emit `\xNN`.
- in a **comment** → emit the CP437→Unicode codepoint as UTF-8.
- in **code** state (shouldn't happen) → leave + **warn** (signals a parser gap).

**The `\x` greedy gotcha:** C's `\xNN` consumes *all* following hex digits, so `\xC4`
immediately before a literal `0-9A-Fa-f` in the same string must be split with `""`
(→ `"\xC4""A"`). The scanner inserts the break automatically.

CP437→Unicode uses the same 256-entry table as `linux/ncurses_io.cpp`.

## Files

All `*.cpp` / `*.hpp` / `*.c` / `*.h` under `devsys/tools/drmon/` that contain bytes ≥ 0x80
(~10 files). `libs/pclib/` checked separately and converted if it has any.

## Verification (must all pass)

1. **No-op build check:** rebuild and `cmp` against the pre-conversion baseline
   (`/build/drmon.baseline`). The **only** permitted differences are the embedded build
   timestamp bytes (drmon bakes in `__DATE__`/`__TIME__`); any other differing bytes mean the
   conversion changed behaviour → bug.
2. **Round-trip check:** for every converted file, re-evaluating the `\xNN` escapes and
   decoding the UTF-8 comments back to CP437 reproduces the original bytes exactly.
3. **No raw high bytes remain in literals; files are valid UTF-8** (`python … .decode('utf-8')`
   succeeds; `count(b'\xef\xbf\xbd') == 0`).
4. **Render check:** `task shot` plus the menu (`╔═╗║╚╝`), About box (`┌─┐│`), and ASCII
   chart (alt-Y) render correctly.
5. Build still 0 errors.

## Rollback

Pure git revert of the conversion commit (single mechanical commit, easy to drop).
