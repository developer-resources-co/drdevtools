# Building drmon on Linux (Phase 1)

**Status:** Phase 1 (compile + link) **complete** — `drmon` builds into an x86-64 ELF
for `SYSTEM=SNES`. It currently **segfaults on launch**: the screen, keyboard and
dev-link transport are no-op stubs (there is no terminal backend and no target yet),
so *running* it is Phase 2+ (a real ncurses/DAP front end + a MAME backend). See
[../../../../docs/plans/2026-06-10-port-drmon-linux.md](../../../../docs/plans/2026-06-10-port-drmon-linux.md).

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
| `CMakeLists.txt` | Builds the `drmon.mak` object set (minus the asm) for `SYSTEM=SNES` with `-std=gnu++98 -fpermissive -fcommon`. |

## Edits to historical source (kept minimal)

- **Tree-wide normalization** (mechanical): stripped the DOS `Ctrl-Z` (0x1A) EOF
  marker; converted DOS `\` separators in `#include` paths to `/`.
- **`compat.hpp`** — rewritten for the Linux-only path (dropped the Borland/Watcom/OS2
  fallback macros).
- **Modern-C++ correctness fixes**: `istream::get` needs `char&` not `unsigned char&`
  (`coff.hpp`, `zardoz.hpp`, `sld.cpp`); K&R implicit-`int` arrays (`board.cpp`);
  `for`-loop-scope variables hoisted (`object.cpp`, `layer.cpp`, `menu.cpp`);
  dependent-base member access `this->next` (`pclib/list.hpp`).
- **Case-sensitivity fixes** (Borland's TLINK was case-insensitive): `pobjBase`→`pObjBase`
  (`object.cpp`); added the `memOpsMenu` pointer alias for the `memopsMenu` table (`memops.cpp`).

## Known limitations (by design, this phase)

- Segfaults on launch — no terminal/target backend yet.
- Genesis (`SYSTEM=GEN`) not built yet (SNES first).
- Symbol/file loading, dev-link I/O, and the TUI are stubbed.
