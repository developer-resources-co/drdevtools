# Lifting the "stubbed safe" out-of-scope features (SNES MAME backend)

## Context

The Phase 2 MAME backend (`05ac59c`) deliberately stubbed a set of features "safe"
so the legacy TUI/menu code could still call them without crashing. From the plan
(`docs/plans/2026-06-11-drmon-mame-backend.md:184-188`):

> Out of scope (stubbed safe, protocol reserved): SNES PPU/CRAM/OBJRAM reads
> (`ReadSlavePPU` → zeros) and SPC700 ops; Genesis VDP/CRAM/VSRAM (`ReadSlaveVDP`
> etc. → zeros) and Z80; write-protect / BRK-on-write (no-ops); `SNES_SEARCH`.
> ROM-region writes won't stick (MAME ROM) — work-RAM writes do.

This plan lifts the stubs **for SNES only**. The reason the split matters: SNES runs
on drmon's own ASCII Lua bridge (`mame_bridge.lua` ↔ `sliomame.cpp`), and MAME's Lua
API exposes every device + memory space — so new reads are additive bridge commands.
Genesis runs on MAME's native GDB RSP stub, which only exposes the M68K bus; VDP/CRAM/
VSRAM/Z80 are unreachable there and need a whole new transport — **deferred** (see TODO).

### Decisions (from clarifying round + code findings)

| Feature | Decision | Why |
|---|---|---|
| `ReadSlavePPU` (MTYPE_PPU window) | **Implement** | Real UI surface (`memory.cpp:1225-1228`); MAME Lua can reach PPU VRAM (pending spike) |
| Write-protect / BRK-on-write | **Implement** | Real UI surface (`monmenu.cpp:293,323`); maps to MAME `wpset` watchpoints |
| ROM-write warning | **Implement** | Cheap client-side guard; turns a silent no-op into a visible message |
| `SNES_SEARCH` | **Implement (client-side)** | Self-contained; needs no MAME change — but search + results window were never wired even in legacy (`search.cpp:206,261`) |
| **SPC700 ops** | **Defer → TODO backlog** | *No UI surface exists* — only protocol enums (`slaveio.hpp:26-30`), gated behind `#ifdef SPC700` (`which.cpp:41`). Building it = new UI + new wire path, not a quick win. |
| Genesis VDP/CRAM/VSRAM/Z80 | **Defer → TODO backlog** | GDB RSP can't reach them; needs a side-channel Lua companion socket (recommended future approach) |
| Make ROM writes "stick" | **Won't implement** | Genuine MAME constraint — ROM is read-only; faking it would mean debugging modified code |

**Deferred work gets TODO entries** (per the user's note — yes, deferring should be
tracked, not lost). See the TODO section at the end.

---

## Architecture recap (SNES path)

```
drmon (snesmon)                      MAME -autoboot_script mame_bridge.lua
  sliomame.cpp  ── TCP 41816 ASCII ──►  dispatch(line)  ── manager.machine ──► devices/spaces
  (client)         R/W/G/P/B/S/C/H/?       (server)
```

New work is purely additive: new ASCII commands in `mame_bridge.lua`'s `dispatch()`,
matching new client senders in `sliomame.cpp`, reusing the existing `mame_cmd()` /
`sock_send()` / read-cache / cache-invalidate plumbing.

---

## Step 0 — MAME 0.277 Lua feasibility spike (de-risks the PPU piece)

Before writing the PPU path, confirm how MAME 0.277's Lua surface exposes the SNES PPU's
internal memory. Run a throwaway `-autoboot_script` against the SNES driver and record,
in the plan's verification section, the answers to:

1. **PPU device tag** — enumerate `manager.machine.devices` for the `snes_ppu` tag
   (expected `:ppu`). Confirm it has a usable handle.
2. **VRAM/CGRAM/OAM access path** — test, in priority order:
   - `manager.machine.memory.shares[":...vram"]` / `.regions[...]` (named share/region) — preferred, non-intrusive.
   - `ppu.spaces[...]` if the PPU device advertises an address space.
   - Any device-level Lua accessor for `m_vram` / `m_cgram` / `m_oam`.
3. **SPC700 tag** (for the backlog item) — expected `:soundcpu`; confirm `.state` regs
   and `.spaces["program"]` (64 KB APU RAM) are present. Record but don't build yet.
4. **`wpset` behaviour** under `-debugger none` — confirm `db:command("wpset addr,len,w")`
   registers and that a write halts (mirrors the existing `bpset` consolelog-marker flow).

**Record raw output in this plan, PASS/FAIL per item.** If item 2 finds *no* non-intrusive
VRAM path, fall back to the guarded port-read approach in Step 1 and note the limitation.

### Step 0 results — MAME 0.277 apt, 2026-06-11 (spikes/spike_devmem.lua, spike_ppuitems.lua)

1. **PPU device tag** — **PASS**: `:ppu` (shortname `snes_ppu`). But it has **no address
   spaces** (`device_memory_interface` not implemented), and VRAM/CGRAM/OAM appear in
   **neither** `memory.regions` (only `:snsslot:cart:rom` 32K, `:soundcpu:sound_ipl` 64,
   `:maincpu` 16M) **nor** `memory.shares` (only `:aram` 64K, `:wram` 128K).
2. **VRAM/CGRAM/OAM access path** — **PASS via save-state items** (the spaces/shares/regions
   route FAILED, as above). `manager.machine.devices[":ppu"].items` exposes:
   ```
   0/m_vram    size=1 count=65536   → emu.item(id):read(off) returns each VRAM byte
   0/m_cgram   size=2 count=256     → 256×u16 palette words
   (OAM is decomposed into m_oam.* structured fields — no flat buffer)
   ```
   `emu.item(id):read(i)` returns element i (nil past end); reads are non-intrusive (no PPU
   port side effects). The MTYPE_PPU window masks `addr & 0xffff` → it is a **VRAM** dump,
   served directly by `0/m_vram`. **Decision: Step 1 uses the save-item path; no port fallback needed.**
3. **SPC700 (backlog)** — **PASS**: `:soundcpu` (shortname `s_smp`), spaces `program`+`data`,
   state regs `PC S P A X Y`, RAM is the `:aram` 64K share (also `0/:aram` save item). Trivial
   to wire when its UI exists.
4. **wpset under `-debugger none`** — **PASS**: `db:command("wpset 7e0000,1,w")` →
   consolelog `Watchpoint 1 set`; `wplist` → `1 @ 7E0000-7E0000 write`.
5. **ROM detection (Step 3)** — **PASS / not needed exactly**: read-back is map-agnostic —
   `$008000`=EA and `$00FFBC`=5C (ROM, unchanged on write) vs `$7E0000`=55 (WRAM). Write-then-
   readback distinguishes ROM from WRAM without computing mapper bounds.

---

## Step 1 — `ReadSlavePPU` (MTYPE_PPU memory window)

The MTYPE_PPU window flows `addr &= 0xffff` → `ReadSlavePPU(addr, buf, len)`
(`memory.cpp:1225-1228`). Currently `sliomame.cpp:657` memsets zeros.

**Bridge (`mame_bridge.lua`):** — confirmed approach = save-state item read (Step 0 #2).
- Resolve the VRAM save item once in `init_machine()` (`mame_bridge.lua:56-62`):
  `ppu_vram = emu.item(manager.machine.devices[":ppu"].items["0/m_vram"])`, cached next
  to `cpu`/`db`. (Re-resolve on reset, like the other handles.)
- Add a PPU read command — `RP <addr> <len>` — mirroring the existing `R` handler
  (`mame_bridge.lua:168-184`) but reading `ppu_vram:read(addr + i)` instead of
  `sp:read_u8(...)`. Same hex-concat reply, same 4096 clamp, same `pcall`-guards-to-`00`.

**Client (`sliomame.cpp`):**
- Replace the stub body (`sliomame.cpp:656-660`) to send `RP %lx %x`, parse the hex
  reply into `data`, and route through the existing read-cache + `mame_cmd()` helpers
  (same shape as `ReadSlaveData`). Errors/short replies → zero-fill (current safe default).

Non-intrusive: reads the actual `m_vram` array, no PPU port latch side effects — so no
guarded-port fallback is needed (the Step 0 spike ruled it out as unnecessary).

> Scope note: for the SNES MAME build, MTYPE_PPU is the *only* graphics window type
> (`memory.cpp` has no SNES CRAM/OBJRAM window — those are GENESIS-only MTYPE_VDPCO/VDPVS).
> So "PPU/CRAM/OBJRAM reads" collapses to this one entry point. CGRAM/OAM, if surfaced as
> distinct windows later, would be sibling `RC`/`RO` commands following the same pattern.

---

## Step 2 — Write-protect / BRK-on-write (MAME watchpoints)

UI: `monmenu.cpp:293/323` → `SlaveSetWriteProtect()` / `SlaveSetBRKOnWrite()` (void;
global toggles — legacy `DB_CTRL_WP` / `DB_CTRL_BRKONWRITE`, `sliosnes.cpp:323-347`).
Currently no-ops (`sliomame.cpp:649-652`).

**Map both to a MAME write watchpoint** (`wpset <addr>,<len>,w`), since MAME watchpoints
*break* on access — they do not silently *block* a write. Honest consequence to document:
"write-protect" and "break-on-write" both manifest as a **halt on write**; true
write-blocking is not a MAME primitive (and ROM is already read-only, so the useful case
is catching stray writes).

**Bridge:** new `WP+`/`WP-` and `BW+`/`BW-` commands that issue `db:command("wpset ...")` /
`wpclear`, reusing the consolelog-marker detection already proven for `bpset`
(`mame_bridge.lua:247-258` is the template — a watchpoint hit halts via the same
`exec_state`/marker path the periodic pump already watches).

**Client:** fill the four `sliomame.cpp:649-652` stubs to send the matching commands.
Region: the calls are parameterless, so default the watch to the cartridge ROM range
(queried from MAME's memory map in Step 0); document the range as coarse/global.

---

## Step 3 — ROM-write warning

`WriteSlaveData` (`sliomame.cpp`) currently sends `W` unconditionally; MAME drops writes
to ROM-mapped addresses silently (the documented "won't stick"). Make it visible:

- After a write whose target falls in a ROM region (bounds from Step 0's memory-map query),
  read back the first byte and, if unchanged, surface a one-line status warning
  ("write to ROM $XXXXXX ignored (MAME ROM is read-only)") via drmon's existing status/
  message path. Work-RAM writes (`$7E0000+`) are unaffected.
- No MAME change; purely client-side guard.

---

## Step 4 — `SNES_SEARCH` (client-side, no MAME change)

Even in legacy drmon the search was never wired: `MemSearchGUI` (`search.cpp:204-207`)
has `//SlaveCompMem(...)` commented out, and `OpenMemSearchWindow` is `#if 0`
(`search.cpp:261`). The input chain (`DoMemSearch` → start addr → len → value,
`search.cpp:209-228`) already collects the query.

- Implement `MemSearchGUI` to loop `ReadSlaveData` over the range (reusing the read cache)
  and scan client-side for the value; collect hit addresses.
- Revive a minimal results window (un-`#if 0` and adapt `OpenMemSearchWindow`) or, if that
  list-window scaffold is too entangled, dump hits to the status/console line as a first cut.
- Entirely within drmon; the bridge is untouched.

---

## Files

| File | Change |
|---|---|
| `devsys/tools/drmon/linux/mame_bridge.lua` | new `RP`, `WP+/-`, `BW+/-` commands; PPU/VRAM handle in `init_machine` |
| `devsys/tools/drmon/linux/sliomame.cpp` | fill `ReadSlavePPU` + 4 watchpoint stubs; ROM-write guard in `WriteSlaveData` |
| `devsys/tools/drmon/search.cpp` | wire `MemSearchGUI` + revive results window |
| `devsys/tools/drmon/linux/README.md`, drmon manual, `docs/plans/2026-06-11-drmon-mame-backend.md` | update the "out of scope" paragraph to reflect what's now implemented vs deferred |
| `TODO.md` | this plan's entry → `[wip]`; backlog entries for deferred SPC700 + Genesis non-CPU (Step 5) |

---

## Step 5 — TODO bookkeeping (deferred work is tracked, not lost)

Add to `TODO.md` (via the `/todo` skill):
1. **This work** — `[wip]` entry linked to this plan: "Lift SNES out-of-scope stubs
   (PPU window, watchpoints, ROM-write warning, mem-search)".
2. **Deferred — SPC700 SNES debugging** — backlog: needs a UI surface (CPU selector /
   SPC register + APU-RAM window) before the (already-easy) `:soundcpu` Lua wiring is worth
   building. Note the device access is confirmed trivial; the gate is UI.
3. **Deferred — Genesis non-CPU state (VDP/CRAM/VSRAM/Z80)** — backlog: GDB RSP can't reach
   it; recommended approach is a **side-channel Lua companion socket** alongside the gdbstub
   (keep gdbstub for M68K/step/breakpoints; small autoboot Lua opens a 2nd socket for device
   reads — `:vdp` + `:z80snd` device handles). Revisit when a concrete Genesis-graphics need appears.

---

## Verification

End-to-end, with MAME 0.277 from apt and a SNES test ROM (`task mame SYS=snes CART=…`
+ `snesmon`):

1. **Spike (Step 0)** — paste device-enum + VRAM-access + `wpset` raw output here; PASS/FAIL each.
2. **PPU window** — open an MTYPE_PPU memory window in `snesmon`; confirm it shows live VRAM
   (non-zero, changes as the ROM draws) rather than the all-zero stub. Cross-check a few bytes
   against MAME's built-in PPU/tilemap viewer.

   **Protocol-level evidence (PASS, 2026-06-11)** — reproducible via
   `python3 spikes/verify_features.py` (PPU section): `spikes/verify_ppu.lua` seeds
   `m_vram[0..15]=A0..AF`, runs the real bridge, and the spike reads it back:
   ```
   RP 0 10 -> a0a1a2a3a4a5a6a7a8a9aaabacadaeaf      (seed match)
   RP fffe 8 -> ...0000                             (offsets past 0xFFFF zero-fill)
   ```
   Confirms RP parsing, length, the resolved `ppu_vram` handle, and out-of-range guarding.
   Full TUI-window cross-check deferred to the connected-`snesmon` integration pass.
3. **Watchpoints** — enable "Break on Write", let the ROM write to a watched address, confirm
   `snesmon` halts and reports the stop (reason surfaced via the existing `?` poll path).

   **Protocol-level evidence (PASS, 2026-06-11)** — reproducible via
   `spikes/verify_features.py` (watchpoint section): the writer ROM
   (`drmon-test-writer.sfc`, `STA $8250` loop into the watched ROM window) + the real bridge:
   ```
   ?(pre)  -> running
   BW+     -> ok
   ?(post) -> stopped 8005 bp      (watchpoint fired on the write; CPU halted)
   BW-/C   -> ok / running         (cleared and resumed cleanly)
   ```
   Confirms `wpset addr,len,w,1,printf` registers, fires on a ROM-window write (MAME
   monitors writes even though ROM drops them), and the marker-scan halt path detects it.
4. **ROM-write warning** — attempt a `W` to a ROM address; confirm the warning fires and the
   byte is unchanged; confirm a `$7E0000+` work-RAM write still succeeds (no warning).

   **Protocol-level evidence (PASS, 2026-06-11)** — reproducible via
   `spikes/verify_features.py` (romwrite section):
   ```
   WRAM  W c3 -> R 7e0000 1 = c3   (stuck → no warning)
   ROM   R 8000 1 before=ea  W c3  after=ea  -> dropped (readback ≠ written → warns)
   ```
   Confirms the read-back-compare condition that drives `PrintWarning`.
   (All three deep checks run together: `python3 spikes/verify_features.py` → 8/8 pass.)
5. **Mem-search** — search a known byte pattern in work-RAM; confirm hit addresses match a
   manual `R`-dump scan.
6. **Regression** — `task test-bridge SYS=snes` still passes (existing R/W/G/P/B/S/C/H/?
   assertions unaffected by the additive commands).

   **Evidence (PASS, 2026-06-11)** — `test_bridge.py` (extended with RP/WP±/BW± assertions)
   against a clean bridge: **24 passed, 0 failed**, including `RP returns 32 hex chars`,
   `WP+/WP-/BW+/BW- return ok`. (NB: run after killing stale MAME — a windowed spike
   instance holding :41816 blocks the test's own bridge; harness already does `fuser -k`.)

5b. **Build** — `task build` produces both `snesmon` + `genmon` clean (search.cpp now
   compiled + wired into the MemOps menu; `mame_cmd` overflow fix; PPU/watchpoint/ROM-warn
   client paths). **PASS.**

5c. **Smoke** — `task smoke SYS=snes` PASS: "opened windows via Alt-keys (M-e M-k M-r M-m
   M-w M-b M-s M-i M-o M-a M-y) + typed into Expression — no SIGSEGV" (no crash from the
   rebuilt binary carrying the new MemOps menu entry).

### Not verified headlessly (manual TUI step)
- **PPU window in the TUI** and **interactive mem-search** (typing addr/len/value at the
  GetExpr prompts) need a connected `snesmon` in a terminal — the bridge-side paths are
  proven above, but the on-screen rendering / prompt flow is a manual check.

Cost: $0 — entirely local (host MAME from apt, drmon in the existing Docker toolchain).
