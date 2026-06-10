# Port `drmon` to Linux — Phase 1: compile + link (SNES)

**Date:** 2026-06-10
**Status:** Planned (Phase 1 not started)

## Context

`drmon` (the "DR Monitor", v2.1.30, 1994) is the Developer Resources source-level
debugger, now in this repo at `devsys/tools/drmon/` — ~32.5K LOC of Borland/Watcom
C++ for DOS/OS2. It debugged SNES (65816) and Genesis (68000) games over a parallel
"SLIO" dev-link. Goal: bring it to Linux. **Phase 1 (this plan) is narrow: get it to
compile + link on Linux.** Running the UI and talking to a target are later phases.

## Decisions (confirmed with user)

- **Phase 1 = compile + link only.** Produce a Linux x86-64 binary; TUI + transport
  stubbed. Actually running/rendering is deferred.
- **SNES first** (`SYSTEM=SNES` / `snesmon`): 65816 disassembler + the C-based
  single-step branch logic in `sliosnes.cpp`.
- **Drop Borland / Watcom / OS2 / DOS-extender support entirely.** Don't add a Linux
  branch *alongside* the legacy ones — *remove* the `__BORLANDC__` / `__WATCOMC__` /
  `__OS2__` / `DOSX286` / `__MSDOS__` paths and make the tree modern-C++/Linux-only.
- **Toolchain:** modern `g++`/`clang` in a **throwaway Docker build image** (no host
  installs), orchestrated by CMake.

## Reuse findings (from exploration)

- **Front end: adopt DAP.** The eventual UI is the
  **[Debug Adapter Protocol](https://microsoft.github.io/debug-adapter-protocol/)** served
  via **[cppdap](https://github.com/google/cppdap)** (Google, Apache-2.0) — editor-agnostic,
  so VS Code / nvim-dap / JetBrains become the front end and we ship no editor plugin. DAP
  is preferred over a bespoke protocol or a GDB/Godot-style scheme because it is
  editor-agnostic; a survey of the alternatives reached the same conclusion. (No existing
  debugger code is being reused — only this architectural decision.)
- **drmon's reusable core** (keep cleanly compilable — it's the asset): 68000/65816
  disassemblers (`dis68000.cpp`, `dis816.cpp`, `tab816.cpp`), symbol/debug-info parsers
  (`sld.cpp`, `coff.cpp`, `zardoz.cpp`, `symbol.cpp`), breakpoint/watch/expression engine
  (`break.cpp`, `watch.cpp`, `expr.*`, `eval.cpp`), command shell, and the SLIO opcode
  protocol (`sliosnes.cpp` + `slaveio.hpp`).
- **Clean seams:** transport is isolated (`sliosnes.cpp`/`sliogen.cpp` + `*.asm`); the
  DOS UI is isolated to ~8 files (`screen.cpp`, `display.cpp`, `input.cpp`, `app.cpp`,
  `mouse.cpp`, `chargfx.cpp`); the code already branches per-compiler in
  `global.hpp`/`compat.hpp`, so a Linux path fits its own pattern.

## Phase 1 plan — compile + link

1. **Toolchain image** — `devsys/tools/drmon/linux/Dockerfile`: debian/ubuntu + `g++`,
   `clang`, `cmake`, `ninja`, `bison`, `flex`. Build mounts the repo; nothing on host.
2. **CMake target** — `devsys/tools/drmon/CMakeLists.txt`: `drmon` with `-DSNES
   -DSYSTEMSNES`, `-std=gnu++98 -fpermissive -w` initially (tighten later). Sources = the
   portable `.cpp` + Linux stub TUs; exclude the 6 `.asm` files and DOS-only UI TUs.
3. **Linux compat layer** — `devsys/tools/drmon/linux/`: extend `global.hpp`/`compat.hpp`
   to neutralize `far/near/huge/_far/_near` and map `farmalloc/farfree→malloc/free`,
   `_fmemcpy→memcpy`, `MK_FP/FP_SEG/FP_OFF`. Provide minimal shim headers for the
   DOS/Borland includes actually used (`dos.h`, `conio.h`, `bios.h`, `alloc.h`, `dir.h`,
   `graphics.h`) declaring only referenced symbols. Inject via CMake forced-include to
   avoid editing every file. Strip the legacy `#ifdef` compiler arms as files are touched.
4. **Transport stub** — `linux/slio_stub.cpp`: provide the symbols the `.asm` files
   defined (`GetAttention`, `GetSlaveBus`/`ReturnSlaveBus`, `SendCmd`, `RecBytes`/
   `SendBytes`, port I/O) as no-ops so `sliosnes.cpp` links. This is the exact seam MAME
   plugs into in Phase 2.
5. **Screen/input stubs** — redirect video memory (`MK_FP(0xB800,…)`) to a normal array,
   no-op the BIOS `int86(0x10/0x16/0x33)` calls and `kbhit/getch`, so the real
   `screen.cpp`/`display.cpp`/`input.cpp` compile + link (foundation for a later
   ncurses/DAP front end).
6. **Parser** — try the pre-generated `ytab.c`/`lexyy.c` first; regenerate with
   `bison`/`flex` from `expr.y`/`expr.l` in the container if they don't build.
7. **Dialect-fix loop (the real grind)** — iterate build → fix 1994 pre-ISO C++ errors
   (`<iostream.h>`, implicit int, `void*` casts, default-arg redecls, nested-type access,
   K&R). Prefer compat-layer/forced-include fixes; edit historical files only when
   unavoidable.
8. **Link → produce the `drmon` ELF.** Phase 1 done = it builds.

## Roadmap (post Phase 1 — context, not this plan)

- **Phase 2 — MAME backend:** replace `slio_stub.cpp` with a bridge that translates
  drmon's opcode protocol (read/write mem+regs, set/clear breakpoint, step, continue,
  async-exception) to MAME debugger ops over its
  [Lua debugger API](https://docs.mamedev.org/luascript/ref-debugger.html) or
  [gdbstub](https://docs.mamedev.org/plugins/gdbstub.html) (`127.0.0.1:2159`). Load the
  SNES game in MAME; drmon drives it.
- **Phase 3 — DAP front end (confirm before building):** wrap drmon-core behind cppdap;
  map DAP `setBreakpoints`/`stackTrace`/`scopes`/`variables`/`continue`/`next`/`evaluate`
  onto drmon-core + the MAME backend. UI = any DAP editor (VS Code, nvim-dap). The 1994
  ncurses TUI is likely dropped here, not ported.

## Documentation deliverables

drmon already ships plaintext help + a changelog to **seed from** — don't write usage
from scratch:
- `devsys/tools/drmon/snesmon.hlp` / `genmon.hlp` — original in-app **key reference**
  (plaintext): F2 Run, F3 Stop/Break, F4 Run-no-update, F6 Step Over, F7 Trace/Step Into,
  F8 local menu, F10 menu bar; window hotkeys (alt-M memory, alt-Z zoom, alt-Q close,
  alt-X exit, alt-D dos shell).
- `history.txt` — feature changelog (conditional breakpoints, watch window, fill memory…).
- `snesmon.scr` — example startup script.

New docs to produce (under `devsys/tools/drmon/docs/`):

1. **USER's manual** (`docs/drmon-manual.md`) — what drmon is; the window types (memory,
   register, disassembly, source, watch, breakpoints, console); the full **key reference**
   (seed verbatim from the `.hlp` files); the debug workflow (load symbols → set
   breakpoints → run → step → inspect); and the console command / expression language
   (`command.cpp`, `expr.*`). Flag target-dependent features as "needs a backend (Phase 2+)".
2. **Installation** (`docs/install.md`, plus a README section) — Phase 1: build from source
   via the Docker toolchain (build the image, `cmake --build`, resulting `drmon` binary),
   prerequisites, where the binary lives, how to launch. Later: a packaged binary / `.deb`
   for the foundry apt repo.
3. **Configuration** (`docs/configuration.md`) —
   - **Environment:** `DR_SNESPORT` (dev-link port base, default `0x318`),
     `DR_SNESMEMBUFFER` (comram segment). These are legacy hardware knobs; under the Phase 2
     MAME backend they're replaced by the bridge endpoint.
   - **Startup scripts:** `.scr` files run commands at launch — e.g. `open reg`,
     `pcl = $C00000`, `open memory dynamicrunning pcl`.
   - **Saved state:** window layout / colors / breakpoints via the config layer
     (`config.cpp`, `profile.cpp`; legacy binary `.cfg`). Define a modern config-file
     location + format as part of the port.

Manual content that depends on a *running* debugger is finalized as the UI/backend comes up
(Phase 2+); the key reference, installation, and configuration can be written now from the
`.hlp` files and source.

## Verification (Phase 1)

1. `docker build` of the toolchain image succeeds.
2. CMake configures with `SYSTEM=SNES` (capture config output).
3. `cmake --build` compiles every TU with 0 errors (capture file count).
4. Link succeeds → `file drmon` reports `ELF 64-bit … x86-64`.
5. `./drmon` (or `--help`) loads without missing-symbol/immediate crash (it may do little
   — acceptable for link-only); capture output.
6. Document the stubbed-vs-compiled file split and every historical-file edit made.

## Verification results — executed 2026-06-10

1. **Toolchain image** — PASS — `docker build -t drmon-build devsys/tools/drmon/linux` (ubuntu:24.04 + g++/clang/cmake/ninja/bison/flex) succeeded.
2. **CMake configure (SYSTEM=SNES)** — PASS — configured with `-DSNES -DSYSTEMSNES -DDEBUGDR`.
3. **Compile all TUs** — PASS — 52 objects, 0 errors (`-std=gnu++98 -fpermissive -fcommon`). Iterated from 329 → 0 errors fixing: Ctrl-Z EOF markers, `\` include paths, `istream::get(char&)`, K&R implicit-int, for-scope leaks, Borland string fns, case-mismatched symbols.
4. **Link → ELF** — PASS

        /tmp/drmon-build/drmon: ELF 64-bit LSB pie executable, x86-64, ... not stripped   (307 KB)

5. **Run** — EXPECTED FAIL (acceptable for link-only) — `./drmon` segfaults immediately: screen/input/transport are no-op stubs (no terminal, no target). Making it *run* is Phase 1.5 (ncurses/DAP front end) + Phase 2 (MAME backend).
6. **Docs** — PASS — see [../../devsys/tools/drmon/linux/README.md](../../devsys/tools/drmon/linux/README.md) (build steps, stub map, full edit list).

**Outcome:** Phase 1 (compile + link) complete — drmon builds into a Linux x86-64 binary.
