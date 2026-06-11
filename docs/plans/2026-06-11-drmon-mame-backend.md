# Phase 2 — MAME backend for drmon (SNES + Genesis)

**Date:** 2026-06-11
**Status:** In progress — both binaries build + transport works; architecture **resolved**
after spikes (SNES = custom bridge + `emu.pause` + software step; Genesis = MAME gdbstub /
native step). See **[Final implementation plan](#final-implementation-plan-post-spike--split-architecture)** — it is the authoritative spec; the
Architecture/Protocol/Implementation-steps sections above it are the original unified-bridge
design, superseded for Genesis.
**Predecessor:** [2026-06-10-port-drmon-linux.md](2026-06-10-port-drmon-linux.md) (Phases 1/1.5)

## Context

Phases 1/1.5 are done: drmon (the 1994 DR Monitor source-level debugger) compiles, links,
and runs its ncurses TUI on Linux — but disconnected, because the SNES dev-link transport
(`snesio.asm`, talking to a parallel-port dev board) is stubbed out by
`devsys/tools/drmon/linux/slio_stub.cpp`. Phase 2 gives drmon a live target again: a game
running in MAME, driven over MAME's debugger. Outcome: set breakpoints, step, continue,
inspect/edit memory and registers of a real running game from drmon's TUI.

**Scope: both systems.** Phase 2 covers `snesmon` (65816, MAME `snes` driver) **and**
`genmon` (68000, MAME `genesis`/`megadriv` driver). The genmon Linux build does not exist
yet — Phase 1 was SNES-only — so Phase 2 includes bringing up `SYSTEM=GEN` compile+link on
the existing compat layer, and unifying the two systems' backend contracts so one bridge
serves both.

## Verified findings (decide the architecture)

1. **gdbstub is not viable for SNES.** MAME 0.277's `gdb_register_maps`
   ([debuggdbstub.cpp:654–680](https://github.com/mamedev/mame/blob/mame0277/src/osd/modules/debugger/debuggdbstub.cpp))
   has **no 65816/5A22 entry** — `mame snes -debugger gdbstub` cannot work without patching
   MAME. (It *does* map `m68000`, so Genesis alone could ride gdbstub — but RSP cannot
   express either system's beyond-CPU state, see finding 6, so one custom bridge for both
   beats a split stack.) *Superseded in part: the hold spike flipped this calculus for
   Genesis — see the [final plan](#final-implementation-plan-post-spike--split-architecture).*
2. **Stock MAME Lua can serve TCP sockets.** `emu.file` opens
   `socket.127.0.0.1:<port>` paths ([luaengine.cpp:1020](https://github.com/mamedev/mame/blob/mame0277/src/frontend/mame/luaengine.cpp),
   [posixsocket.cpp](https://github.com/mamedev/mame/blob/mame0277/src/osd/modules/file/posixsocket.cpp));
   with the create flag it listens (one client). MAME's own
   [Lua gdbstub plugin](https://github.com/mamedev/mame/blob/master/plugins/gdbstub/init.lua)
   is shipped proof of the whole mechanism (socket + periodic pump + debugger API).
3. **Lua keeps running while the debugger is stopped.** The stopped-loop
   `debugger_cpu::wait_for_debugger()` calls `emulator_info::periodic_check()` →
   `lua()->on_periodic()` ([debugcpu.cpp:446](https://github.com/mamedev/mame/blob/mame0277/src/emu/debug/debugcpu.cpp) →
   [mame.cpp:467](https://github.com/mamedev/mame/blob/mame0277/src/frontend/mame/mame.cpp)).
   So an `emu.register_periodic` socket pump services requests in both run and stop states.
4. **The correct seam is the `slaveio.hpp` contract, not the comram wire protocol.**
   `slaveio.hpp` is titled *"These are the calls that must be implemented for each
   platform"* — the per-platform backend contract (~25 functions), with the existing
   `slioemul.cpp` as precedent (selected by `which.cpp`'s `#include` chain). Emulating one
   level lower (board/comram/wormhole, keeping `sliosnes.cpp`/`sliogen.cpp`) is rejected
   because:
   - `SlaveSetBkPt` patches a break instruction into target memory (1-byte BRK on SNES,
     16-bit trap on 68000) — worked on dev-board emulation RAM, **cannot work on a MAME
     cartridge ROM region** (writes don't stick). Native MAME breakpoints work in ROM.
   - `sliosnes.cpp`'s manual branch-following single-step
     ([sliosnes.cpp lines 375–571](https://github.com/developer-resources-co/drdevtools/blob/main/devsys/tools/drmon/sliosnes.cpp#L375-L571))
     exists because the dev board couldn't hardware-step; MAME steps natively.
   - Faithfully emulating the wormhole/comram handshake is pure overhead with no consumer.

   `slio_stub.cpp` itself stays (trimmed comment) — it still provides the residual
   asm-exported symbols (`SwapWord`/`SwapLong`/`GetAttention`/…) that the still-compiled
   `board.cpp` references.
5. **The Genesis contract has drifted and must be unified.** `gslaveio.hpp` (the
   Genesis/"sfx board" header) diverges from `slaveio.hpp` in signatures (e.g.
   `InitSlaveIO(void)` vs `InitSlaveIO(int, ushort)`) and carries its own opcode set
   (`SL_CMD_*`). Phase 2 normalizes genmon onto the shared `slaveio.hpp` contract — per
   that header's own claim ("same for every platform") — updating GEN call sites in the
   same commit. The GENESIS-specific accessors (`ReadSlaveVDP`, CRAM, VSRAM) stay declared
   in `slaveio.hpp`'s `#ifdef GENESIS` block.
6. **CPU-agnostic target mapping.** MAME 0.277 is installed from apt (`/usr/games/mame`).
   SNES: `maincpu` = 5A22 (shortname `5a22`), program space = the full 24-bit bus, drmon's
   flat addresses (`PCL = PB<<16 | PC`,
   [reg.cpp:435](https://github.com/developer-resources-co/drdevtools/blob/main/devsys/tools/drmon/reg.cpp#L435))
   map directly. Genesis: `maincpu` = `m68000`, 24-bit bus, drmon GEN regs are 32-bit
   (NUMREGS 19: D0–D7, A0–A7/SSP, USP, SR, PC). Beyond-CPU state (SNES PPU/SPC700;
   Genesis VDP/CRAM/VSRAM/Z80) is reachable through MAME devices later — a key reason the
   bridge protocol stays custom rather than RSP, which has no notion of secondary address
   spaces or co-CPUs.

## Architecture

```
┌──────────────── drmon (TUI, in container) ────────────────┐      ┌────────── host ───────────┐
│ break.cpp / control.cpp / memory.cpp / reg.cpp …          │      │ MAME 0.277 (snes/genesis)  │
│        │  slaveio.hpp contract (unified, both systems)    │      │  ┌──────────────────────┐  │
│        ▼                                                  │ TCP  │  │ mame_bridge.lua      │  │
│ linux/sliomame.cpp  ── client, read cache, reconnect ─────┼──────┼─▶│ emu.file socket srv  │  │
│ (replaces sliosnes.cpp / sliogen.cpp via which.cpp)       │41816 │  │ → debugger Lua API   │  │
└───────────────────────────────────────────────────────────┘      │  └──────────────────────┘  │
   built twice: snesmon (-DSNES) + genmon (-DGENESIS)              └────────────────────────────┘
```

- **`linux/mame_bridge.lua`** — autoboot script: listens on `127.0.0.1:41816`, pumps the
  socket from `emu.register_periodic`, dispatches commands to the
  [Lua debugger API](https://docs.mamedev.org/luascript/ref-debugger.html)
  (`cpu.spaces["program"]`, `cpu.state[]`, `cpu.debug:bpset/bpclear/step/go`,
  `debugger.execution_state`). CPU-agnostic: per-CPU register-alias tables keyed by
  `cpu.shortname` (`5a22`, `m68000`) — the same pattern MAME's own gdbstub plugin uses.
  Keeps an `addr → bp index` map and a pending-step/halt state machine so `S`/`H` reply
  only once actually stopped.
- **`linux/sliomame.cpp`** — implements the unified `slaveio.hpp` contract as a TCP
  client: blocking request/response with 1 s timeout, throttled non-blocking reconnect
  (2 s) so the TUI stays responsive with no MAME running (today's disconnected behavior
  preserved). Included by `which.cpp` exactly like its siblings (`MAMEBACKEND` arm ahead
  of the `sliosnes.cpp`/`sliogen.cpp` arms; `SNES`/`GENESIS` stay defined for
  disassembler/reg layout). System-neutral by construction: register traffic is driven by
  drmon's own `regNameArray[]`/`NUMREGS`, so the same TU compiles into both binaries.
  Defines the globals the legacy slio TUs owned (`bConvertUserBreaks` — now `FALSE`,
  no break-patching under native bps — `slaveDead`, `snesStatus`, …; final list from the
  linker, per build).
- **Read cache** in sliomame.cpp: aligned 1 KB blocks serving the disasm/memory windows
  (they read in tiny chunks; uncached, a window refresh would cost one ~16 ms round-trip
  per chunk). Invalidated on any write, step, continue, halt, reset, or run-state change.

## Protocol (drmon ↔ bridge)

A custom minimal protocol — **drmon's own 1994 opcode protocol re-framed as ASCII lines**:
each command is the 1:1 image of a `slaveio.hpp` contract function / `SNESCOMMANDS` opcode
(`R`/`W` = `SNES_SEND_SNES2PC`/`PC2SNES`, `G`/`P` = `SNES_REGS_*`, `C` = `SNES_GO`,
`S` = `SNES_STEP`, `B±` = `SNES_SET/CLEAR_BREAK`, `?` = the `SNES_ASYNC` event channel —
and equally of the Genesis `SL_CMD_*` set, which is a subset), minus the comram/wormhole
hardware framing. Deliberately **not** one of the standards:

- **Not DAP** — wrong layer. DAP is the *editor ↔ debugger* protocol (stack frames, scopes,
  variables); drmon-core already owns those concepts. What it needs below is a dumb
  *debugger ↔ target* pipe. DAP arrives in Phase 3 on drmon's other side (cppdap → VS Code),
  mirroring the original stack: UI ↔ drmon-core ↔ dev-link ↔ target.
- **Not GDB RSP** — MAME's gdbstub has no 65816 map (Genesis-only coverage), and RSP can't
  express either console's beyond-CPU state (PPU/SPC700, VDP/Z80) — a dead end as the
  primary channel for console-specific debugging. (Adding a 65816 map upstream *is* a
  worthwhile standalone contribution — see the
  [65816 gdbstub investigation](../investigations/2026-06-11-65816-gdbstub.md) — but it
  doesn't belong in this phase's critical path.)
- **Not MAME console command strings** — replies would mean scraping debugger console text;
  the Lua side calls the structured debugger API (`cpu.debug:bpset()`, `space:read_u8()`)
  directly.

The 1:1 mapping keeps `sliomame.cpp` a thin pass-through, and the wire is hand-debuggable
with `nc 127.0.0.1 41816`.

Newline-terminated ASCII over TCP `127.0.0.1:41816` (override: `DRMON_MAME_ADDR=host:port`),
strict request→response, single client. Hex args, 24-bit addresses, register values as
plain hex (width per system: 16-bit-masked on SNES, 32-bit on GEN — client-side concern).

| Cmd | Reply | Maps to |
|---|---|---|
| `V` | `ok drmon-bridge 1 <cpu.shortname>` | `SlaveGetVer` handshake / liveness |
| `REGS <name,…>` | `ok` | client announces its `regNameArray[]` order; Lua resolves each drmon name to a MAME state entry via the per-CPU alias table |
| `R <addr> <len>` | `<hexbytes>` (chunk ≤ 4096) | `ReadSlaveData`, `MemRead`, `SaveFileFromSlave` |
| `W <addr> <hexbytes>` | `ok` | `WriteSlaveData`, `WriteSlaveDatum`, `LoadFileToSlave`, fill/copy loops |
| `G` | hex fields in announced order | `GetRegsFromSlave` |
| `P <fields>` | `ok` | `PutRegsToSlave` |
| `B+ <addr>` / `B- <addr>` | `ok` | `SlaveSetBkPt` (returns mem word via `R`) / `SlaveBkClr` |
| `S` | `stopped <pc> step` | `SingleStep`, `StepOverBreak` (native step; no patch shuffle) |
| `C` | `ok` | `SlaveRun` |
| `H` | `stopped <pc> halt` | `SlaveStop` |
| `?` | `running` \| `stopped <pc> <reason>` | `HandleSlaveInput` poll (~50 ms throttle); reason ∈ `bp step halt unknown` |
| `RESET` | `ok` (soft reset) | `SlaveRestart` |
| `BYE` | closes; Lua reopens listener | `GoodByeSlave` |

`<pc>` is MAME's current instruction address (GENPC); the client assigns it to
`REG_INSTRUCTIONPOINTER` (`PCL` on SNES, `PC` on GEN). Async-exception path:
`HandleSlaveInput` polls `?`; on running→stopped transition it fetches regs (`G`),
classifies via drmon's own `FindBreak`/`BreakPointHit` (same flow the legacy `GetMessage`
msgType 1 used), and drives the existing status UI (`PrintMessageStatus`, `slaveRunning`,
`modeUpdate`).

Register alias tables (in `mame_bridge.lua`, per `cpu.shortname`; exact MAME state names
enumerated by the spike):

- `5a22`: drmon `A X Y FLAGS EMUL D DB PB SP PC PCL` → expected MAME `A X Y P E D DB PB S PC`
  (PCL composed/decomposed Lua-side; masks per `regMaskArray`).
- `m68000`: drmon `D0–D7 A0–A7 USP SR PC` → MAME `D0–D7 A0–A7 USP SR PC` (A7-vs-SSP/ISP
  active-stack nuance resolved in the spike — MAME exposes USP/ISP separately).

**Post-spike deltas to this table** (see the
[final plan](#final-implementation-plan-post-spike--split-architecture)): `S` gains
client-computed candidate addresses (`S <next> [<alt>]`), `H`/`C`/`?` are implemented via
`emu.pause()`/`emu.unpause()`/`machine.paused` rather than `execution_state`; and the
**entire table applies to SNES only** — Genesis moves to GDB RSP over `-debugger gdbstub`
(the `m68000` alias table stays in the Lua bridge for hand-debugging with `nc`, but genmon
doesn't ship on it).

**Out of scope (stubbed safe, protocol reserved):** SNES PPU/CRAM/OBJRAM reads
(`ReadSlavePPU` → zeros) and SPC700 ops; Genesis VDP/CRAM/VSRAM (`ReadSlaveVDP` etc. →
zeros) and Z80; write-protect / BRK-on-write (no-ops); `SNES_SEARCH`. ROM-region writes
won't stick (MAME ROM) — work-RAM writes do (SNES `$7E0000+`, GEN `$FF0000+`); documented
in the manual.

## Implementation steps (original — status annotated; remaining work lives in the [final plan](#final-implementation-plan-post-spike--split-architecture))

1. ~~**TODO** — Phase 2 entry → `[wip]`, linked to this plan; fold the separate "Genesis target (SYSTEM=GEN)" TODO item into this phase.~~ **Done** — both entries `[wip]` in `TODO.md`, GEN item folded.
2. ~~**Spike (½ day, de-risks everything)** — throwaway Lua via `-autoboot_script` against both drivers: socket while running/stopped, state-name dumps, bpset+go, RAM/ROM reads.~~ **Done** — output recorded in [Spike results](#spike-results-step-2--mame-0277-apt-2026-06-11) below; follow-up hold/step spikes in the build findings.
3. **Contract unification — dropped as moot for the Linux build.** `moninc.hpp` includes
   only `slaveio.hpp` (`gslaveio.hpp` is in no Linux TU), and `drmon.cpp:98` already calls
   the unified `InitSlaveIO(int, ushort)` signature — which `sliomame.cpp` implements for
   both binaries. `gslaveio.hpp` alignment only matters if the legacy DOS GEN path is ever
   rebuilt; deferred indefinitely (no TODO entry warranted).
4. ~~**`genmon` build bring-up** — CMake two executables (`snesmon`/`genmon`), shared source list, dialect-fix pass over GEN-only TUs, `smoke`/`shot` extended.~~ **Done** — both binaries build clean; `smoke`/`signals` PASS on both (see build findings).
5. **`linux/mame_bridge.lua`** — **landed, but pre-redesign**: halt still rides
   `execution_state` (proven transient) and step is broken under `-debugger none`.
   v2 redesign specified in the final plan, step 1.
6. **`linux/sliomame.cpp`** — **landed** (unified contract, socket client, read cache,
   globals; `which.cpp` `MAMEBACKEND` arm; CMake option; `slio_stub.cpp` comment updated;
   drives 14/15 protocol assertions). `SingleStep()` candidate computation pending —
   final plan, step 2.
7. ~~**Taskfile** — `task mame SYS=… CART=…`, `task run` gains `--network=host` + `DRMON_MAME_ADDR`, `task test-bridge`.~~ **Done** — plus `SYS=snes|gen` threaded through `build`/`run`/`debug`/`shot`/`smoke`/`signals`. (`task mame SYS=gen` switches to gdbstub flags in the final plan, step 7.)
8. **Regression guard: `task test-bridge`** — **landed with gaps**: the harness kills by
   PID only (the orphan-port findings below mandate kill-by-port), the
   bp-hit-while-running assertion (`B+` → `C` → `?` reports `stopped … bp`) specified here
   was never written into `test_bridge.py`, and the GEN ROM glob (`-name "*.md"`) also
   matches `README.md` in `test-roms/`. Final plan, steps 3 and 5.
9. **Test ROMs** — **partially done**: Genesis `linux/test-roms/drmon-test.md` is vendored
   (1 KB own-work ROM, reset vectors + `bra *` at `$200` — safe to commit), **but its
   generator script lives only in shell history** (per `test-roms/README.md`) — no repo
   source to regenerate or modify it. SNES **works today** via the user-supplied ROM in
   `roms/snes/` (the harness finds it), but that ROM is copyrighted and can never be
   committed — so `task test-bridge SYS=snes` is not reproducible from a fresh clone, and
   `roms/` is not gitignored. Final plan, steps 3–4.
10. **Docs, same turn as code** — open, unchanged (`linux/README.md`, configuration doc,
    [port-plan roadmap](2026-06-10-port-drmon-linux.md)), plus the split architecture and
    the GEN trade-offs from the final plan.

Bring-up order (revised): SNES bridge v2 end-to-end first, then the GEN gdbstub spike,
then `sliogdb.cpp` — genmon no longer reuses the Lua bridge.

## Files

| File | Action |
|---|---|
| `devsys/tools/drmon/linux/mame_bridge.lua` | new — MAME-side server, per-CPU tables |
| `devsys/tools/drmon/linux/sliomame.cpp` | new — unified-contract impl (client) |
| `devsys/tools/drmon/which.cpp` | `MAMEBACKEND` include arm (both systems) |
| `devsys/tools/drmon/CMakeLists.txt` | two targets `snesmon`/`genmon`; `DRMON_MAME_BACKEND` option |
| `devsys/tools/drmon/gslaveio.hpp` + GEN call sites | contract unification onto `slaveio.hpp` |
| `devsys/tools/drmon/linux/slio_stub.cpp` | keep; comment updated |
| `devsys/tools/drmon/linux/test_bridge.{sh,py}` | new — protocol regression test, both drivers |
| `Taskfile.yml` | `mame`, `test-bridge`; `run`/`smoke`/`shot` gain `SYS` + `--network=host` |
| `devsys/tools/drmon/linux/README.md`, `TODO.md`, port plan | docs |

## Risks

- **MAME Lua API churn** — developed/verified against the apt-pinned 0.277; version noted
  in README and checked by `task mame` precondition (`mame -version`).
- **GEN dialect grind unknown** — `dis68000.cpp` and the GEN arms of shared TUs were never
  compiled on Linux; bounded by the Phase 1 compat layer and the same fix patterns
  (Phase 1 went 329 → 0 errors).
- **State-name mismatches** (5A22, m68000 — incl. the A7/USP/ISP nuance) — spike
  enumerates; mapping is one Lua table per CPU.
- **`diag.rom` may assume dev-cart hardware**; no in-repo Genesis ROM — fallbacks per
  step 9.
- **Single-client listener** — Lua reopens the listening socket on EOF; sliomame.cpp
  reconnects with throttle, so either side can restart freely.
- **Step latency** — one periodic tick (~16 ms) per `S`; ≈60 steps/s held-key trace,
  faster than the original parallel-port link. Read cache covers the bulk-read paths.

Cost: $0 — entirely local (host MAME from apt, drmon in the existing Docker toolchain).

## Spike results (step 2) — MAME 0.277 apt, 2026-06-11

### 5A22 (SNES) actual state names
```
A  X  Y  P(=FLAGS)  E(=EMUL)  D  DB  PB  S(=SP)  PC  GENPC(=PCL=PB<<16|PC)
NMI  IRQ  CURPC  CURFLAGS  fastROM   ← internal, not mapped
```
Program space name: `program`. Socket `open()` returns `nil` on success.

### m68000 (Genesis) actual state names
```
D0–D7  A0–A6  USP  SP(=A7/SSP)  SR  PC  IR  CURPC  CURFLAGS  ← plus GENPC=CURPC
```
`SP` = active supervisor stack pointer (drmon's `A7`/`REG_SSP`). Program space: `program`.

### Corrected register alias tables
- `5a22`: drmon `FLAGS`→MAME `P`, `EMUL`→`E`, `SP`→`S`, `PCL`→`GENPC` (read: direct; write: decompose to PC+PB)
- `m68000`: drmon `A7`→`SP` (active SP in supervisor mode)

### Debugger API findings
- `cpu.debug:bpset()` **panics MAME 0.277** (Lua→C++ fatal error, uncatchable by pcall) — use `db:command("bpset <addr>")` instead
- `db:command("bpset <addr>")` → consolelog appends `"Breakpoint N set"` → parse N for bpclear
- `db:command("bpclear <N>")` → consolelog `"Breakpoint N cleared"`
- `db:command("go")`, `db:command("step")` both accepted ✓
- `db.execution_state = "stop"` registers a **transient** stop only — see "Hold semantics" below ⚠
- `cpu.state[name].value` = read; `cpu.state[name].value = v` = write ✓
- Memory: `cpu.spaces["program"]:read_u8/read_u32/write_u8` all work ✓; WRAM writes persist ✓
- Step is **asynchronous**: periodic sees exec=stop → step issued → exec=stop again in next tick; bridge must defer response

### Socket and execution state
- `emu.file("", 7):open("socket.127.0.0.1:41816")` = nil (success), one client
- `emu.register_periodic` **fires while machine is stopped** (exec=stop) ✓ — confirmed with -debugger none
- Machine starts in exec=stop with `-debug -debugger none`

## Build + integration findings (2026-06-11, post-build)

After wiring `sliomame.cpp` + `mame_bridge.lua` and getting both binaries to build,
end-to-end protocol testing surfaced two issues — one harness, one architectural.

### Build fixes (landed)
- `sliomame.cpp` is text-`#include`d via `which.cpp` **after** `moninc.hpp` already
  pulled in the drmon project headers (`break.hpp`, `message.hpp`, …), which lack
  include guards → redefinition errors. Fix: drop those `#include`s from `sliomame.cpp`.
- `WriteSlaveDatum` is already defined in `command.cpp` (applies WORDSWAP then calls
  `WriteSlaveData`) → duplicate-symbol link error. Fix: don't redefine it in `sliomame.cpp`.
- Both `snesmon` + `genmon` now build clean; `smoke`/`signals` PASS on both.

### Test-harness hygiene (orphan MAME processes)
A failed/aborted test leaves MAME holding `LISTEN` on `127.0.0.1:41816`. The next MAME
can't bind, so the test connects to the **stale orphan** mid-state and gets `ECONNRESET`
— which looks like a bridge crash but isn't. `test_bridge.sh` must kill by **port**
(`fuser -k 41816/tcp` or `lsof -ti :41816 | xargs kill`) on entry and exit, not just
`$MAME_PID`. On a clean port, runs are stable: **14/15 protocol assertions PASS**
(`V REGS G P R W B+ B- S C RESET`; the one failure is the hold issue below).

### ⚠ Hold semantics — the real blocker (decides the design)
Under `-debugger none`, **stops do not persist**. Measured against the `bra *` test ROM:

| Action | Observed |
|---|---|
| `H` (`execution_state="stop"`) | transient stop (deferred reply fires once) → **machine resumes** |
| `B+ <pc>` then `C` on the actively-executing address | **never stops** — runs through the bp forever |
| `S` (`db:command("step")`) | reports `stopped` once → **resumes immediately**; `?` reads `running` |

Root cause: a MAME break is held by the debugger backend's `wait_for_debugger()` loop.
The `none` backend has no such loop, so the CPU resumes the instant the periodic returns.
The shipped gdbstub plugin only holds because `-debugger gdbstub` **is** that loop. Our
custom bridge has no equivalent — so "break and stay broken to inspect" isn't there yet.
This invalidates the plan's implicit assumption (steps 5–6) that `execution_state="stop"`
+ a pending-reply state machine suffices; the bridge must **provide the hold itself**.

Candidate hold mechanisms — **spiked 2026-06-11** (NOP-sled test ROM; PC varies when
running, stable when frozen — see `/tmp/spike_hold.lua`):

1. **`emu.pause()` / `emu.unpause()`** — ✓ **THE ANSWER.** Pause/resume are **global
   functions on the `emu` table**, not methods on `manager.machine` (my first spike called
   the non-existent `manager.machine:pause()` → nil; the API probe showed the machine
   metatable has a read-only `paused` property but no `pause`/`resume` method). `emu.pause()`
   **truly freezes the CPU** — PC pinned, stable across polls; `emu.unpause()` cleanly
   resumes. Independent of the debugger backend. This is the halt/continue/inspect primitive:
   `H`→`emu.pause()`, `C`→`emu.unpause()`, `?`→`manager.machine.paused`.
2. **`db.execution_state = "stop"`** — ✗ transient (PC keeps varying after). Ruled out.
3. **Breakpoint + `go`** — ✓ holds, but it's a **re-trigger-on-revisit pseudo-hold,
   not a true stop**:
   - `bpset <pc>` + `go` → PC pinned at the bp address (stable across polls). ✓ for halt+inspect.
   - `bpclear` (remove the bp) → machine **immediately resumes**. So the hold *is* the bp.
   - `step` while the halt-bp is still set → PC returns to the bp addr (the bp re-catches
     after one sled loop) → **step appears not to advance**. ✗ for clean single-step.
   - Implication: the bp doesn't park the CPU in a persistent `wait_for_debugger` loop;
     it stops, the scheduler resumes, the CPU revisits the bp address and stops again.
     Fine for "halt & inspect a looping program", broken for instruction-stepping and
     unstable for inspecting straight-line code (registers drift between reads).

Conclusion: `-debugger none` has no persistent *debugger* hold, but **`emu.pause()`
provides a true CPU freeze** that's all we need for halt/continue/inspect. The redesign:

- **`H` (halt)** → `emu.pause()`; reply `stopped <pc> halt`. CPU frozen, regs/mem stable.
- **`C` (continue)** → `emu.unpause()`.
- **`?`** → `manager.machine.paused` → `running` / `stopped <pc> <reason>`.
- **`B+`/`B-`** → native `bpset`/`bpclear` (proven). On a bp hit while *running*, the
  re-trigger pins PC at the bp; the `?` poll detects it and the bridge calls `emu.pause()`
  to convert the pseudo-hold into a true freeze (stable for inspection).
- **`S` (step)** → ✗ **the `emu.pause` + `db:command("step")` combo does NOT give clean
  single-instruction granularity** (spiked `/tmp/spike_step.lua`): on a NOP sled (expect
  +2/step) the measured deltas were `+0, +4, +240, +4, +240, +4`, and PC drifted from the
  step-reply value (`f66`) to a different freeze point (`256`) afterward. Root cause:
  `emu.pause()` pauses the **scheduler**, but `db:command("step")` needs the scheduler
  *running* to advance one instruction — so the bridge must `emu.unpause()` first, and the
  machine then free-runs racily before the step's break is detected and re-paused. The two
  pause layers (scheduler vs debugger `execution_state`) don't compose into one-instruction
  stepping.

### Single-step: design fork (resolved — see final plan)
Halt/continue/inspect (`emu.pause`/`unpause`), regs, mem R/W, and bp set/clear are all
solid. **Only single-step is unsolved**, with three candidate tactics:

- **(A) Disassembler-driven step** *(recommended)* — drmon already links its own
  `dis68000`/`dis816`; compute the next PC (sequential length, or branch/jump target —
  with branch-following, exactly what 1994 drmon's `sliosnes.cpp` single-step did),
  set a one-shot bp there, `emu.unpause()`, hit, `emu.pause()`. Backend-agnostic, reuses
  proven logic, deterministic. Cost: wire the disasm "next-PC" helper into the step path.
- **(B) Pure-bp step from a true `execution_state=stop`** — reach a genuine debugger stop
  (bp hit, not `emu.pause`), then remove that bp, `db:command("step")`, re-add. Needs a
  spike to confirm step-from-real-stop yields exactly one instruction (earlier the lingering
  bp re-caught; clearing it resumed the machine — fragile).
- **(C) `-debugger gdbstub` for Genesis only** (has the `m68000` map; SNES stays custom) —
  gets MAME's real hold loop + native step for free on one system, at the cost of a split
  stack (rejected in finding 1, but the cleanest *correct* stepping for 68000).

**Resolved 2026-06-11:** **(A)** for SNES, **(C)** for Genesis — full spec in the next section.

### GEN gdbstub spike results (2026-06-11, MAME 0.277 apt)

Spike script: `linux/spikes/spike_gdbstub_gen.py`; ROM: `linux/test-roms/drmon-test.md`.

1. **Port**: MAME `-debugger gdbstub` default port = **23946**. Override via `-debugger_port`.
2. **g-packet layout** (140 hex chars = 70 bytes):
   - D0–D7: hex offsets 0–63 (8 × 4B big-endian) → drmon regs 0–7
   - A0–A7: hex offsets 64–127 (8 × 4B big-endian) → drmon regs 8–15
   - SR: hex offsets 128–131 (2B big-endian, 16-bit only!) → drmon reg 17
   - PC: hex offsets 132–139 (4B big-endian) → drmon reg 18
   - USP (drmon reg 16): not in RSP register set — read as 0, writes ignored
3. **Z0 in cartridge ROM**: `Z0,202,2` → `OK`; `c` → `T05` stop reply with PC=0x202 ✓
4. **`s` advances exactly one instruction**: NOP (4E71 = 2 bytes), `s` → PC 0x202→0x204 ✓
5. **Break-in 0x03**: while running, `\x03` → `T05` stop reply received ✓
6. **Restart (`R0`/`vRun`)**: empty reply — **not supported**. `SlaveRestart()` is a documented no-op on GEN.
7. **Initial notification**: `T05` emitted immediately on connect under `-debug`. Client must drain it before sending `qSupported`.

## Final implementation plan (post-spike — split architecture)

**Decision.** The step fork resolves differently per system:

- **SNES — (A): custom bridge, `emu.pause()` hold, disassembler-driven software step.**
  gdbstub cannot serve the 5A22 (finding 1), the spikes proved `emu.pause()` is a true CPU
  freeze, and the next-PC prediction logic already exists, proven, at
  [sliosnes.cpp:375–571](https://github.com/developer-resources-co/drdevtools/blob/main/devsys/tools/drmon/sliosnes.cpp#L375-L571)
  — flag-evaluated branches plus JMP/JML/JSR/JSL/RTS/RTL/RTI via memory/stack reads, built
  on `Disassem()` (`dis.hpp`), which already links into snesmon.
- **Genesis — (C): MAME `-debugger gdbstub`, native hold + native step.** The
  [C++ stub](https://github.com/mamedev/mame/blob/mame0277/src/osd/modules/debugger/debuggdbstub.cpp)
  maps `m68000`, and `-debugger gdbstub` **is** the persistent `wait_for_debugger()` hold
  loop that `-debugger none` lacks — the exact capability the hold spike showed cannot be
  synthesized from Lua alone. Native `s`/`Z0` give correct single-instruction stepping
  with zero prediction logic.

This **is** the split stack finding 1 originally rejected. That rejection assumed the
custom bridge could hold a stop on both systems; the hold spike falsified the assumption
for `-debugger none`, flipping the cost calculus: a software-step engine for the 68000
would duplicate what MAME ships for free. Finding 1's "one bridge" survives at the
*contract* level — both binaries implement the same `slaveio.hpp` contract; only the
transport TU differs.

```
snesmon ── slaveio.hpp ── sliomame.cpp ── custom ASCII protocol ──▶ mame_bridge.lua (Lua, :41816)
genmon  ── slaveio.hpp ── sliogdb.cpp  ── GDB RSP ────────────────▶ -debugger gdbstub (C++, MAME-native)
```

### SNES — bridge v2 (`mame_bridge.lua` + `sliomame.cpp`)

Replace the `execution_state` halt machinery (proven transient) with the pause primitive:

| Op | Bridge action (v2) |
|---|---|
| `H` | `emu.pause()`; reply `stopped <pc> halt` immediately (the spike showed the freeze takes effect synchronously; if a corner case proves otherwise, the existing deferred-reply machinery covers it) |
| `C` | clear any internal one-shot step bps; `emu.unpause()`; reply `ok` |
| `?` | if `manager.machine.paused` → `stopped <pc> <reason>`; else if a bp pseudo-hold is detected (consolelog "Stopped at breakpoint", or `execution_state == "stop"` while unpaused) → call `emu.pause()` first (**bp-auto-pause**, converting the re-trigger pseudo-hold into a stable freeze), reply `stopped <pc> bp`; else `running` |
| `S <next> [<alt>]` | set internal one-shot bp(s) at the client-computed candidate address(es); `emu.unpause()`; on hit: clear the one-shots, `emu.pause()`, deferred-reply `stopped <pc> step`. **Watchdog:** if nothing hits within ~500 ms, `emu.pause()` anyway and reply `stopped <pc> step-timeout`. |

Wire delta vs the v1 table: `S` now carries 1–2 candidate addresses; all other commands
are unchanged. Internal one-shot bps are tracked **separately from user `B+` bps**, so
stepping through an address that carries a user breakpoint never clears the user's bp
(MAME assigns distinct indices to duplicate `bpset`s at one address — verify during
implementation).

Client side, `sliomame.cpp` `SingleStep()`: port the legacy next-PC computation from
`sliosnes.cpp:375–571` (reads 4 bytes at PC via the read cache, `Disassem()` for length,
flag evaluation for Bxx, stack/vector reads for returns and indirect jumps). Where
prediction is ambiguous, send both next-sequential and predicted-target as the two
candidates.

Accepted edge case: an NMI/IRQ arriving mid-step runs the handler before a candidate is
reached, so the step completes after the handler returns — the same exposure the 1994
BRK-patch step had. The watchdog covers a handler that never returns.

Load-bearing spike facts this design rests on (verified 2026-06-11):

- `emu.register_periodic` **fires while `emu.pause()` is active** — the socket pump, and
  therefore `R`/`W`/`G`/`P`, work on a frozen machine. The whole inspect-while-halted
  story depends on this.
- `emu.pause`/`emu.unpause` are globals on `emu`; `manager.machine` exposes only the
  read-only `paused` property.
- Native bps pseudo-hold (PC pinned by re-trigger) under `-debugger none`; `bpclear`
  releases the hold — hence bp-auto-pause above.

### Genesis — gdbstub backend (`linux/sliogdb.cpp`, new)

genmon's transport speaks GDB RSP to `mame genesis -debug -debugger gdbstub`; the Lua
bridge is not loaded for GEN. New TU `linux/sliogdb.cpp` (~300 lines: packet
framing/checksum/escaping/acks plus the contract mapping), selected inside `which.cpp`'s
`MAMEBACKEND` arm by `#ifdef SNES` → `sliomame.cpp` / `#else` → `sliogdb.cpp`.

Contract mapping: `ReadSlaveData`→`m`, `WriteSlaveData`→`M`, regs→`g`/`G` (order fixed by
MAME's m68k register map — no `REGS` handshake in RSP), `SlaveSetBkPt`/`SlaveBkClr`→
`Z0`/`z0` (mapped to native MAME bps; work in ROM), `SingleStep`→`s` (native — the whole
point of this path), `SlaveRun`→`c`, `SlaveStop`→ break-in byte `0x03`,
`HandleSlaveInput`→ non-blocking read for the asynchronous `T05…` stop reply.

**GEN spike (blocks `sliogdb.cpp`; analogue of the SNES spike).** Raw Python RSP client
against apt MAME 0.277, `genesis` driver, `drmon-test.md`:

1. Does the apt build ship the gdbstub debugger module, and on which port
   (`-debugger_port` default — verify, don't assume)?
2. `g`-packet register order and widths for the m68k map → drives the client-side
   `regNameArray` mapping.
3. `Z0` in cartridge ROM → hit → `T05` stop reply.
4. `s` advances exactly one instruction (NOP-sled / `bra *` ROM).
5. Break-in `0x03` while running → stop reply.
6. Restart: is `R`/`vRun` supported? If not, `SlaveRestart()` is a documented no-op on GEN.
7. **Hybrid check (cheap; record the answer either way):** with `-debugger gdbstub`
   active, does the Lua periodic still pump the custom bridge, and does
   `db:command("step")` from a *real* held stop yield clean single-instruction steps? If
   yes, "gdbstub as hold-engine + custom bridge as protocol" becomes a future option that
   would restore one wire protocol for both systems. Not the Phase 2 path.

**Accepted trade-offs vs the custom bridge** (document in README + manual):

- **No live memory/register reads while the target runs.** RSP permits one outstanding
  request: after `c`, the only legal client transmission is `0x03` until a stop reply
  arrives. drmon's RUN-with-update mode degrades to run-no-update on GEN; windows refresh
  on stop. (SNES keeps live reads — the Lua API reads memory while the machine runs.)
  **Fallback if this proves unacceptable in use:** option (A) works for the 68000 too —
  `dis68000.cpp` has the same `Disassem()` machinery — at the cost of porting the
  prediction logic. Recorded here so the decision can be revisited with evidence, not
  re-litigated from scratch.
- **VDP/CRAM/VSRAM headroom is lost on this channel** — RSP has no secondary address
  spaces. Those accessors are stubbed to zeros in Phase 2 either way; revisit trigger is
  the first concrete VDP-window user need (options then: the hybrid above, or a
  side-channel Lua socket).
- `RESET` support unknown until spike item 6.

### Remaining work (supersedes original steps 5–10)

1. ~~**SNES bridge v2** — fold the redesign into `mame_bridge.lua`: `H`/`C`/`?` via
   pause/unpause/paused, bp-auto-pause, one-shot step bps + watchdog; delete the
   `execution_state` halt path.~~ **Done** — `mame_bridge.lua` v2 (2026-06-11).
2. ~~**`sliomame.cpp` step** — port the next-PC logic from `sliosnes.cpp:375–571` into
   `SingleStep()`; emit `S <next> [<alt>]`.~~ **Done** — `sliomame.cpp` `SingleStep()` rewritten (2026-06-11).
3. ~~**Harness fixes** (`test_bridge.sh` / `test-roms/`):
   kill stale listeners by port; fix GEN ROM glob; commit generators.~~ **Done** — `test_bridge.sh` v2: kill-by-port on entry/exit, explicit `drmon-test.md` match, GEN gdbstub launch mode; `generate_test_roms.py` added (2026-06-11).
4. ~~**`.gitignore` `roms/`**~~ **Done** — `.gitignore` updated (2026-06-11).
5. ~~**`test_bridge.py` v2** — new `S` form; bp-hit-while-running; step granularity assertion.~~ **Done** — `test_bridge.py` rewritten (2026-06-11).
6. ~~**GEN spike** (checklist above) → results recorded in this plan.~~ **Done** — results in section above; spike script at `linux/spikes/spike_gdbstub_gen.py` (2026-06-11).
7. ~~**`linux/sliogdb.cpp`** — RSP client per spike results; `which.cpp` arm split;
   `task mame SYS=gen` → gdbstub; `test_gdb.py`.~~ **Done** — `sliogdb.cpp` + `which.cpp` split + Taskfile + `test_gdb.py` (2026-06-11).
8. ~~**Vendor the spike scripts** — `spike_hold.lua`, `spike_step.lua`, `spike_gdbstub_gen.py`.~~ **Done** — under `linux/spikes/` (2026-06-11).
9. ~~**Docs, same turn as the code** — README, plan.~~ **Done** — `linux/README.md` updated for split architecture (2026-06-11).

### Files (delta vs the original table)

| File | Action |
|---|---|
| `devsys/tools/drmon/linux/mame_bridge.lua` | v2 redesign (pause-hold, one-shot step bps, watchdog) |
| `devsys/tools/drmon/linux/sliomame.cpp` | `SingleStep()` candidates via ported next-PC logic |
| `devsys/tools/drmon/linux/sliogdb.cpp` | **new** — GEN RSP client |
| `devsys/tools/drmon/which.cpp` | `MAMEBACKEND` arm splits SNES/GENESIS |
| `devsys/tools/drmon/linux/test_bridge.{sh,py}` | port-kill, glob fix, new assertions; GEN→RSP |
| `devsys/tools/drmon/linux/test-roms/` | committed generators (GEN + minimal SNES) |
| `devsys/tools/drmon/linux/spikes/` | **new** — vendored spike scripts |
| `.gitignore` | ignore `roms/` |
| `Taskfile.yml` | `task mame SYS=gen` → gdbstub flags |

## Verification (updated for the split architecture)

1. ~~Spike evidence recorded for **both** drivers: socket served while stopped, `:maincpu`
   state-name dumps (5a22 + m68000), bp hit observed via Lua.~~ **Done** — see
   [Spike results](#spike-results-step-2--mame-0277-apt-2026-06-11); hold/step spike
   scripts to be vendored under `linux/spikes/` (remaining-work step 8).
2. `task build` — both `snesmon` and `genmon` compile + link with `MAMEBACKEND`
   (0 errors); `file` reports x86-64 ELF for each.
3. `task test-bridge SYS=snes` — **all** assertions PASS, including the two new ones:
   bp-hit-while-running (`B+` → `C` → `?` reports `stopped <bp-addr> bp`) and step
   granularity (NOP sled: PC advances exactly one instruction per `S`). Raw output pasted.
4. Orphan immunity: `kill -9` MAME mid-run of step 3, rerun `task test-bridge SYS=snes` —
   second run passes cleanly (proves kill-by-port works; this was the 14/15 failure mode).
5. GEN spike results recorded in this plan (register order, port, Z0-in-ROM, `s`
   granularity, break-in, restart, hybrid check).
6. `task test-bridge SYS=gen` — RSP-backend assertions PASS (raw output pasted).
7. Live end-to-end, snesmon: `task mame SYS=snes CART=…` + `task run` — register window
   shows live values; set breakpoint, F2 Run → status `STOPPED`, PC = bp address; F7
   advances PC by exactly one instruction; memory window edit of `$7E0000` visible via
   `test_bridge.py` read-back. tmux screenshot captured (existing `shot` pattern).
8. Live end-to-end, genmon over gdbstub: same flow against `genesis` (`D0–D7/A0–A7/SR/PC`
   on stop, bp, native step, memory edit at `$FF0000`); confirm RUN degrades to no-update
   (documented trade-off) and stop refreshes all windows. tmux screenshot captured.
9. No TUI regressions: `task smoke` and `task signals` still PASS (both binaries).
10. Disconnected behavior preserved: `task run` with no MAME running — TUI responsive,
    status shows dead/disconnected target, no crash (both binaries; genmon without a
    gdbstub listener).
11. Fresh-clone reproducibility: in a clean worktree, `task test-bridge SYS=snes` and
    `SYS=gen` pass with **no user-supplied ROM** (committed generators produce the test
    ROMs).
