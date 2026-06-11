# Phase 2 finish-up — EOF recovery, orphan immunity, disconnected TUI, fresh-clone test

**Date:** 2026-06-11  
**Status:** Open  
**Predecessor:** [2026-06-11-drmon-mame-backend.md](2026-06-11-drmon-mame-backend.md) — Phase 2 complete;
19/19 SNES and 11/11 GEN protocol tests passing.

## Context

Three verification steps from the parent plan are unrecorded, plus one code issue was found
during testing. This plan closes them out.

**Open items:**

1. **`mame_bridge.lua` EOF recovery (code fix)** — when a client disconnects without sending
   `BYE` (crashed process, `nc` Ctrl-C), `open_server()` is never re-called. Subsequent
   connections see `ECONNREFUSED` until MAME is restarted. Must be fixed before verifying
   steps 2–4 below.
2. **Parent plan step 4 — orphan immunity** — verify `kill -9` MAME mid-test-run and the
   next `task test-bridge` run passes (proves `test_bridge.sh`'s kill-by-port entry clause
   works).
3. **Parent plan step 10 — disconnected behavior** — run `task run` with no MAME process;
   confirm TUI is responsive and shows a dead-target state without crashing.
4. **Parent plan step 11 — fresh-clone reproducibility** — from a clean worktree (no `roms/`
   directory), `task test-bridge SYS=snes` and `SYS=gen` both pass using only the committed
   test ROMs in `test-roms/`.

On completion: all parent plan verifications will be recorded; Phase 2 `[wip]` → `[x]` in
`TODO.md`.

---

## Implementation — `mame_bridge.lua` EOF recovery

### Problem

`mame_bridge.lua` only calls `open_server()` on an explicit `BYE` command. If the client
drops the connection without sending `BYE`, `srv:read(4096)` keeps returning nil or empty
string every periodic tick. The bridge cannot distinguish this from a live-but-idle
connection, so it stays bound to the dead socket. No new client can connect until MAME is
restarted.

### Root cause

MAME's `emu.file` TCP socket is non-blocking: `read()` returns nil when no data is available
*and also* when the peer has closed the connection. There is no distinct EOF signal exposed
to Lua. We must infer EOF from behaviour: a connection that was once active (at least one
command successfully dispatched) and then goes silent for many ticks has almost certainly
dropped.

### Fix

Add two new state variables and track consecutive nil reads in `readline()`. After
`EOF_TICKS` consecutive nil ticks on a connection that has seen at least one command, call
`open_server()` to recover.

**New constants + state** (top of file, with other globals):

```lua
local EOF_TICKS      = 300   -- ~5 s at 16 ms/tick before treating silence as disconnect
local ever_connected = false -- true after first dispatch on this socket handle
local nil_read_ticks = 0     -- consecutive ticks with nil/empty srv:read
```

**`open_server()` additions** (inside the function, after existing resets):

```lua
ever_connected = false
nil_read_ticks = 0
```

**`readline()` replacement:**

```lua
local function readline()
    if not srv then return nil end
    local chunk = srv:read(4096)
    if chunk and #chunk > 0 then
        rx_buf = rx_buf .. chunk
        nil_read_ticks = 0
    else
        if ever_connected then
            nil_read_ticks = nil_read_ticks + 1
            if nil_read_ticks > EOF_TICKS then
                open_server()
                return nil
            end
        end
    end
    local line, rest = rx_buf:match("^([^\n]*)\n(.*)")
    if not line then return nil end
    rx_buf = rest
    return line
end
```

**`dispatch()` — set `ever_connected` on first call.** Add at the very top of the function
body, before any other logic:

```lua
ever_connected = true
nil_read_ticks = 0
```

(Resetting `nil_read_ticks` on every dispatch ensures a slow interactive `nc` session that
goes quiet between commands never false-trips the EOF detection.)

### Edge cases

| Scenario | Outcome |
|---|---|
| Live idle connection (no commands for >5 s, `ever_connected = false`) | No recovery — timer doesn't start until first command received; avoids triggering before MAME finishes its start-up sequence |
| Slow interactive `nc` session: command, long pause, command | `nil_read_ticks` resets on each dispatch → no false positive |
| Client crashes mid-session (no `BYE`) | `ever_connected = true`; after ≤300 nil ticks (~5 s), `open_server()` called → new client can connect |
| Clean `BYE` | `open_server()` called immediately, same as before |
| `EOF_TICKS` tuning | Named constant at top of file; lower it for unit-test environments, raise for very slow machines |

---

## Verification procedures

### V1 — EOF recovery (new)

Run with MAME already started (`task mame SYS=snes CART=…` in a separate shell).

```bash
# 1. Connect with nc, send one command (V), then Ctrl-C (no BYE).
echo "V" | nc 127.0.0.1 41816

# 2. Wait > 5 s (let the 300-tick watchdog fire).
sleep 6

# 3. Reconnect — expect the bridge to have re-opened the listener.
echo "V" | nc 127.0.0.1 41816
# Expected: ok drmon-bridge 1 5a22
```

Paste the second `nc` reply below:

```
ok drmon-bridge 1 5a22
second connect OK (bridge recovered)
```

**PASS** (2026-06-11) — bridge re-opened listener after ~5 s with no BYE sent.

### V2 — Orphan immunity (parent plan step 4)

```bash
# 1. Start MAME in the background, mimicking a test run.
env SDL_VIDEODRIVER=offscreen SDL_AUDIODRIVER=dummy \
  mame snes \
    -cart devsys/tools/drmon/linux/test-roms/drmon-test.sfc \
    -debug -debugger none \
    -autoboot_script devsys/tools/drmon/linux/mame_bridge.lua \
    -video none -nothrottle \
    &>/tmp/mame_orphan_test.log &
MAME_PID=$!

# 2. Let it start, then kill it hard (orphan the port).
sleep 3
kill -9 $MAME_PID

# 3. Immediately run test-bridge — kill-by-port on entry should clear the stale socket.
task test-bridge SYS=snes
```

Paste `test-bridge` output:

```
=== test_bridge.sh: SYS=snes driver=snes ROM=drmon-test.sfc ===
Connecting to 127.0.0.1:41816 ...
Connected.  SYS=snes
  PASS: V returns ok drmon-bridge 1 <cpu>
  PASS: V cpu shortname is 5a22
  PASS: REGS returns ok
  PASS: G returns 11 hex values
  PASS: P returns ok
  PASS: H returns stopped ...
  PASS: ? returns stopped after H
  PASS: R returns 32 hex chars (16 bytes)
  PASS: W returns ok
  PASS: R returns written pattern deadbeef
  PASS: B+ returns ok
  PASS: B- returns ok
  PASS: S <next> returns stopped ... step
  PASS: S advanced PC by at least 1 byte
  PASS: B+ ahead returns ok
  PASS: C returns ok
  PASS: bp fires after C (stopped ... bp)
  PASS: B- ahead returns ok
  PASS: RESET returns ok

Results: 19 passed, 0 failed
=== test_bridge.sh: PASS ===
```

**PASS** (2026-06-11) — 19/19 after kill -9 MAME; kill-by-port entry clause freed the port.

### V3 — Disconnected behavior (parent plan step 10)

Run with **no MAME process** active on either port (confirm: `fuser 41816/tcp 23946/tcp` returns nothing).

```bash
# snesmon
task run SYS=snes &
TUI_PID=$!
sleep 3
task shot SYS=snes
kill $TUI_PID

# genmon
task run SYS=gen &
TUI_PID=$!
sleep 3
task shot SYS=gen
kill $TUI_PID
```

Expected: TUI renders, status bar shows disconnected/dead state (e.g. `Slave Dead` or
`Not Connected`), no crash on either binary.

Paste screenshot or status bar text for snesmon:

```
☼ File Control Windows Macros Rate Settings Help       Thu Jun 11 07:55:17 2026


 SNESMon V2.1.30  Copyright 1991─1994 Developer Resources          Slave Dead
```

Paste screenshot or status bar text for genmon:

```
☼ File Control Windows Macros Rate Settings Help       Thu Jun 11 07:55:26 2026


 GenMon V2.1.30  Copyright 1991─1994 Developer Resources           Slave Dead
```

**PASS** (2026-06-11) — both TUIs render, status `Slave Dead`, no crash.

### V4 — Fresh-clone reproducibility (parent plan step 11)

```bash
# Create a temporary worktree from HEAD (no roms/ directory).
git worktree add /tmp/drmon-freshclone HEAD
ls /tmp/drmon-freshclone/roms 2>/dev/null && echo "FAIL: roms/ present" || echo "roms/ absent — good"

# Build inside the worktree (uses the committed source).
docker run --rm \
  -v /tmp/drmon-freshclone:/src \
  -v /tmp/drmon-freshclone-build:/build \
  -w /src/devsys/tools/drmon \
  drmon-build \
  bash -euc "cmake -S . -B /build -G Ninja && ninja -C /build"

# Run test-bridge for both systems.
pushd /tmp/drmon-freshclone
task test-bridge SYS=snes
task test-bridge SYS=gen
popd

# Clean up.
git worktree remove /tmp/drmon-freshclone
```

Paste SNES output:

```
=== test_bridge.sh: SYS=snes driver=snes ROM=drmon-test.sfc ===
Connecting to 127.0.0.1:41816 ...
Connected.  SYS=snes
  PASS: V returns ok drmon-bridge 1 <cpu>
  PASS: V cpu shortname is 5a22
  PASS: REGS returns ok
  PASS: G returns 11 hex values
  PASS: P returns ok
  PASS: H returns stopped ...
  PASS: ? returns stopped after H
  PASS: R returns 32 hex chars (16 bytes)
  PASS: W returns ok
  PASS: R returns written pattern deadbeef
  PASS: B+ returns ok
  PASS: B- returns ok
  PASS: S <next> returns stopped ... step
  PASS: S advanced PC by at least 1 byte
  PASS: B+ ahead returns ok
  PASS: C returns ok
  PASS: bp fires after C (stopped ... bp)
  PASS: B- ahead returns ok
  PASS: RESET returns ok

Results: 19 passed, 0 failed
=== test_bridge.sh: PASS ===
```

Paste GEN output:

```
=== test_bridge.sh: SYS=gen driver=genesis ROM=drmon-test.md ===
Connecting to 127.0.0.1:23946 ...
Connected.
  PASS: qSupported returns non-empty reply
  PASS: m200,10 returns 32 hex chars
  PASS: Z0,202,2 returns OK
  [c] running to bp at 0x202...
  PASS: stop reply after c at 0x202 (T05)
  PASS: g returns 140 hex chars
  PASS: PC at bp is 0x202
  [s] single step from 0x202...
  PASS: stop reply after s
  PASS: s advances exactly 2 bytes (NOP=4e71)
  PASS: z0,202,2 returns OK
  PASS: M/m round-trip at Genesis RAM
  [c+break] continue then break-in...
  PASS: break-in 0x03 returns stop reply

Results: 11 passed, 0 failed
=== test_bridge.sh: PASS ===
```

**PASS** (2026-06-11) — no `roms/` directory in worktree; both suites pass using committed
test ROMs in `test-roms/`.

---

## Files changed

| File | Change |
|---|---|
| `devsys/tools/drmon/linux/mame_bridge.lua` | EOF recovery: `ever_connected`, `nil_read_ticks`, `EOF_TICKS`; resets in `open_server()` and `dispatch()` |
| `docs/plans/2026-06-11-drmon-mame-backend.md` | Paste V2–V4 raw output into parent plan steps 4, 10, 11 once PASS |
| `TODO.md` | Phase 2 item `[wip]` → `[x]` after all four verifications pass |
