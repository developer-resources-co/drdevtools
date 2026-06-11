# Configuring drmon

Three mechanisms tune drmon: **environment variables** (dev-link hardware knobs), **startup
scripts** (`.scr`, a batch of console commands run at launch), and a **saved settings file**
(`.ini`, colours/layout/modes). See also [Installation](install.md) and the
[User Manual](drmon-manual.md).

## Environment variables

drmon reads two environment variables that configure the legacy parallel-port dev-link to
target hardware (`board.cpp`):

| Variable | Meaning | Default |
|----------|---------|---------|
| `DR_SNESPORT` | Base I/O address of the dev-link port (hex). | `0x318` |
| `DR_SNESMEMBUFFER` | "Comram" shared-memory window segment (hex). | platform comram buffer |

Values are parsed as hex (`sscanf("%x")`), e.g. `export DR_SNESPORT=0x378`. The generic form is
`DR<PLATFORM>PORT` / `DR<PLATFORM>MEMBUFFER` (so a Genesis build would read `DR_GENESISPORT`).

> **No effect — superseded by the MAME bridge.** These are legacy hardware knobs for the original
> parallel-port dev cartridge. The Phase 2 backend talks to MAME over TCP instead, so the live
> endpoint is configured with **`DRMON_MAME_ADDR`** (SNES Lua bridge, default `127.0.0.1:41816`) or
> **`DRMON_GDB_ADDR`** (Genesis GDB RSP, default `127.0.0.1:23946`) — not these. `DR_SNESPORT` /
> `DR_SNESMEMBUFFER` change nothing today.

## Startup scripts (`.scr`)

A `.scr` file is a plain-text batch of [console commands](drmon-manual.md#console-command-language),
one per line, executed at launch. drmon picks one up automatically:

1. If you pass a path as the first argument (`drmon myscript.scr`) and the file exists, run it.
2. If that argument doesn't exist as given, retry with `.SCR` appended.
3. With no argument, run **`SNESMon.scr`** from the current directory if present (then a
   `progname`-with-`.scr`-extension fallback).

Line syntax (`command.cpp`):

- `;` (or `#`) at the start of a line — **comment**, not executed.
- `\` at the start — **continuation** (the `\` becomes a space, joining multi-line commands).
- anything else — echoed to the Command window, then parsed as a console command.

Example `SNESMon.scr` (the shipped sample):

```
open reg
pcl = $C00000
open memory dynamicrunning pcl
```

That opens the Register window, sets the symbol `pcl` to `$C00000`, and opens a Memory window
tracking that address. (Commands that touch a target — memory reads, run/step — need a connected
MAME target; window-opening and symbol assignment work with or without one.)

## Saved settings (`.ini`)

Colours, glyphs, display/run modes, and related preferences persist in a Windows-style `.ini`
file (`config.cpp` + `profile.cpp`), discovered as `SNESMon.ini` (then a `progname`-with-`.ini`
fallback). Load/save it from the **Settings** menu (*Load INI Settings* / *Save INI Settings*).

Representative keys (each UI element has a colour attribute + glyph):

```ini
[SNESMon]
DISPMODE=3
WINDOWTITLEATTR=7
MESSAGEBARATTR=1024
WRITEPROTECT=0
RUNMODE=0
INSERT=1
```

What persists: per-element colour attributes and glyph characters (window borders, menu bar,
register/memory/source/symbol/watch/command views, ASCII + processor charts), display mode,
write-protect flag, run mode, insert-mode default, and window shadow offsets.

## Build variant (SNES vs Genesis)

The build is **SNES** by default (`-DSNES -DSYSTEMSNES` in `CMakeLists.txt`), which sets
`PLATFORMNAME=SNES`, the 65816 disassembler, and the `SNESMon` program name. A Genesis variant
(`-DGENESIS`, 68000) is planned but **not built yet** — see the roadmap in
[Installation](install.md#roadmap).
