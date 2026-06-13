-- mame_genesis_bridge.lua: drmon Genesis bridge — the shared CPU core + Genesis devices.
-- Listens on 127.0.0.1:41817 (override env DRMON_MAME_ADDR=host:port).
-- Load via: mame genesis -cart <rom> -debug -debugger none -autoboot_script mame_genesis_bridge.lua
--
-- The CPU half (M68K registers/memory/run/stop/step/breakpoints/write-protect) lives in
-- mame_cpu_bridge.lua, shared with the SNES bridge.  This file adds only the Genesis
-- non-CPU device reads: VDP VRAM (RV), CRAM (RC), VSRAM (RS) and Z80 program space (RZ).
--
-- Replaces the old gdbstub(:23946)+companion split: one -debugger none channel serves
-- the M68K *and* VDP/Z80, so non-CPU state is readable at a breakpoint (the Lua pump runs
-- in both run and halt states, unlike the gdbstub-halted machine).

-- Locate the shared core next to this script.  Prefer DRMON_BRIDGE_DIR (set by the
-- launch tasks/scripts — robust regardless of how MAME names the autoboot chunk);
-- fall back to this script's own dir via debug.getinfo for hand-launches.
local here = os.getenv and os.getenv("DRMON_BRIDGE_DIR")
if here and #here > 0 then
    if here:sub(-1) ~= "/" then here = here .. "/" end
else
    here = (debug.getinfo(1, "S").source or ""):match("^@(.*/)") or "./"
end
local M = dofile(here .. "mame_cpu_bridge.lua")

-- Same default port as the SNES bridge (they never run at once); override via DRMON_MAME_ADDR.
M.default_port = "41816"
-- Write-protect region: cartridge ROM $000000-$3FFFFF — writes to ROM are bugs.
M.wp_addr, M.wp_len = 0x000000, 0x400000

-- ── Genesis device handles (resolved on each machine reset) ───────────────────
local vdp_vram  = nil   -- emu.item m_vram  (byte array, 64K) — RV
local vdp_cram  = nil   -- emu.item m_cram  (64 × u16)        — RC
local vdp_vsram = nil   -- emu.item m_vsram (40 × u16)        — RS
local z80_space = nil   -- :z80/:soundcpu program space (64K) — RZ

M.add_init(function()
    vdp_vram = nil; vdp_cram = nil; vdp_vsram = nil
    pcall(function()
        local vdp = manager.machine.devices[":vdp"]
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
end)

-- ── hex helpers ───────────────────────────────────────────────────────────────
-- len bytes from a byte-per-element emu.item (VDP VRAM) at byte offset addr.
local function item_bytes_hex(item, addr, len)
    if not item then return string.rep("00", len) end
    local out = {}
    for i = 0, len - 1 do
        local ok, v = pcall(function() return item:read(addr + i) end)
        out[i + 1] = string.format("%02x", (ok and v) and (v & 0xff) or 0)
    end
    return table.concat(out)
end

-- n u16 elements from an emu.item (CRAM/VSRAM), big-endian byte pairs.
local function item_u16_hex(item, n)
    if not item then return string.rep("00", n * 2) end
    local out = {}
    for i = 0, n - 1 do
        local ok, v = pcall(function() return item:read(i) end)
        local val = (ok and v) or 0
        out[2 * i + 1] = string.format("%02x", (val >> 8) & 0xff)
        out[2 * i + 2] = string.format("%02x", val & 0xff)
    end
    return table.concat(out)
end

-- len bytes from a MAME address space (Z80), addr masked to 16-bit.
local function space_bytes_hex(space, addr, len)
    if not space then return string.rep("00", len) end
    local out = {}
    for i = 0, len - 1 do
        local ok, b = pcall(function() return space:read_u8((addr + i) & 0xffff) end)
        out[i + 1] = string.format("%02x", (ok and b) and (b & 0xff) or 0)
    end
    return table.concat(out)
end

M.add_command(function(line)
    -- RV addr len — VDP VRAM read (byte item; addr/len hex, ≤4096/call)
    local rv_addr, rv_len = line:match("^RV (%x+) (%x+)$")
    if rv_addr then
        return item_bytes_hex(vdp_vram, tonumber(rv_addr, 16),
                              math.min(tonumber(rv_len, 16), 4096))
    end

    -- RC — CRAM read (always 64 × u16 = 128 bytes, big-endian pairs)
    if line == "RC" then return item_u16_hex(vdp_cram, 64) end

    -- RS — VSRAM read (always 40 × u16 = 80 bytes, big-endian pairs)
    if line == "RS" then return item_u16_hex(vdp_vsram, 40) end

    -- RZ addr len — Z80 program space read (≤4096/call)
    local rz_addr, rz_len = line:match("^RZ (%x+) (%x+)$")
    if rz_addr then
        return space_bytes_hex(z80_space, tonumber(rz_addr, 16),
                               math.min(tonumber(rz_len, 16), 4096))
    end

    return nil   -- not a Genesis device command
end)

M.start()
