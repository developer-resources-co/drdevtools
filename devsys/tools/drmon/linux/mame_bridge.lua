-- mame_bridge.lua: drmon SNES bridge — the shared CPU core + SNES device commands.
-- Listens on 127.0.0.1:41816 (override env DRMON_MAME_ADDR=host:port).
-- Load via: mame <snes-driver> -cart <rom> -debug -debugger none -autoboot_script mame_bridge.lua
--
-- The CPU half (registers/memory/run/stop/step/breakpoints/write-protect) lives in
-- mame_cpu_bridge.lua, shared with the Genesis bridge.  This file adds only the SNES
-- device reads: PPU VRAM (RP), SPC700 APU RAM (RA) and SPC700 registers (GA/PA).

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

M.default_port = "41816"
-- Write-protect region: the LoROM bank-0 ROM window $8000-$FFFF where code lives —
-- stray writes there halt the CPU.  Coarse by design (a single CPU-space range).
M.wp_addr, M.wp_len = 0x8000, 0x8000

-- ── SNES device handles (resolved on each machine reset) ──────────────────────
local ppu_vram = nil   -- emu.item for snes_ppu m_vram (RP command)
local apu_cpu  = nil   -- :soundcpu (SPC700 s_smp) device — GA/PA commands
local apu_ram  = nil   -- :soundcpu program space (64K APU RAM) — RA command

M.add_init(function()
    -- SNES PPU VRAM via the save-state item interface (not a space/share/region):
    -- emu.item(:read(i)) yields each of the 65536 VRAM bytes, non-intrusively.
    ppu_vram = nil
    pcall(function()
        local ppu = manager.machine.devices[":ppu"]
        if ppu and ppu.items and ppu.items["0/m_vram"] then
            ppu_vram = emu.item(ppu.items["0/m_vram"])
        end
    end)
    -- SNES SPC700 audio co-CPU: registers via .state, 64K APU RAM via program space.
    apu_cpu = nil; apu_ram = nil
    pcall(function()
        apu_cpu = manager.machine.devices[":soundcpu"]
        if apu_cpu then apu_ram = apu_cpu.spaces["program"] end
    end)
end)

-- SPC700 register order: drmon PC A X Y SP PSW → :soundcpu state PC A X Y S P.
local SPC_STATE = {"PC","A","X","Y","S","P"}

M.add_command(function(line)
    -- RP addr len — PPU VRAM read (MTYPE_PPU); addr is a 0..0xffff VRAM offset.
    local rp_addr, rp_len = line:match("^RP (%x+) (%x+)$")
    if rp_addr then
        local addr = tonumber(rp_addr, 16)
        local len  = math.min(tonumber(rp_len, 16), 4096)
        if not ppu_vram then return string.rep("00", len) end
        local out = {}
        for i = 0, len-1 do
            local ok, b = pcall(function() return ppu_vram:read(addr + i) end)
            out[i+1] = (ok and b) and string.format("%02x", b & 0xff) or "00"
        end
        return table.concat(out)
    end

    -- RA addr len — SPC700 APU RAM read (MTYPE_SPC); :soundcpu program space.
    local ra_addr, ra_len = line:match("^RA (%x+) (%x+)$")
    if ra_addr then
        local addr = tonumber(ra_addr, 16)
        local len  = math.min(tonumber(ra_len, 16), 4096)
        if not apu_ram then return string.rep("00", len) end
        local out = {}
        for i = 0, len-1 do
            local ok, b = pcall(function() return apu_ram:read_u8(addr + i) end)
            out[i+1] = ok and string.format("%02x", b & 0xff) or "00"
        end
        return table.concat(out)
    end

    -- GA — get SPC700 registers (fixed order, space-separated hex)
    if line == "GA" then
        local vals = {}
        for _, n in ipairs(SPC_STATE) do
            local entry = apu_cpu and apu_cpu.state[n]
            vals[#vals+1] = entry and string.format("%x", entry.value) or "0"
        end
        return table.concat(vals, " ")
    end

    -- PA pc a x y sp psw — put SPC700 registers (fixed order)
    local pa_data = line:match("^PA (.+)$")
    if pa_data then
        local i = 1
        for val_str in pa_data:gmatch("%S+") do
            local n = SPC_STATE[i]
            local entry = n and apu_cpu and apu_cpu.state[n]
            if entry then pcall(function() entry.value = tonumber(val_str, 16) or 0 end) end
            i = i + 1
        end
        return "ok"
    end

    return nil   -- not an SNES device command
end)

M.start()
