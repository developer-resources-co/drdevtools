# Installing drmon (Linux)

drmon is the **Developer Resources Monitor** — a source-level debugger/monitor for the SNES (65816) and
Genesis (68000), originally a 1990s DOS product, now ported to Linux/ncurses. This page
covers building and launching it. For day-to-day use see the [User Manual](drmon-manual.md);
for tuning see [Configuration](configuration.md).

> **Current status — Phase 2 MAME backend is in.** drmon builds and runs its full text UI, and the
> dev-link transport is wired to a live target: both `snesmon` and `genmon` talk to MAME via a Lua
> bridge on `:41816` under `-debugger none` (`mame_bridge.lua` for SNES, `mame_genesis_bridge.lua`
> for Genesis — one channel for CPU + non-CPU state, so VDP/Z80 read at a breakpoint). Launch a
> target with `task mame SYS=snes|gen CART=…`, then run the matching binary. **Without a connection** drmon runs
> *disconnected* (UI works; target reads return zero); features needing a live link are flagged
> "needs a connected target" throughout the manual.

## Prerequisites

- **Docker** — that's it. The entire toolchain (g++/clang, CMake, Ninja, Bison/Flex, ncurses,
  gdb, tmux) lives in a throwaway image; **nothing is installed on the host**.
- **[Task](https://taskfile.dev)** (`task`) — optional but recommended; the repo's
  `Taskfile.yml` wraps every Docker invocation. The raw `docker run` commands are shown too.
- A terminal at least **80×25** (drmon's screen now grows to fill a larger terminal and
  re-flows on resize, but 80×25 is the floor).

## Build and run with `task` (recommended)

From the repo root (`~/SRC/drdevtools`):

```bash
task image     # 1. build the throwaway toolchain image (once; cached thereafter)
task build     # 2. configure + compile + link drmon (SYSTEM=SNES)
task run       # 3. launch drmon in your terminal
```

`task run` needs an interactive TTY ≥ 80×25 and runs drmon disconnected (no target). Quit with
**Alt+X** (Ctrl+C is intentionally ignored — see the manual).

The compiled binary lands at **`/tmp/drmon-build/drmon`** (a 64-bit x86-64 ELF, built inside
the container against the repo mounted at `/src`).

### All drmon tasks

| Task | What it does |
|------|--------------|
| `task image` | Build the `drmon-build` Docker toolchain image. |
| `task build` | CMake configure + Ninja compile + link (depends on `image`). |
| `task run` | Launch drmon in an interactive terminal (disconnected). |
| `task debug` | Run drmon under gdb and print a backtrace (crash diagnosis). |
| `task shot` | Headless screenshot — launch in tmux, dump the rendered TUI as text. |
| `task smoke` | Smoke test — open every window via its Alt-key under gdb; fail on any crash. |
| `task signals` | Signal test — Ctrl+C is ignored; a fatal signal restores the terminal. |
| `task clean` | Remove the `/tmp/drmon-build` build tree. |
| `task rebuild` | `clean` then `build` from scratch. |

> **AddressSanitizer is on by default.** The build is compiled with `-DDRMON_ASAN=ON`
> (`-fsanitize=address`). ASan reports are written to `/tmp/drmon-build/asan.<pid>` (via
> `ASAN_OPTIONS=log_path=/build/asan:…`) so they never corrupt the TUI. This has already
> flushed out several dormant memory bugs — see
> [BUGS.md](../../../../docs/BUGS.md).

## Build and run without `task` (raw Docker)

```bash
# 1. build the toolchain image (once)
docker build -t drmon-build devsys/tools/drmon/linux

# 2. configure + build (repo at /src, scratch build tree at /tmp/drmon-build)
docker run --rm -v "$PWD":/src -v /tmp/drmon-build:/build \
    -w /src/devsys/tools/drmon drmon-build \
    bash -c 'cmake -S . -B /build -G Ninja && ninja -C /build'

# 3. run it (interactive TTY, ≥ 80×25)
docker run --rm -it -v "$PWD":/src -v /tmp/drmon-build:/build \
    -w /build -e TERM=xterm-256color drmon-build ./drmon
```

## How the port is structured

The historical DOS/Borland `.cpp`/`.hpp` files compile almost unmodified; all the Linux
portability lives in `devsys/tools/drmon/linux/` and is injected by CMake. For the full
breakdown (compat shims, the ncurses front end, the MAME bridge transport) see
[`linux/README.md`](../linux/README.md).

## Roadmap

- **Phase 2 — done.** The dev-link is wired to MAME via one `sliomame.cpp` client ↔ a Lua bridge
  per platform (`mame_bridge.lua` SNES / `mame_genesis_bridge.lua` Genesis), both sharing
  `mame_cpu_bridge.lua` under `-debugger none`. Live memory/registers/run/step/breakpoints, the
  SNES PPU window, and write-protect / break-on-ROM-write all work against a connected target.
- **Genesis — done.** Both `snesmon` (65816) and `genmon` (68000) build, run, and debug. The M68K
  was folded into the Lua bridge (dropping MAME gdbstub) so **VDP/CRAM/VSRAM/Z80 are now served on
  the same channel and readable at a breakpoint**; the 68000's native single-step is used directly.
- **Phase 3** — a DAP front end so VS Code / nvim-dap can drive drmon-core (confirm approach first).
- **Packaging** — a `.deb` for the foundry apt repo.
- **Deferred** — SPC700 (SNES audio CPU; needs a UI surface). See the project TODO.
