# Spike + tag for the multi-window ("multiple monitors") feature

## Context

Goal: before committing to the ~400–600 LOC Option-A refactor (one drmon process driving N
terminals so several windows view **one** running game), **prove the architecture empirically** —
because the *last* multi-window design rested on a premise I asserted without testing ("MAME accepts
multiple connections") that turned out false. This time the unproven claims are:

1. One process can run **N independent ncurses `SCREEN`s** via `newterm()` on N PTYs, `set_term()`
   between them, and render to each independently.
2. That process can **multiplex input** from N terminals (`poll()` the PTY fds) and route each key to
   the right SCREEN.
3. **`xterm -S`** can attach a real visible window to a pre-opened PTY (drive N xterms from one
   process), instead of the current "exec a whole new drmon process" model.

The user may **shelve and revert** the feature, so I also drop a **tag** marking the existing
multi-window commits for a clean revert later.

## Part 1 — the spike (throwaway, proves the architecture)

New throwaway file `devsys/tools/drmon/linux/spikes/multiterm_spike.c` (+ a tiny build/run line; not
wired into CMake). One small C program, two modes:

**`headless` mode (deterministic, no X needed) — proves risks #1 and #2:**
- `openpty()` twice → two `(master, slave)` pairs.
- `newterm("xterm", out, in)` on each **slave** → two independent `SCREEN*`.
- `set_term(s0)`, draw `"SCREEN 0"`; `set_term(s1)`, draw `"SCREEN 1"`; `wrefresh` each.
- Read back each **master** and **assert** it received its own SCREEN's bytes (independent render).
- Inject a distinct keystroke into each master; `poll()` the slaves; on readiness `set_term` the
  matching SCREEN and assert `wget_wch` returns the injected key (correct input routing).
- Print `SPIKE PASS/FAIL` per assertion. Runs anywhere (container or host), no display.

**`xterm` mode (needs a reachable X display) — proves risk #3:**
- Same PTYs, but `fork`+`execlp("xterm", "xterm", "-S<cc>/<masterfd>", …)` (clear `O_CLOEXEC` on the
  master so it survives exec); ncurses uses the **slave**.
- Self-verifying without eyeballs: write a Cursor-Position-Report query (`ESC[6n`) to the slave; only a
  live attached terminal replies `ESC[<row>;<col>R`. Reading that reply **proves xterm attached and is
  pumping the PTY bidirectionally**. (If `DISPLAY` is reachable, also grab an `xwd`/`import` screenshot
  as a bonus.)
- If no X is reachable in this session, report Part 2 honestly as "mechanism coded, handshake not
  exercised here — run `./multiterm_spike xterm` on a desktop," rather than claiming it works.

Build/run: `gcc -o /tmp/multiterm_spike multiterm_spike.c -lutil -lncursesw` (slave path), run in the
container (`xterm`, X passthrough already wired in the `run` task) or on the host if it has xterm +
`$DISPLAY`.

**Decision gate:** headless PASS + xterm CPR handshake → architecture confirmed, Option A is viable →
report back and (separately) decide whether to build it. Any FAIL → report exactly which leg broke; do
**not** proceed to the refactor on a shaky leg.

## Part 2 — tag the existing multi-window code (revert safety)

The shipped (multi-process) multi-window feature is **three non-contiguous commits**:
- `033527c` feat — New Window spawn (spawn_window.cpp/.hpp, monmenu, CMakeLists, 2 plan docs)
- `9bf0879` fix — configurable terminal + Dockerfile xterm + Taskfile run flags
- `8567a03` fix — Taskfile `xhost +local:` quoting (current HEAD)

`d28c9d8` (system cppdap) sits between the last two and is **unrelated** — so a range revert is wrong.

Drop an **annotated tag** `drmon-multiwindow-v1` at `8567a03`, message listing the three commits and
the exact revert recipe:

```
git tag -a drmon-multiwindow-v1 8567a03 -m "Multi-window (multi-process spawn) feature.
Commits: 033527c, 9bf0879, 8567a03 (NOT d28c9d8 — unrelated cppdap).
Revert: git revert --no-commit 8567a03 9bf0879 033527c"
```

Tag is local (push only if asked). Leave the unrelated `.claude/memory/` untracked dir alone.

## Verification

1. `gcc … multiterm_spike.c -lutil -lncursesw` compiles clean.
2. `./multiterm_spike headless` → all `SPIKE PASS` (two independent SCREENs render + route input).
3. `./multiterm_spike xterm` (where X reachable) → `ESC[6n` → `ESC[…R` reply received (xterm attached).
4. `git tag -n drmon-multiwindow-v1` shows the annotated message; `git tag --points-at 8567a03`
   lists it.

## Non-goals

- Not building Option A yet — this only confirms it *can* be built and protects the revert path.
- No change to the MAME bridge or shared debug state.
