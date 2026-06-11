# Building drmon on Linux (Phases 1 – 2)

**Status:** Phase 1 (compile + link), Phase 1.5 (run + render), and Phase 2
(MAME TCP bridge) **complete** —
`snesmon` and `genmon` both build into x86-64 ELFs and launch in a terminal,
rendering the full text UI via **ncurses**. When MAME is running with
`mame_bridge.lua`, both debuggers connect over TCP and give live register read/write,
memory read/write, breakpoints, single-step, continue, halt, and reset.
See [docs/plans/2026-06-11-drmon-mame-backend.md](../../../../docs/plans/2026-06-11-drmon-mame-backend.md).

```
  File Control Windows Macros Rate Settings Help       Wed Jun 11 12:00:00 2026

 SNESMon V2.1.30  Copyright 1991-1994 Developer Resources          Stopped
```

## Quick start

```
# 1. build the toolchain image (once)
task image

# 2. build both snesmon and genmon
task build

# 3a. live SNES debugging — run MAME with the bridge lua script:
task mame SYS=snes CART=/path/to/game.sfc
# in a second terminal, attach drmon:
task run SYS=snes

# 3b. live Genesis debugging:
task mame SYS=gen CART=/path/to/game.md
task run SYS=gen
```

`DRMON_MAME_ADDR=host:port` overrides the default `127.0.0.1:41816` for both
`task mame` (the Lua server) and `task run` (the C++ client).

## Documentation

User-facing docs live in [`../docs/`](../docs/):

- **[User Manual](../docs/drmon-manual.md)** — windows, key reference, menus, the console
  command + expression languages, and the debug workflow.
- **[Installation](../docs/install.md)** — build & run via the Docker / `task` toolchain.
- **[Configuration](../docs/configuration.md)** — environment variables, `.scr` startup
  scripts, and the saved `.ini` settings.

## Build

Nothing is installed on the host — the toolchain lives in a throwaway Docker image.

```
# 1. build the toolchain image (once)
docker build -t drmon-build devsys/tools/drmon/linux

# 2. configure + build (repo mounted at /src, build tree in a scratch dir)
docker run --rm -v "$PWD":/src -v /tmp/drmon-build:/build \
    -w /src/devsys/tools/drmon drmon-build \
    bash -c 'cmake -S . -B /build -G Ninja && ninja -C /build'

# result: /tmp/drmon-build/snesmon   (ELF 64-bit x86-64, SNES/65816)
#         /tmp/drmon-build/genmon    (ELF 64-bit x86-64, Genesis/68000)
```

## How the port is structured

The original DOS/Borland source is compiled almost unmodified. Portability lives in
this `linux/` directory and is injected by CMake, so the historical `.cpp`/`.hpp`
files stay close to original:

| Piece | Role |
|-------|------|
| `linux/linux_compat.hpp` | Force-included (`-include`) into every TU. Neutralizes `far`/`near`/`cdecl`, maps far-heap/mem ops, defines `_MAX_*` path limits, `stricmp`→`strcasecmp`, declares `_splitpath`/`_makepath`/`strupr`. |
| `linux/include/*.h` | Shim headers for the Borland/DOS headers the source includes: `dos.h`, `conio.h`, `alloc.h`, `dir.h`, `mem.h`, `io.h`, `bios.h`, `direct.h`, `iostream.h`, `fstream.h`, plus empty stubs for the dropped `phapi.h`/`bse.h`/`os2.h`/`i86.h`. |
| `linux/dos_stubs.cpp` | No-op implementations of DOS/BIOS/conio (`int86`, `inportb`, `getch`, `gettextinfo`, …) + functional `_splitpath`/`_makepath`/`strupr`. |
| `linux/ncurses_io.cpp` | **Phase 1.5 front end.** Blits drmon's CGA char+attr video buffer to the terminal via **wide ncurses** (CP437→Unicode box-drawing/blocks, CGA→colour pairs) and feeds the keyboard back in the DOS extended-key format — arrows/F-keys→scan codes, **Alt+letter** (ESC-disambiguated) → DOS alt scan codes. |
| `linux/sliomame.cpp` | **Phase 2 SNES bridge client.** Implements the `slaveio.hpp` contract over TCP to `mame_bridge.lua`. Included via `which.cpp` under `#ifdef MAMEBACKEND && SNES`. |
| `linux/sliogdb.cpp` | **Phase 2 Genesis GDB RSP client.** Implements the `slaveio.hpp` contract over GDB RSP TCP to MAME's native `-debugger gdbstub`. Included under `#ifdef MAMEBACKEND && GENESIS`. |
| `linux/slio_stub.cpp` | Link residue for `board.cpp` when `DRMON_MAME_BACKEND=OFF`. Provides `SwapWord`/`GetSlaveBus`/`SendCmd`/… as stubs. Excluded from the build when the MAME backend is on. |
| `linux/mame_bridge.lua` | **Phase 2 SNES bridge server.** Lua autoboot script loaded by MAME (`-autoboot_script`). Listens on `127.0.0.1:41816`, dispatches drmon protocol commands. SNES only — Genesis uses MAME's native GDB RSP. |
| `linux/test_bridge.sh` | Integration test: launches MAME headless (SNES=Lua bridge, GEN=gdbstub), runs the appropriate test suite, tears down on exit. |
| `linux/test_bridge.py` | SNES protocol test suite: V/REGS, G field count, R/W round-trip in WRAM, RP (PPU VRAM read), WP±/BW± (watchpoint toggles), B+/B-, S `<next>` step with PC-advance assertion, bp-while-running, RESET. |
| `linux/test_gdb.py` | Genesis GDB RSP test suite: g-packet layout, m/M round-trip, Z0/z0, `c`+bp, `s` step, break-in 0x03. |
| `linux/spikes/verify_features.py` | Deep feature checks (reproduce the stub-lift evidence): PPU seed→readback, watchpoint fire, ROM-write drop. Launches headless MAME per check. `python3 spikes/verify_features.py` → 8/8. |
| `linux/spikes/connected_shot.sh` | Capture a *connected* `snesmon` TUI as text (host MAME + container `snesmon` via `--network=host`). Proves the live pipeline (status `Running`, live target memory). `KEYS=` drives tmux keys. |
| `linux/test-roms/` | `drmon-test.sfc` (NOP sled), `drmon-test-writer.sfc` (loops `STA $8250` — fires watchpoints), `drmon-test.md` (Genesis) — homebrew, committed. Regenerate with `python3 test-roms/generate_test_roms.py`. SNES ROMs from `roms/snes/` take precedence for the SNES test. |
| `CMakeLists.txt` | Builds `snesmon` (`-DSNES -DSYSTEMSNES`) and `genmon` (`-DGENESIS -DSYSTEMGEN`), both with `MAMEBACKEND` by default. `DRMON_MAME_BACKEND=OFF` falls back to `slio_stub.cpp`. |

## Environment variables

| Variable | Default | Description |
|----------|---------|-------------|
| `DRMON_MAME_ADDR` | `127.0.0.1:41816` | TCP address of the `mame_bridge.lua` Lua server (SNES only). |
| `DRMON_GDB_ADDR` | `127.0.0.1:23946` | TCP address of MAME's GDB RSP stub (Genesis only). |

## Protocol overview

The bridge uses a minimal newline-terminated ASCII protocol, hand-debuggable with
`nc 127.0.0.1 41816`:

| Command | Response | Meaning |
|---------|----------|---------|
| `V` | `ok drmon-bridge 1 <cpu>` | Version handshake; returns CPU shortname (`5a22`, `m68000`) |
| `REGS n,n,...` | `ok` | Announce register order for G/P |
| `G` | `val val ...` | Get registers in announced order (hex) |
| `P val val ...` | `ok` | Put registers in announced order (hex) |
| `R addr len` | `hexbytes` | Read memory (program space, hex addr/len) |
| `RP addr len` | `hexbytes` | Read PPU VRAM (snes_ppu `m_vram` save-item; addr = 0..0xffff VRAM offset) |
| `W addr hexbytes` | `ok` | Write memory |
| `WP+` / `WP-` | `ok` | Write-protect: arm/clear a write-watchpoint over the ROM window ($8000-$FFFF) |
| `BW+` / `BW-` | `ok` | Break-on-ROM-write: same watchpoint primitive (both halt on write) |
| `B+ addr` | `ok` | Set breakpoint |
| `B- addr` | `ok` | Clear breakpoint |
| `S <next> [<alt>]` | `stopped <pc> step` | Single step via one-shot bps at candidate next-PC(s); reply deferred until bp fires (SNES only) |
| `C` | `ok` | Continue |
| `H` | `stopped <pc> halt` | Halt (async if running) |
| `?` | `running` or `stopped <pc> <reason>` | Poll execution state |
| `RESET` | `ok` | Soft reset |
| `BYE` | — | Disconnect; server reopens listener |

## Edits to historical source (kept minimal)

- **Tree-wide normalization** (mechanical): stripped the DOS `Ctrl-Z` (0x1A) EOF
  marker; converted DOS `\` separators in `#include` paths to `/`.
  ([commit c7d1ea2](https://github.com/developer-resources-co/drdevtools/commit/c7d1ea226dc4df7442ed330b0250d9ba14cc47ba))
- **`compat.hpp`** — rewritten for the Linux-only path (dropped the Borland/Watcom/OS2
  fallback macros).
- **Modern-C++ correctness fixes**: `istream::get` needs `char&` not `unsigned char&`;
  K&R implicit-`int` arrays; `for`-loop-scope variables hoisted; dependent-base member
  access `this->next`.
- **Case-sensitivity fixes** (Borland's TLINK was case-insensitive): `pobjBase`→`pObjBase`;
  added the `memOpsMenu` pointer alias.
- **Phase 1.5 run fixes** (all `#if defined(__GNUC__)` guarded): comram backed by a real
  buffer; non-fatal "no slave" so the UI runs disconnected; real 80×25 framebuffer +
  ncurses init; `scrBuffer` allocated on Linux + ncurses blit; keyboard wired through ncurses.
- **Phase 2 build fixes**: removed duplicate project-header includes from `sliomame.cpp`
  (headers lack include guards; `which.cpp` → `moninc.hpp` already pulls them in);
  removed duplicate `WriteSlaveDatum` definition (already in `command.cpp`).

## CP437 / extended-ASCII bytes

drmon draws its UI in **IBM CP437 / extended-ASCII** (box-drawing borders, shades, the
`█` logo, the ASCII-chart table, default config chars). Those bytes are stored as
`\xNN` escapes throughout the source tree — every file is valid UTF-8.

To keep it that way: write any new border/glyph **string** as ASCII escape sequences
(e.g. `"\xC9\xCD\xBB\xBA\xBA\xC8\xCD\xBC"`), never raw high bytes. Sanity check:

```
python3 -c "print(open('menu.cpp','rb').read().count(b'\xef\xbf\xbd'))"   # 0 = clean
```
