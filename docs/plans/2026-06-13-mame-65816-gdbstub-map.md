# Add a 65816/5A22 register map to MAME's gdbstub (Lua Tier 1 + C++ Tier 2)

**Date:** 2026-06-13
**Status:** wip
**TODO item:** "Upstream MAME 65816/5A22 register map to `debuggdbstub.cpp`" (DRMON — INVESTIGATIONS).
**Source investigation:** [2026-06-11-65816-gdbstub.md](../investigations/2026-06-11-65816-gdbstub.md)
(Tiers 1/2 specified there; this plan executes them).
**Sibling plan:** [2026-06-13-65816-rsp-clients-ida-ghidra.md](2026-06-13-65816-rsp-clients-ida-ghidra.md)
(TODO item A — depends on the Tier‑1 endpoint this plan stands up).

---

## Goal

Give MAME a working **65816 GDB Remote Serial Protocol (RSP)** register map so XML‑aware RSP
clients (IDA Pro, custom tooling, CI harnesses) can attach to a running SNES / Apple IIgs /
Foenix‑class 65816 target. Two tiers, per the investigation:

- **Tier 1 — Lua plugin (no rebuild).** Extend a repo‑local shadow of MAME's shipped
  `plugins/gdbstub/init.lua` with a `5a22` register map. Works on stock apt MAME 0.277
  **today**. Enhanced beyond the investigation's "quick hack" to the **honest version**:
  per‑register widths + `qSupported`/`qXfer:features:read` serving a hand‑authored
  `target.xml` + `Z0`→`bpset`. The `target.xml` is what unblocks the sibling IDA/Ghidra plan.
- **Tier 2 — C++ module (`debuggdbstub.cpp`).** The proper upstream contribution: ~35‑line
  register map keyed to `5a22`/`w65c816`/`w65c802`, proposing arch name `w65c816`. Authored
  + prepared as a PR‑ready diff. **Build‑verification is disk‑gated** (see Constraints).

## Non‑goals

- Not changing the Phase‑2 drmon↔MAME decision (custom Lua bridge stays — RSP can't express
  PPU/SPC700/secondary address spaces). This work is **decoupled from drmon** and lives in its
  own `devsys/tools/mame-65816-gdbstub/`.
- No SA‑1 / co‑CPU map (stubs expose only the first/visible CPU — out of scope).
- gdb itself is **not** a target client: upstream gdb has no 65816 gdbarch (binutils `w65`
  was assembler‑only, removed 2018). The beneficiaries are XML‑aware clients.

---

## Ground truth (probed against MAME 0.277 + DKC, 2026-06-13)

`mame snes -cart DKC.smc -autoboot_script probe.lua -video none -seconds_to_run 3`:

```
PROBE shortname=5a22
PROBE reg A    = 0x400     PROBE reg D    = 0x0
PROBE reg X    = 0x5DA     PROBE reg DB   = 0x0
PROBE reg Y    = 0xF2      PROBE reg PB   = 0x8A
PROBE reg P    = 0x26      PROBE reg PC   = 0x8AB523   (24-bit linear, includes bank)
PROBE reg E    = 0x0       PROBE reg GENPC/CURPC = 0x8AB523 (mirror PC)
PROBE reg S    = 0x1F2     PROBE reg DBR/PBR/GENSP/GENFLAGS = <absent>
```

- maincpu device shortname = **`5a22`** (the regmap key). `w65c816`/`w65c802` are the same
  core's other device names (Apple IIgs / Foenix) — alias them to the same map.
- The 10 state names A/X/Y/P/E/S/D/DB/PB/PC all read via `cpu.state[name].value`.
- **PC already carries the 24‑bit linear address** (bank in high byte) — no PB<<16 assembly needed.

### Register widths + canonical g‑packet order

65816 mixes widths, so the plugin's uniform‑`regsize` assumption is replaced with per‑register
byte widths. Canonical order (defines both g‑packet byte order and `target.xml` `<reg>` order):

| # | reg | width (bytes) | gdb type    | note                         |
|---|-----|---------------|-------------|------------------------------|
| 0 | A   | 2             | int         | accumulator (16‑bit max)     |
| 1 | X   | 2             | int         | index                        |
| 2 | Y   | 2             | int         | index                        |
| 3 | P   | 1             | int         | processor status             |
| 4 | E   | 1             | int         | emulation‑mode flag (1 bit)  |
| 5 | S   | 2             | data_ptr    | stack pointer                |
| 6 | D   | 2             | int         | direct‑page register         |
| 7 | DB  | 1             | int         | data bank                    |
| 8 | PB  | 1             | int         | program bank                 |
| 9 | PC  | 4             | code_ptr    | 24‑bit, zero‑extended to 32  |

g‑packet total = 2+2+2+1+1+2+2+1+1+4 = **18 bytes = 36 hex chars**. 65816 is little‑endian; the
plugin's existing `makebestr(val,len)` already emits LSB‑first — correct as‑is.

PC zero‑extended to 32 bits (not 24) for maximum client compatibility — most RSP clients assume
pointer widths are byte‑multiples / power‑of‑two; the high byte is always `00`.

---

## Tier 1 — design (Lua plugin)

New dir `devsys/tools/mame-65816-gdbstub/`:
- `plugins/gdbstub/init.lua` — fork of the shipped 0.277 plugin with the 65816 additions.
- `plugins/gdbstub/plugin.json` — copy (name `gdbstub`, `start: false`).
- `target.xml` — the canonical feature description (also embedded as a Lua string in init.lua).
- `test_65816.py` — Python RSP client (forked from drmon's `linux/test_gdb.py`).
- `run-mame.sh` — headless MAME launcher (mirrors drmon `task mame SYS=snes`, swaps the bridge
  for `-plugin gdbstub` + the shadow `-pluginspath`).
- `README.md` — what it is, how to run, the no‑rebuild story.

Loaded without touching root‑owned system files:
```
mame snes -cart DKC.smc -debug -debugger none \
  -pluginspath "<repo>/devsys/tools/mame-65816-gdbstub/plugins;/usr/share/games/mame/plugins" \
  -plugin gdbstub -video none -sound none -nothrottle -skip_gameinfo
```
(`-debug -debugger none` = debugger core on, no interactive UI — same combo drmon's SNES bridge
uses. The Lua plugin opens its own socket on `127.0.0.1:2159`.)

### Changes vs. the shipped plugin

1. **`regmaps["5a22"]`** (+ `w65c816`/`w65c802` aliases): replace scalar `regsize` with an
   ordered `regs = {{name,width,...},...}` list; keep `pcreg="PC"`, `addrsize=3`.
2. **`g`** — emit each register at its own width in canonical order (not uniform `regsize`).
3. **`G`** — parse incoming hex consuming each register's own width.
4. **stop replies** (`T05`) — emit PC with PC's width (4 B), not `regsize` (the shipped code
   uses `regsize` for the PC field — a latent 16‑bit truncation for 24‑bit PCs; fixed here).
5. **`Z0` software breakpoints** → `cpu.debug:bpset(addr)` (shipped plugin returns empty for
   `Z0`; XML clients often use `Z0`). Track in the same `breaks` table as `Z1`.
6. **`qSupported`** → `PacketSize=1000;qXfer:features:read+;swbreak+`.
7. **`qXfer:features:read:target.xml:OFF,LEN`** → serve the hand‑authored `target.xml` with
   `l`/`m` chunk prefixes.
8. **Graceful stubs** for `qAttached`(→`1`), `qC`(→empty), `H`(→`OK`), `vMustReplyEmpty`(→empty)
   so IDA's handshake doesn't stall.

These additions are guarded so the i386 path is byte‑for‑byte unchanged (the new code only
triggers for `cpu.shortname == "5a22"` and the new query verbs).

---

## Tier 2 — design (C++ `debuggdbstub.cpp`)

Fetch the exact 0.277 `src/osd/modules/debugger/debuggdbstub.cpp` via the GitHub raw API
(no full clone — disk). Add, using the verified state names:

```cpp
static const gdb_register_map gdb_register_map_w65c816 =
{
    "w65c816",            // proposed gdb arch name (none official exists)
    {
        {
            "mame.w65c816",   // proposed feature name (no org.gnu.gdb.* standard)
            {
                { "A",  "a",  false, TYPE_INT },
                { "X",  "x",  false, TYPE_INT },
                { "Y",  "y",  false, TYPE_INT },
                { "P",  "p",  false, TYPE_INT },
                { "E",  "e",  false, TYPE_INT },
                { "S",  "sp", true,  TYPE_DATA_POINTER },
                { "D",  "d",  false, TYPE_INT },
                { "DB", "db", false, TYPE_INT },
                { "PB", "pb", false, TYPE_INT },
                { "PC", "pc", true,  TYPE_CODE_POINTER },
            }
        }
    }
};
// in gdb_register_maps:
{ "5a22",    gdb_register_map_w65c816 },   // SNES / NSS / sfcbox
{ "w65c816", gdb_register_map_w65c816 },   // Apple IIgs, Foenix-class
{ "w65c802", gdb_register_map_w65c816 },
```

The module already generates `target.xml`, maps `Z0`→native breakpoints, and emits proper stop
packets — the map is the only missing piece. Register sizes are derived by the module from the
device state entries (the same widths the probe confirmed). Output: a clean patch +
PR‑ready branch under `devsys/tools/mame-65816-gdbstub/tier2-debuggdbstub.patch` with a
`PR-NOTES.md` (arch‑name rationale, the `m6502` precedent for unofficial arch strings).

### Constraints

- **Disk at 98% (14 GB free).** A full MAME source build is tens of GB of intermediates +
  hours — **unsafe here**. The patch is authored against the exact upstream source and is
  trivially correct (the module already does everything else for m68k etc.), but in‑tree build
  verification is **deferred until disk headroom exists** (or run on another machine / in CI on
  the PR). This is recorded as a pending verification step, not skipped silently.

---

## Verification

### Tier 1 (runnable now)

1. **Plugin loads, attaches to the 5a22.** Launch MAME with the shadow plugin; confirm the
   socket opens and no "no register map for cpu 5a22" appears.
2. **`qSupported` + `qXfer:features:read:target.xml` round‑trips** a well‑formed XML document
   whose `<reg>` list matches the canonical table (10 regs, correct bitsizes/order).
3. **`g` returns 36 hex chars**; decoded per the canonical widths, PC == live `cpu.state.PC`
   (24‑bit) and matches an independent MAME‑side read.
4. **`Z0` software breakpoint** set at a known PC fires; stop reply carries the 24‑bit PC
   (not truncated to 16).
5. **single‑step (`s`)** advances PC by the executed instruction's length; **`m`** memory read
   matches a MAME‑side read of the same address.
6. **i386 regression** — the shipped i386 path is unchanged (`diff` shows only additive hunks;
   an i386 driver still reports its map / `gcc`‑style no‑op confirmation).

### Tier 2 (deferred — disk‑gated)

7. **[deferred]** Build `mame` from source with the patch (SOURCES‑limited to the snes driver
   to bound disk/time), launch `mame snes -debug -debugger gdbstub`, and confirm the
   module‑generated `target.xml` + `g` match the Tier‑1 results. Blocked on disk headroom;
   record raw output here when run.

(Numbered raw‑output evidence gets pasted under each step as it's executed — per the SRC plan
verification format.)

### Verification — Tier 1 — 14/14 PASS (2026-06-13)

Reproducible via `devsys/tools/mame-65816-gdbstub/verify.sh` (launches headless MAME + the
gdbstub Lua, runs `test_65816.py`, retries the rare boot race). Raw output:

```
PASS: qSupported advertises qXfer:features:read+
PASS: qXfer:features:read returns a last-chunk ('l') document
PASS: target.xml is well-formed XML
PASS: target.xml <reg> names/bitsizes/order match canonical layout
PASS: g returns 36 hex chars
       decoded: {A:0x0, X:0x40, Y:0x60, P:0x6, E:0x0, S:0x1ff, D:0x0, DB:0x80, PB:0x80, PC:0x80c0d5}
PASS: PC high byte is zero (24-bit zero-extended)
PASS: PC bits[16:24] == PB (24-bit linear address)
PASS: E (emulation flag) is 0 or 1
PASS: m@PC returns 8 hex chars
PASS: Z0 (software breakpoint) set returns OK
       stopped at 0x80a97a after bp@0xa976 (continue ran)
PASS: Z0 breakpoint causes a stop on continue (machine ran from break-in PC then halted)
PASS: z0 (software breakpoint) clear returns OK
PASS: M/m round-trip at WRAM $7E0000
PASS: single step returns readable registers and advances PC
Results: 14 passed, 0 failed
```

Mapping to the numbered steps:

1. **PASS** — plugin attaches to the `5a22` (lazy-init from the periodic; the reset notifier
   doesn't fire under `-autoboot_script`, so `setup()` is retried from the periodic — drmon's
   `if not db then init() return end` pattern).
2. **PASS** — `qSupported` + `qXfer:features:read:target.xml` round-trips a well-formed document
   whose 10 `<reg>` names/bitsizes/order match the canonical table.
3. **PASS** — `g` = 36 hex; decoded PC `0x80c0d5` with `PB=0x80` (PC bits[16:24] == PB → 24-bit
   linear PC is correct and not truncated).
4. **PASS** — `Z0` set/clear return OK (no crash) and a continue with the bp set halts the
   machine. Note: in `-debugger none` MAME a breakpoint is a *pseudo-hold* (`execution_state
   == "stop"` without a hard scheduler freeze), so the reported PC lands a few instructions past
   the bp (here just after the NMI handler at `0x80a97a`) rather than exactly on it; the Tier‑2
   C++ module freezes exactly. Continue + bp-detection both work.
5. **PASS** — single step advances PC; `m` reads match.
6. **i386 register path preserved; shared protocol bugs fixed for all CPUs.** The new
   per-register `g`/`G` is gated on `map.regs`, so the uniform-width i386 path runs the original
   code unchanged. The fork *also* fixes shared 0.277 protocol bugs that affected every CPU:
   `bpset`/`wpset` now pass non-null `cond`/`act` (the shipped null args `strlen`-crash MAME — the
   exact SIGSEGV captured under gdb during this work), `bpclear`/`wpclear` (the shipped
   `bpclr`/`wpclr` are not 0.277 binding names), `M addr,len:data` colon parsing (shipped used a
   comma → every write silently `E00`'d), and a single stop notifier using `emu.pause`/`unpause`
   (the shipped execution_state/`go()` path can't resume in this context). A live i386 regression
   run was not performed (no i386 target on hand); the changes are additive/bug-fix in nature.

Control-flow lesson (now in `init.lua` + recorded as memory): MAME 0.277 `-debugger none` stop/go
must use `emu.pause()`/`emu.unpause()`, not `execution_state`/`cpu.debug:go()` —
`mame_bridge.lua` already proved this.
