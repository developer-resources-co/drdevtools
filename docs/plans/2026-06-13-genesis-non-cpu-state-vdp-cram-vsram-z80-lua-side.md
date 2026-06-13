# Plan: Genesis non-CPU state — VDP/CRAM/VSRAM/Z80 Lua side-channel

## Context

`genmon` connects to MAME via GDB RSP (`-debugger gdbstub`, port 23946). GDB RSP exposes
only the M68K CPU bus — VDP VRAM/CRAM/VSRAM and Z80 are architecturally unreachable.
The three VDP read functions (`ReadSlaveVDP`, `ReadSlaveCRAM`, `ReadSlaveVSRAM`) are
intentionally zero-stubbed in `linux/sliogdb.cpp:631-638`. The Z80 has no read function
and no memory-window type at all.

The fix: a companion Lua script loaded via `-autoboot_script` opens a second TCP socket
(port 41817) serving VDP/Z80 reads. `sliogdb.cpp` gains a second fd alongside GDB RSP.
The SNES Lua bridge (`linux/mame_bridge.lua` + `linux/sliomame.cpp`) is the direct pattern.

## Files to create

### `linux/mame_genesis_bridge.lua` (~200 lines)

Stripped-down companion to `mame_bridge.lua` — no step logic, no breakpoints, no
register management. Only device reads.

**Port:** 41817 default, override via `DRMON_GEN_BRIDGE_ADDR=host:port` env var.

**Machine init** (called on reset via `emu.add_machine_reset_notifier`):
```lua
vdp     = manager.machine.devices[":vdp"]
vdp_vram  = nil; vdp_cram = nil; vdp_vsram = nil
pcall(function()
    if vdp and vdp.items then
        if vdp.items["0/m_vram"]  then vdp_vram  = emu.item(vdp.items["0/m_vram"])  end
        if vdp.items["0/m_cram"]  then vdp_cram  = emu.item(vdp.items["0/m_cram"])  end
        if vdp.items["0/m_vsram"] then vdp_vsram = emu.item(vdp.items["0/m_vsram"]) end
    end
end)
z80_space = nil
pcall(function()
    local z = manager.machine.devices[":z80"] or manager.machine.devices[":soundcpu"]
    if z then z80_space = z.spaces["program"] end
end)
```

**Socket pump** (`emu.register_periodic`): same one-command-per-tick dispatch as
`mame_bridge.lua`.

**Commands:**

| Command | Request | Response | Notes |
|---------|---------|----------|-------|
| `V` | `V` | `ok drmon-genesis-bridge 1 m68000` | version handshake |
| `RV addr len` | `RV 0 400` | 2×len hex chars | VDP VRAM; addr/len in hex; chunked |
| `RC` | `RC` | 256 hex chars | CRAM; always 128 bytes (64×u16) |
| `RS` | `RS` | 160 hex chars | VSRAM; always 80 bytes (40×u16) |
| `RZ addr len` | `RZ 0 100` | 2×len hex chars | Z80 program space; addr/len hex |
| `BYE` | `BYE` | _(none)_ | disconnect + reopen listener |

**Read helpers** — `item_to_hex(item, byte_offset, count)` reads `emu.item` elements
(accounting for `item:size()` = 1 or 2) and formats as hex pairs. For 16-bit items
(CRAM, VSRAM) output high byte first (big-endian, matching Genesis VDP word layout).
For byte items (VRAM) use `item:read(i)` directly. Z80 reads use
`z80_space:read_u8(addr+i)`.

**EOF reconnect:** same nil-read-ticks pattern as `mame_bridge.lua` (300 ticks ≈ 5 s).

### `linux/test_genesis_bridge.py` (~80 lines)

Protocol test following `test_bridge.py` pattern. Tests: V handshake, RV (16-byte
sample), RC (128 bytes, asserts length), RS (80 bytes), RZ (16-byte sample), BYE
reconnect. Requires live MAME with the bridge loaded.

## Files to modify

### `linux/sliogdb.cpp`

**Add companion bridge state** (after line 73 — existing GDB state block):
```cpp
static int   g_br_fd              = -1;
static char  g_br_host[64]        = "127.0.0.1";
static int   g_br_port            = 41817;
static struct timespec g_br_last_connect = {0, 0};
```

**Parse env** — `parse_gdb_env()` already reads `DRMON_GDB_ADDR`; add parallel
`parse_br_env()` reading `DRMON_GEN_BRIDGE_ADDR`.

**Add `br_close()`, `br_cmd()`, `try_br_connect()`, `maybe_reconnect_bridge()`** —
exact copies of `gdb_close()`, `mame_cmd()` (from `sliomame.cpp:110`), `try_connect()`
and `maybe_reconnect()` from `sliomame.cpp`, but operating on `g_br_fd` / `g_br_host` /
`g_br_port` / `g_br_last_connect`. `maybe_reconnect_bridge()` handshakes with `V` and
checks `strncmp(reply, "ok drmon-genesis-bridge", 23)`.

**Call `maybe_reconnect_bridge()`** from `HandleSlaveInput()` (line ~410) alongside
the existing GDB reconnect logic.

**Replace the stubs (lines 631–638):**
```cpp
void ReadSlaveVDP(unsigned long addr, char *data, unsigned int len) {
    if (g_br_fd < 0) { memset(data, 0, len); return; }
    // chunked RV commands (same loop as ReadSlavePPU in sliomame.cpp:690)
    // command format: "RV %lx %x"
}
void ReadSlaveCRAM(char *data) {
    if (g_br_fd < 0) { memset(data, 0, 128); return; }
    char reply[256 + 4];
    if (br_cmd("RC", reply, sizeof(reply)) < 0) { memset(data, 0, 128); return; }
    // hex-decode reply into 128 bytes
}
void ReadSlaveVSRAM(char *data) {
    if (g_br_fd < 0) { memset(data, 0, 80); return; }
    char reply[160 + 4];
    if (br_cmd("RS", reply, sizeof(reply)) < 0) { memset(data, 0, 80); return; }
    // hex-decode reply into 80 bytes
}
void ReadSlaveZ80(unsigned long addr, char *data, unsigned int len) {
    if (g_br_fd < 0) { memset(data, 0, len); return; }
    // chunked RZ commands (same loop as RV)
}
```
Writes (`WriteSlaveVDP`, `WriteSlaveCRAM`, `WriteSlaveVSRAM`) remain no-ops — VDP
write via debugger is unusual; MAME Lua write API for VDP internals is not exposed.

**Hex-decode helper** — factor out the pair-decode loop from `ReadSlavePPU` (already
in sliomame.cpp) as a file-static `hex_decode(const char *src, char *dst, unsigned int n)`
to avoid repeating it four times.

### `slaveio.hpp` (after line 130, inside `#ifdef GENESIS`)

```cpp
void ReadSlaveZ80(unsigned long addr, char *data, unsigned int len);
```

### `memory.cpp`

**Enum** (after `MTYPE_VDPVS`, line 87, still inside `#ifdef GENESIS`):
```cpp
MTYPE_Z80,
```

**`mTypeText[]`** (after `"VDP VScroll\xC4"`, line 110):
```cpp
"Z80\xC4",
```

**Size/width tables** — wherever `MTYPE_VDPVS` gets a size/width entry, add a matching
entry for `MTYPE_Z80` (size=1, width=3, same as `MTYPE_BYTE`).

**All existing `case MTYPE_VDP: case MTYPE_VDPCO: case MTYPE_VDPVS:` groups** (lines
997–999, 1021–1027, 1221–1223, 1319) — add `case MTYPE_Z80:` to each group so Z80
windows behave the same as VDP windows for display-mode selection, address limits, etc.

**Dispatch** (line 1260, after `MTYPE_VDPVS` case):
```cpp
case MTYPE_Z80:
    addr &= 0xffff;
    ReadSlaveZ80(addr, xferBuffer, bytesNeeded);
    break;
```

### `monmenu.cpp`

In the memory-type submenu (inside `#ifdef GENESIS`, after the VDP VScroll entry):
```cpp
{"Z80 RAM", 0, NULL, MemoryTypeZ80, 0},
```
with a small handler that sets `oPtr->mType = MTYPE_Z80` and re-renders (identical to
the existing VDP handlers at lines 555/559/563).

### `sliogen.cpp` (DOS devel-board path)

Add stub after existing VDP stubs:
```cpp
void ReadSlaveZ80(unsigned long addr, char *data, unsigned int len) {
    // TODO: wire GEN_READ_Z80 (cmd 0x0F) if devel-board support is wanted
    memset(data, 0, len);
}
```

### `slioemul.cpp` (emulation path, compiled for Genesis)

Add matching stub:
```cpp
void ReadSlaveZ80(unsigned long addr, char *data, unsigned int len) {
    memset(data, 0, len);
}
```

### `linux/sliomame.cpp` (SNES path — has dead `#ifdef GENESIS` stubs at line 774)

Add `ReadSlaveZ80` stub inside the existing `#ifdef GENESIS` block:
```cpp
void ReadSlaveZ80(unsigned long, char *data, unsigned int len) { memset(data, 0, len); }
```

## MAME launch

```
mame genesis -cart myrom.bin \
  -debug -debugger gdbstub \
  -autoboot_script devsys/tools/drmon/linux/mame_genesis_bridge.lua
```

`-debugger gdbstub` and `-autoboot_script` coexist — gdbstub handles M68K CPU/step/bps
on port 23946; the companion Lua script handles VDP/Z80 on port 41817.

## Verification

1. **Build clean** — `task build` (SYSTEM=GEN); any compile error means a missing stub or
   signature mismatch.

2. **Smoke (disconnected)** — `task smoke` or `task run` with no MAME; genmon TUI opens,
   VDP/Z80 windows show zeros (bridge not connected), no crash.

3. **Bridge protocol** (live MAME):
   ```
   mame genesis -cart rom.bin -debug -debugger gdbstub \
     -autoboot_script devsys/tools/drmon/linux/mame_genesis_bridge.lua
   python3 devsys/tools/drmon/linux/test_genesis_bridge.py
   ```
   All test commands should pass: V, RV (non-zero when game running), RC (128 bytes),
   RS (80 bytes), RZ (non-zero), BYE + reconnect.

4. **UI** (live MAME + genmon connected):
   - Memory window → Type → VDP: shows non-zero VRAM tiles when game is running.
   - Memory window → Type → VDP Color: 128 bytes of palette data visible.
   - Memory window → Type → VDP VScroll: 80 bytes of vertical scroll data.
   - Memory window → Type → Z80 RAM: Z80 program space visible (sound driver code/RAM).

5. **Build SNES** — `task build SYSTEM=SNES`; new `ReadSlaveZ80` stub in sliomame.cpp
   must not break the SNES build.
