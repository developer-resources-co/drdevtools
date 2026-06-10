# Installing drmon (Linux)

drmon is the **Developer Resources Monitor** — a source-level debugger/monitor for the SNES (65816) and
Genesis (68000), originally a 1990s DOS product, now ported to Linux/ncurses. This page
covers building and launching it. For day-to-day use see the [User Manual](drmon-manual.md);
for tuning see [Configuration](configuration.md).

> **Current status — runs disconnected.** drmon builds and runs its full text UI, but the
> dev-link transport to a target is still stubbed: there is **no live target yet**. Wiring a
> real target (MAME) is Phase 2. Everything below works today; target-dependent features are
> flagged "needs a backend (Phase 2+)" throughout the manual.

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
breakdown (compat shims, the ncurses front end, the stubbed transport seam) see
[`linux/README.md`](../linux/README.md).

## Roadmap

- **Phase 2** — replace the stubbed dev-link (`linux/slio_stub.cpp`) with a bridge to a real
  target via MAME's debugger; live memory/registers/run/step/breakpoints come alive here.
- **Packaging** — a `.deb` for the foundry apt repo once drmon drives a target.
- **Genesis** — a `SYSTEM=GEN` build variant (68000); SNES is the only build today.
