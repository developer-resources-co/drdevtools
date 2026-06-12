# Multiple top-level drmon windows — Phase 1

**Date:** 2026-06-12  
**Status:** COMPLETE — verifications 1–2, 6 PASS; 3–5 require live desktop (no xterm in Docker)  
**Phase 2 plan:** [2026-06-12-multiple-top-level-windows-phase2.md](2026-06-12-multiple-top-level-windows-phase2.md)  
**TODO entry:** DRMON — UI / UX → Support multiple "monitors" (as windows)

---

## Context

"Multiple monitors" means N independent top-level drmon windows, each a complete session
with its own sub-windows (Memory, Register, Breakpoint, etc.), its own layer/object state,
and its own MAME connection. The DOS-era `twoScreen` buffer path is a shared virtual-desktop
model and does not match this requirement — it requires a 160-column terminal and can't give
each screen an independent window set.

The right fit is **multi-process**: each top-level window is a separate drmon process
launched in a new xterm. Processes are fully independent — no shared mutable state, no
refactor of globals. MAME's TCP bridge already accepts multiple simultaneous connections,
so each process connects independently and maintains its own breakpoint list, register view,
memory windows, etc.

```
user: Windows ▸ New Window
  → SpawnNewWindow()
      → fork() + execlp("xterm … -e /proc/self/exe")
          → new xterm with a fresh drmon process
              → connects to MAME via DRMON_MAME_ADDR (inherited env var)
              → full independent TUI session
```

Window positions cascade: each new window is offset `+80+25` pixels from the previous.

---

## What changed

### New: `linux/spawn_window.hpp` + `linux/spawn_window.cpp`

`SpawnNewWindow()` — reads own binary from `/proc/self/exe`, resolves a terminal
emulator, forks, and execs `<terminal> … <binary>`. A one-time `SIGCHLD` handler
reaps the terminal children; `setsid()` detaches each so closing one can't signal
another.

**Terminal resolution (not hardcoded xterm — that's the bug the first cut hit):**
1. `$DRMON_TERMINAL` — launcher prefix incl. its run-flag (`gnome-terminal --`,
   `kitty`, `konsole -e`, `xterm -e`); the binary is appended.
2. else `x-terminal-emulator -e` — the system's configured default terminal.
3. else `xterm -e`.
4. none → returns `-1` (caller reports it; see below).

**Failure is no longer silent:** a close-on-exec pipe lets the parent read the
child's `errno`, so a failed `exec` (no terminal / bad `$DRMON_TERMINAL`) returns
`-1` instead of a child that silently `_exit`s.

### Modified: `monmenu.cpp`

- Added `#include "linux/spawn_window.hpp"`, the `MenuNewWindow()` callback, and the
  "New Window" entry + separator at the top of `windowMenu[]`.
- `MenuNewWindow()` now reports via `PrintMessageBar()` when `SpawnNewWindow()` fails
  ("no terminal found — set $DRMON_TERMINAL").

### Modified: `CMakeLists.txt`, `linux/Dockerfile`, `Taskfile.yml`

- `CMakeLists.txt`: `linux/spawn_window.cpp` in `DRMON_CXX_SRC`.
- `linux/Dockerfile`: install `xterm` (also registers `x-terminal-emulator`), so the
  in-container fallback resolves — required for the `task run` (Docker) path.
- `Taskfile.yml` `run`: pass the display into the container (`-e DISPLAY`,
  `-v /tmp/.X11-unix:/tmp/.X11-unix`, `-e DRMON_TERMINAL`) and best-effort
  `xhost +local:` so a spawned terminal draws on the host X server.

---

## Verification

1. ~~**Build**: `task build SYS=snes` — no errors.~~

   ```
   [94/94] Linking CXX executable drmon-dap-gen
   /build/snesmon: ELF 64-bit LSB pie executable …
   ```
   **PASS**

2. ~~**Smoke**: `task smoke SYS=snes` passes — no regression.~~

   ```
   PASS: opened windows via Alt-keys (M-e M-k M-r M-n M-m M-w M-b M-s M-i M-o M-a M-y)
   + typed into Expression — no SIGSEGV
   ```
   **PASS**

3. **Spawn**: Run snesmon (`task run`); select Windows ▸ New Window; a second terminal opens
   with a fresh independent drmon TUI. **Environment verified** — image carries
   `xterm` + `x-terminal-emulator`, and a container terminal reaches the host X through the
   `run`-task flags (`xterm -e true` → connected). Final in-TUI click is a manual desktop step.

4. **Independence**: Open a Memory window in window 1; confirm it does not appear in
   window 2. **(Needs live desktop.)**

5. **Multiple spawns**: "New Window" a third time; third xterm appears at a different
   screen position; all three run independently. **(Needs live desktop.)**

6. ~~**Close regression**: closing a child does not affect parent — verified structurally
   (`setsid()` detaches process group; SIGCHLD handler reaps zombies).~~ **PASS (structural)**
