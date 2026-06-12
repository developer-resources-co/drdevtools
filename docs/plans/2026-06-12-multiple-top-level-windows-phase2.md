# Multiple top-level drmon windows — Phase 2: shared state

**Date:** 2026-06-12  
**Status:** Parked — design complete, not yet implemented  
**Phase 1 plan:** [2026-06-12-multiple-top-level-windows.md](2026-06-12-multiple-top-level-windows.md)

---

## Problem

Phase 1 gives N fully independent drmon processes. Each has its own MAME TCP connection
and its own breakpoint list — setting a breakpoint in window 1 is invisible to window 2,
and if both issue `B+ addr` concurrently the bridge responses can interleave.

Phase 2 adds a lightweight IPC layer so:
- Breakpoints set in any window are reflected in all windows.
- The symbol file path is shared — load symbols once, all windows see them.
- Only one process issues `B+`/`B-` to MAME at a time (hub model).

---

## Mechanism: Unix socket hub, first process is server

On startup every drmon process attempts to connect to
`$XDG_RUNTIME_DIR/drmon/state.sock` (falling back to `/tmp/drmon-$UID-state.sock`).

- **Connect fails** → no peers yet; bind the socket and become the **hub**. The hub
  owns the canonical breakpoint list and is the only process that issues `B+`/`B-` to MAME.
- **Connect succeeds** → become a **client**. On connect, the hub sends a `SYNC` dump
  of all current breakpoints and the active symbol-file path. The client applies them
  locally (sets its own display state; does *not* re-send to MAME).

---

## Wire protocol (line-delimited text, UTF-8)

| Message (client → hub) | Meaning |
|------------------------|---------|
| `BP+ <hex-addr>` | Request: set breakpoint at address |
| `BP- <hex-addr>` | Request: clear breakpoint |
| `SYM <path>` | Broadcast: loaded this symbol file |
| `BYE` | Orderly disconnect |

| Message (hub → clients) | Meaning |
|-------------------------|---------|
| `BP+ <hex-addr>` | Notify: breakpoint added |
| `BP- <hex-addr>` | Notify: breakpoint removed |
| `SYM <path>` | Notify: symbol file path |
| `SYNC BP+ <addr>…\nSYNC END` | Sent once on new-client connect: full current state |

Hub workflow on `BP+ addr` from client C:
1. Dedup — if already in set, broadcast confirm and return.
2. Issue `B+ addr` to MAME; wait for ack.
3. Add to canonical set; broadcast `BP+ addr` to **all** clients (including C).

---

## New files

**`linux/state_hub.hpp`** / **`linux/state_hub.cpp`** — `StateHub` class:

| Method | Role |
|--------|------|
| `bool tryBind()` | Try to become hub (bind socket + listen). Returns false if a peer already answers. |
| `bool tryConnect()` | Connect as client; receive `SYNC` dump and apply it. |
| `void poll()` | Called from main loop (non-blocking `MSG_DONTWAIT`); dispatches incoming messages. |
| `void sendBpSet(uint32_t addr)` | Client requests breakpoint set (hub issues to MAME + broadcasts). |
| `void sendBpClear(uint32_t addr)` | Client requests breakpoint clear. |
| `void sendSymbolPath(const char*)` | Broadcast symbol file path to all peers. |

---

## Integration points

| Location | Change |
|----------|--------|
| `drmon.cpp Init()` | After `InitManager()`: `g_stateHub.tryBind() \|\| g_stateHub.tryConnect()` |
| `break.cpp` set/clear | After local update: `g_stateHub.sendBpSet/Clear(addr)` |
| `StateHub::poll()` BP callback | Call `AddBreakPt` / `RemoveBreakPt` on local break window |
| `linux/ncurses_io.cpp` main loop | Add `g_stateHub.poll()` each iteration |
| `linux/dap/session.cpp` | DAP `setInstructionBreakpoints` can also connect as hub client |
| `SpawnNewWindow()` | No change — child inherits env and discovers socket on its own |

---

## Hub exit / orphan recovery

When the hub exits, the socket file is removed via `atexit`. The next client to see EOF
on `poll()` calls `tryBind()` to elect itself hub, re-issues all current `B+` commands
to MAME (from its own local state, which was kept in sync), and starts accepting peers.

---

## Verification (future)

1. Two windows open; set breakpoint in window 1; it appears in window 2's Breakpoint list.
2. Clear breakpoint in window 2; it disappears in window 1.
3. Load symbols in window 1; window 2's disassembly shows labels.
4. Close hub window; one of the clients promotes to hub; breakpoints remain set in MAME.
5. Three-way: set in window 1, clear in window 2, window 3 stays consistent throughout.
