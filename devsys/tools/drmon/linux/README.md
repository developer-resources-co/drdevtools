# Building drmon on Linux (Phases 1 & 1.5)

**Status:** Phase 1 (compile + link) and Phase 1.5 (run + render) **complete** —
`drmon` builds into an x86-64 ELF for `SYSTEM=SNES` and **launches in a terminal**,
rendering its full text UI (menu bar, windows, status line, live clock) via an
**ncurses** front end and accepting keyboard input (F10 menu, arrows, F-keys).
The dev-link transport is still stubbed — there is no target yet, so drmon runs
"disconnected"; wiring a real target (MAME) is Phase 2. See
[../../../../docs/plans/2026-06-10-port-drmon-linux.md](../../../../docs/plans/2026-06-10-port-drmon-linux.md).

```
  File Control Windows Macros Rate Settings Help       Wed Jun 10 09:35:21 2026




 SNESMon V2.1.30  Copyright 1991-1994 Developer Resources          Running
```

## Documentation

User-facing docs live in [`../docs/`](../docs/):

- **[User Manual](../docs/drmon-manual.md)** — windows, key reference, menus, the console
  command + expression languages, and the debug workflow.
- **[Installation](../docs/install.md)** — build & run via the Docker / `task` toolchain.
- **[Configuration](../docs/configuration.md)** — environment variables, `.scr` startup
  scripts, and the saved `.ini` settings.

Run it in a terminal at least 80×25:

```
docker run --rm -it -v "$PWD":/src -v /tmp/drmon-build:/build \
    -w /build drmon-build ./drmon
```

## Build

Nothing is installed on the host — the toolchain lives in a throwaway Docker image.

```
# 1. build the toolchain image (once)
docker build -t drmon-build devsys/tools/drmon/linux

# 2. configure + build (repo mounted at /src, build tree in a scratch dir)
docker run --rm -v "$PWD":/src -v /tmp/drmon-build:/build \
    -w /src/devsys/tools/drmon drmon-build \
    bash -c 'cmake -S . -B /build -G Ninja && ninja -C /build'

# result: /tmp/drmon-build/drmon  (ELF 64-bit x86-64)
```

## How the port is structured

The original DOS/Borland source is compiled almost unmodified. Portability lives in
this `linux/` directory and is injected by CMake, so the historical `.cpp`/`.hpp`
files stay close to original:

| Piece | Role |
|-------|------|
| `linux/linux_compat.hpp` | Force-included (`-include`) into every TU. Neutralizes `far`/`near`/`cdecl`, maps far-heap/mem ops, defines `_MAX_*` path limits, `stricmp`→`strcasecmp`, declares `_splitpath`/`_makepath`/`strupr`. |
| `linux/include/*.h` | Shim headers for the Borland/DOS headers the source includes: `dos.h`, `conio.h`, `alloc.h`, `dir.h`, `mem.h`, `io.h`, `bios.h`, `direct.h`, `iostream.h`, `fstream.h`, plus empty stubs for the dropped `phapi.h`/`bse.h`/`os2.h`/`i86.h`. |
| `linux/dos_stubs.cpp` | No-op implementations of DOS/BIOS/conio (`int86`, `inportb`, `getch`, `gettextinfo`, …) + functional `_splitpath`/`_makepath`/`strupr`. Real terminal I/O is a later phase. |
| `linux/slio_stub.cpp` | Replaces the DOS x86 transport asm (`snesio.asm`): provides `SwapWord`/`GetSlaveBus`/`SendCmd`/… as no-ops. **This is the seam the Phase 2 MAME bridge plugs into.** |
| `linux/ncurses_io.cpp` | **Phase 1.5 front end.** Blits drmon's CGA char+attr video buffer to the terminal via **wide ncurses** (CP437→Unicode box-drawing/blocks, CGA→colour pairs) and feeds the keyboard back in the DOS extended-key format — arrows/F-keys→scan codes, and **Alt+letter** (ESC-disambiguated) → DOS alt scan codes so alt-Q/alt-X/alt-M etc. work. Replaces the keyboard stubs. |
| `CMakeLists.txt` | Builds the `drmon.mak` object set (minus the asm) for `SYSTEM=SNES` with `-std=gnu++98 -fpermissive -fcommon`; links `ncurses`. |

## Edits to historical source (kept minimal)

- **Tree-wide normalization** (mechanical): stripped the DOS `Ctrl-Z` (0x1A) EOF
  marker; converted DOS `\` separators in `#include` paths to `/`.
  ([commit c7d1ea2](https://github.com/developer-resources-co/drdevtools/commit/c7d1ea226dc4df7442ed330b0250d9ba14cc47ba))
- **`compat.hpp`** — rewritten for the Linux-only path (dropped the Borland/Watcom/OS2
  fallback macros).
  ([diff](https://github.com/developer-resources-co/drdevtools/commit/c7d1ea226dc4df7442ed330b0250d9ba14cc47ba#diff-b5b68f01106e02d54e5da89993cadf895c0acc690fa5216793842468c56f6684))
- **Modern-C++ correctness fixes**: `istream::get` needs `char&` not `unsigned char&`
  ([coff.hpp](https://github.com/developer-resources-co/drdevtools/commit/c7d1ea226dc4df7442ed330b0250d9ba14cc47ba#diff-1e66fe1f587c5164155fa75ce2be73a1728c01754b14e9ffd761473dd94f2a82),
  [zardoz.hpp](https://github.com/developer-resources-co/drdevtools/commit/c7d1ea226dc4df7442ed330b0250d9ba14cc47ba#diff-409064c9f87fa36f5f65ae13ce30359ca0cb83db45fb47c3d1be938b1a9d9ee8),
  [sld.cpp](https://github.com/developer-resources-co/drdevtools/commit/c7d1ea226dc4df7442ed330b0250d9ba14cc47ba#diff-de0b3498087492a2d03c73dbdba98d4529a10a5d3e883c74706bee3e2038d20d));
  K&R implicit-`int` arrays
  ([board.cpp](https://github.com/developer-resources-co/drdevtools/commit/c7d1ea226dc4df7442ed330b0250d9ba14cc47ba#diff-46b7bcbbc75f88e06310b9af0505b530025279ce46279ae8fc7bf0629c0a4c0c));
  `for`-loop-scope variables hoisted
  ([object.cpp](https://github.com/developer-resources-co/drdevtools/commit/c7d1ea226dc4df7442ed330b0250d9ba14cc47ba#diff-ce341b9ed1e141de6e5f2a7aa485aa0d71bec37847092600279a7881821b776c),
  [layer.cpp](https://github.com/developer-resources-co/drdevtools/commit/c7d1ea226dc4df7442ed330b0250d9ba14cc47ba#diff-ccf9565d7c221986fde93d63be45ba1cc765b2db4a274e1ccdc3787d828f1f56),
  [menu.cpp](https://github.com/developer-resources-co/drdevtools/commit/c7d1ea226dc4df7442ed330b0250d9ba14cc47ba#diff-0fec4e6a5f39f8ebf3f60bdb6c98fc28e7f720d898b43cfd7d40093ee89f1039));
  dependent-base member access `this->next`
  ([pclib/list.hpp](https://github.com/developer-resources-co/drdevtools/commit/c7d1ea226dc4df7442ed330b0250d9ba14cc47ba#diff-0f6c67ca39c08ba2ea92d177dc4e8ad1e2027995bd0ca7aa3f877adf8ca86061)).
- **Case-sensitivity fixes** (Borland's TLINK was case-insensitive): `pobjBase`→`pObjBase`
  ([object.cpp](https://github.com/developer-resources-co/drdevtools/commit/c7d1ea226dc4df7442ed330b0250d9ba14cc47ba#diff-ce341b9ed1e141de6e5f2a7aa485aa0d71bec37847092600279a7881821b776c));
  added the `memOpsMenu` pointer alias for the `memopsMenu` table
  ([memops.cpp](https://github.com/developer-resources-co/drdevtools/commit/c7d1ea226dc4df7442ed330b0250d9ba14cc47ba#diff-b51ff26efd4b443374efa32782e386e49bbefb3cb800669ef5cd0f12365ba41d)).
- **Phase 1.5 run fixes** (all `#if defined(__GNUC__)` guarded): comram backed by a real
  buffer instead of a `seg<<16` far pointer
  ([board.cpp](https://github.com/developer-resources-co/drdevtools/commit/d97785e4470479c1490d7c2896bdb6b5bb15b41b#diff-46b7bcbbc75f88e06310b9af0505b530025279ce46279ae8fc7bf0629c0a4c0c));
  non-fatal "no slave" so the UI runs disconnected
  ([drmon.cpp](https://github.com/developer-resources-co/drdevtools/commit/d97785e4470479c1490d7c2896bdb6b5bb15b41b#diff-514b974653e9ea8a635c14365364e040eb7ba847b34f199e97b638c20ef59471));
  real 80×25 framebuffer + ncurses init in `SetupScreen`
  ([screen.cpp](https://github.com/developer-resources-co/drdevtools/commit/d97785e4470479c1490d7c2896bdb6b5bb15b41b#diff-51d84234d8182372f150550eda77829d7892b713e0c42a45275a51e65a7e856a));
  `scrBuffer` allocated on Linux + ncurses blit after `CopyScreen`
  ([display.cpp](https://github.com/developer-resources-co/drdevtools/commit/d97785e4470479c1490d7c2896bdb6b5bb15b41b#diff-02597adf45b63236181f6a6f92c05e0b35e361b200b1355e1a9a0076f60c5ee6));
  keyboard wired through ncurses
  ([input.cpp](https://github.com/developer-resources-co/drdevtools/commit/d97785e4470479c1490d7c2896bdb6b5bb15b41b#diff-0689495909ce363ed69070c5477ed56bb927f368f28042647b66d426e2ab5025)).

## CP437 / extended-ASCII bytes

drmon draws its UI in **IBM CP437 / extended-ASCII** (box-drawing borders, shades, the
`█` logo, the ASCII-chart table, default config chars). Those bytes used to be stored
**raw** (not valid UTF-8), which the UTF-8 Edit/Write tools silently corrupted into
`U+FFFD` (�) and broke the borders. The whole tree has since been converted to a
UTF-8-safe form (see [the conversion plan](../../../../docs/plans/2026-06-10-cp437-ascii-conversion.md),
tool `cp437-to-ascii.py`): **string/char literals + data → `\xNN` escapes**, **comments →
UTF-8**. Every file is now valid UTF-8, so ordinary editing is safe again.

To keep it that way: write any new border/glyph **string** as ASCII escape sequences
(e.g. `"\xC9\xCD\xBB\xBA\xBA\xC8\xCD\xBC"`), never raw high bytes. Sanity check after edits:

```
python3 -c "print(open('menu.cpp','rb').read().count(b'\xef\xbf\xbd'))"   # 0 = clean
```

## Known limitations (by design, this phase)

- Runs **disconnected** — no target yet (dev-link transport stubbed); a real target
  arrives via the Phase 2 MAME bridge.
- Keyboard covers letters/arrows/F-keys/Enter/Esc and **Alt-combos** (alt-Q close,
  alt-X exit, alt-M/alt-R/… open windows). **Mouse** isn't wired yet.
- Genesis (`SYSTEM=GEN`) not built yet (SNES first).
- Fixed 80×25; terminal must be at least that size.
