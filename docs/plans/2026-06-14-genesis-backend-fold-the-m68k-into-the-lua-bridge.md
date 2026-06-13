# Genesis backend: fold the M68K into the Lua bridge (drop gdbstub)

## Context

genmon (Genesis) currently debugs the M68K over **MAME gdbstub (`:23946`, GDB RSP, `linux/sliogdb.cpp`)** and reads VDP/CRAM/VSRAM/Z80 over a **separate companion Lua bridge (`:41817`, `mame_genesis_bridge.lua`)**. Live step-4 verification (this session) proved a hard architectural limit: **MAME's Lua periodic pump only runs while the M68K executes**, so the VDP/Z80 bridge can answer *only while the game is running* — never at a breakpoint, which is exactly when you want to inspect graphics state. Confirmed: connected genmon shows live VDP only while `Running`; halted → no VDP. The two-channel split (gdbstub + bridge) is fundamentally wrong for this.

**Goal:** replace it with a **single `-debugger none` Lua bridge** that serves M68K CPU control (registers, memory, run/stop, single-step, breakpoints, write-protect) **and** VDP/CRAM/VSRAM/Z80 — exactly the proven SNES architecture (`mame_bridge.lua` + `sliomame.cpp`). Because the bridge runs the machine itself (via `emu.pause`/one-shot breakpoints, not gdbstub), the pump is alive in both run and halt states, so non-CPU state is readable at a breakpoint. Drop gdbstub for Genesis entirely.

**Decisions (user-confirmed):** unify the bridge into `mame_bridge.lua` (additive, device-gated; re-verify SNES 19/19); implement full M68K single-step now; unify the client into `sliomame.cpp`.

## Why this is well-templated (not from scratch)

- `linux/mame_bridge.lua` is already CPU-agnostic — it **already has an `m68000` register-alias table** (lines 25-31: D0-D7, A0-A6, A7→SP, USP, SR, PC) and reads `cpu.state[...]`/`cpu.spaces["program"]` generically. The whole CPU command set (`V`/`REGS`/`G`/`P`/`R`/`W`/`H`/`C`/`?`/`S`/`B+`/`B-`/`WP±`/`BW±`) works for any CPU.
- `linux/sliomame.cpp` is already mostly platform-agnostic and **already has `#ifdef GENESIS` branches** — big-endian `MemRead` (line ~440) and zero-stub `ReadSlaveVDP/CRAM/VSRAM/Z80` (lines 775-783). It uses `regNameArray`/`NUMREGS=19` from `reg.hpp` (D0-D7=0-7, A0-A7=8-15, USP=16, SR=17, PC=18; `REG_INSTRUCTIONPOINTER=REG_PC`).
- The VDP/Z80 device-read Lua already exists in `mame_genesis_bridge.lua` (`RV`/`RC`/`RS`/`RZ`, emu.item probes).

So the work is: graft VDP/Z80 onto the unified bridge, switch the client to the bridge, and write the **M68K next-PC decoder** (the one genuinely new piece).

## Design

### 1. MAME side — `linux/mame_bridge.lua` becomes the single bridge for both platforms
Additive, device-gated (SNES paths byte-for-byte unchanged):
- **Device init** (the nil-checked probe block ~lines 80-95): add VDP probes (`:vdp` → `0/m_vram`,`0/m_cram`,`0/m_vsram` via `emu.item`) and Z80 program space (`:z80` or `:soundcpu`), copied from `mame_genesis_bridge.lua:init_machine`. All `pcall`/nil-guarded → nil on SNES, populated on Genesis. PPU/SPC700 probes stay (nil on Genesis).
- **Commands**: add `RV addr len`, `RC`, `RS`, `RZ addr len` (from `mame_genesis_bridge.lua` dispatch + `item_bytes_hex`/`item_u16_hex`/`space_bytes_hex` helpers). Existing `RP`/`RA`/`GA`/`PA` stay nil-safe (return `00…`/`err` on Genesis).
- **WP/BW region** (currently hardcoded `$8000-$FFFF`, line 44): make CPU-dependent — for `m68000`, watch Genesis work-RAM `$FF0000-$FFFFFF` (where mutable code/vars live); SNES range unchanged.
- Version string is already generic (`ok drmon-bridge 1 <shortname>`; `shortname="m68000"` on Genesis).
- **Port**: unify on `DRMON_MAME_ADDR` (bridge default `41816`). The Genesis launch sets `DRMON_MAME_ADDR=127.0.0.1:41817` to preserve the existing genesis port and avoid any SNES collision. Retire `DRMON_GEN_BRIDGE_ADDR`.
- **Retire `mame_genesis_bridge.lua`** (its logic now lives in `mame_bridge.lua`).

### 2. genmon client — unify into `linux/sliomame.cpp`
- `which.cpp:18`: compile `sliomame.cpp` for Genesis (replace the `sliogdb.cpp` include). Compile-time `#ifdef`-gated → **zero SNES risk**.
- Under `#ifdef GENESIS` in `sliomame.cpp`:
  - **VDP/CRAM/VSRAM/Z80 reads**: replace the zero-stubs with `RV`/`RC`/`RS`/`RZ` bridge calls — port `hex_decode` + the chunked-read loops from `sliogdb.cpp:752-809`.
  - **M68K single-step next-PC decode** (replaces the 65816 opcode switch at `sliomame.cpp:550-614`): use `Disassem()` (`dis68000.cpp:1474`) for instruction length; decode M68K control-flow to compute `<next>`/`<alt>` one-shot-bp targets:
    - `Bcc`/`BRA`/`BSR` (`0x6xxx`, 8/16/32-bit disp): `BRA/BSR` → `next=target`; `Bcc/DBcc` → `next=PC+len`, `alt=target`.
    - `JMP`/`JSR` (`0x4EC0`/`0x4E80` + EA): absolute/indirect → `next=target` (read EA via big-endian mem).
    - `RTS`/`RTE`/`RTR` (`0x4E75`/`0x4E73`/`0x4E77`): `next` = return addr popped from A7 (big-endian).
    - default: `next=PC+len`.
    - Falls back safely to `PC+len`; the bridge's 500 ms step watchdog catches any miss.
  - Big-endian `MemRead`/register handshake (`REGS D0,…,PC`) already work via existing `#ifdef GENESIS` + `regNameArray`.
- **Retire `sliogdb.cpp`** from the build (out of `which.cpp`; keep the file for reference). The gdbstub-path commits this session (`789a4ee`, `99ca973`) become moot — note in the plan, don't revert history. `e3756ba` (the bridge listener fix) stays relevant — that VDP/Z80 logic moves into `mame_bridge.lua`.

### 3. Taskfile + tests
- `task mame SYS=gen`: `-debugger none -autoboot_script …/mame_bridge.lua` with `DRMON_MAME_ADDR=127.0.0.1:41817` (drop `-debugger gdbstub -debugger_port`).
- `task test-bridge SYS=gen`: run an extended protocol test (CPU + VDP/Z80) against the unified bridge under `-debugger none` (currently runs `test_gdb.py`/gdbstub — switch it).
- `verify_genesis_bridge.sh`: point at `mame_bridge.lua`; the readiness/teardown/`ss` logic stays.
- `test_genesis_bridge.py`: extend to cover the CPU commands (`V`/`REGS`/`G`/`R`/`H`/`C`/`S`/`B+`) alongside the existing `RV`/`RC`/`RS`/`RZ`.

## Files to change
- `linux/mame_bridge.lua` — add VDP/Z80 device init + `RV/RC/RS/RZ`; CPU-dependent WP region.
- `linux/sliomame.cpp` — `#ifdef GENESIS`: VDP/Z80 reads via bridge; M68K `SingleStep` next-PC decoder.
- `which.cpp` — compile `sliomame.cpp` for Genesis.
- `Taskfile.yml` — `mame`/`test-bridge` SYS=gen → `-debugger none` + unified bridge.
- `linux/verify_genesis_bridge.sh`, `linux/test_genesis_bridge.py` — point at unified bridge; cover CPU commands.
- Retire: `linux/mame_genesis_bridge.lua` (folded in), `linux/sliogdb.cpp` (out of `which.cpp`).
- Docs: persist this plan to `docs/plans/2026-06-14-genesis-fold-m68k-into-lua-bridge.md`; update the genesis non-CPU plan + `TODO.md` item.

## Verification
1. **Build**: snesmon + genmon compile/link clean (genmon now via `sliomame.cpp`); `task build`.
2. **SNES regression**: `task test-bridge SYS=snes` → **19/19 PASS** (bridge edits were additive).
3. **Genesis bridge protocol** (desktop, `-debugger none`): extended `test_genesis_bridge.py` covers `V/REGS/G/R/H/C/S/B+` + `RV/RC/RS/RZ` → all PASS, via `task verify-genesis-bridge`.
4. **Genesis live** (desktop): `task mame SYS=gen` + `task run SYS=gen` → genmon connects; F2 runs; set a breakpoint, hit it, halt; **read VDP/CRAM/VSRAM/Z80 while *halted*** → non-zero (resolves the step-4 failure); registers D0-D7/A0-A7/SR/PC correct and editable; single-step advances PC correctly through a sequential run, a taken/untaken branch, a JSR, and an RTS; memory read/write works.
5. Confirm the original step-4 defect is gone (VDP visible at a breakpoint, not just while running).

## Implementation (2026-06-14) — built, awaiting desktop verification

Two refinements to the plan, both user-decided during implementation:
- **Shared module, not a merge into `mame_bridge.lua`.** Extracted the CPU core into a new
  **`linux/mame_cpu_bridge.lua`** (returns a module `M` with `add_init`/`add_command`/`start`
  + config); `mame_bridge.lua` (SNES → PPU/SPC700) and `mame_genesis_bridge.lua` (Genesis →
  VDP/Z80) are now thin wrappers that `dofile` it (dir via `DRMON_BRIDGE_DIR`, set by the
  launch tasks; `debug.getinfo` fallback). One CPU core, two focused device files.
- **M68K single-step = native, not a software decoder.** The docs show native step needs
  gdbstub's `wait_for_debugger` hold loop, gone under `-debugger none` — but the 68000 has a
  real TRACE bit the 65816 lacks, untested under `-debugger none`. So instead of writing the
  next-PC decoder, the core gained a **`T`** command (`emu.unpause` + `db:command("step")`,
  reusing the deferred-step machinery); genmon's `SingleStep` (`#ifdef GENESIS`) just sends
  `T`. **If the 68000 native step does not hold, the watchdog fires `step-timeout` and PC runs
  on — that's the signal to fall back to the software next-PC decoder.**

Done + verified by me (build-time): `task build` clean (snesmon + genmon); `luac5.4 -p` clean
on all three Lua files; `bash -n` / `py_compile` clean on the scripts; `task smoke` snes + gen
PASS. Unified on `DRMON_MAME_ADDR` :41816 (gdbstub/:23946 retired). `sliogdb.cpp` left in-tree
but out of the build.

**Desktop verification (run these — MAME's Lua won't run under the agent shell):**
1. `task test-bridge SYS=snes` → SNES regression guard (the core extraction is mechanical).
   **PASS — 2026-06-14, 27/27** (desktop): 5a22 regs, R/W, PPU `RP`, SPC700 `GA/RA/PA`,
   `WP±/BW±`, `B±`, `S` step (PC advanced), `C` + bp fire, RESET — no regression from the
   core extraction.
2. `task verify-genesis-bridge` (Aladdin) → V/REGS/G/R/H/C + RV/RC/RS/RZ all PASS.
   **PASS — 2026-06-14, 15/15** (desktop). Proves the shared-module dofile (DRMON_BRIDGE_DIR),
   the unified `ok drmon-bridge` handshake, the M68K CPU commands (REGS / `G → 19 regs` / R /
   H / C), and VDP/CRAM/VSRAM/Z80 (RV/RC/RS/RZ) all on one :41816 socket, + BYE-reconnect.
3. `task mame SYS=gen CART="roms/genesis/Aladdin (USA).md"` + `task run SYS=gen` → connect, F2,
   set a breakpoint, hit + halt, then **read VDP/CRAM/VSRAM/Z80 windows while halted** (the
   whole point — should be non-zero). Then **single-step (F1)**: PC should advance one
   instruction. If the status flips to `step-timeout` / PC jumps far → native step doesn't hold
   → ping me and I'll write the software next-PC decoder.

## Risks / notes
- **M68K next-PC decode** is the one hard piece (variable length, many addressing modes). Mitigation: cover common control-flow opcodes, fall back to `PC+len`, rely on the 500 ms step watchdog.
- **Modifying the proven SNES bridge** — strictly additive + the 19/19 re-verify is the guard.
- **MAME Lua can't run under the agent shell** (signal 16 / exit 144) — all bridge/live verification is on the user's desktop (as in steps 3-4); the C++ client is build-verified by me.
- Plan persisted to `docs/plans/` on implementation per the project's plan-first convention.
