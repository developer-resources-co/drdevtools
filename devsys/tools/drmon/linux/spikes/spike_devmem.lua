-- spike_devmem.lua — Step 0 feasibility spike for lifting the SNES out-of-scope stubs.
-- Enumerates how MAME 0.277's Lua surface exposes the SNES PPU (VRAM/CGRAM/OAM),
-- the SPC700 sub-CPU, ROM-region bounds, and whether wpset works under -debugger none.
-- Throwaway: prints "SPIKE>"-prefixed lines to stdout, then exits.
--
-- Run headless:
--   mame snes -cart test-roms/drmon-test.sfc -debug -debugger none \
--        -autoboot_script spikes/spike_devmem.lua -video none -sound none \
--        -nothrottle -seconds_to_run 5

local done  = false
local ticks = 0

local function P(s) print("SPIKE> " .. s) end

local function try(label, fn)
    local ok, res = pcall(fn)
    if ok then return res else P(label .. " ERROR: " .. tostring(res)); return nil end
end

local function dump()
    local m = manager.machine

    -- 1) Devices — show every tag mentioning ppu/vram/cram/oam/spc/sound/apu/aram.
    P("=== DEVICES (filtered) ===")
    try("devices", function()
        for tag, dev in pairs(m.devices) do
            local t = tag:lower()
            if t:find("ppu") or t:find("vram") or t:find("cram") or t:find("oam")
               or t:find("spc") or t:find("sound") or t:find("apu") or t:find("aram")
               or tag == ":maincpu" then
                P(string.format("dev %-20s short=%s", tag, tostring(dev.shortname)))
            end
        end
    end)

    -- 2) Memory regions (tag + size) — candidate non-intrusive VRAM/CGRAM/OAM/ROM source.
    P("=== REGIONS ===")
    try("regions", function()
        for tag, r in pairs(m.memory.regions) do
            P(string.format("region %-24s size=%s", tag, tostring(r.size)))
        end
    end)

    -- 3) Memory shares (tag + size).
    P("=== SHARES ===")
    try("shares", function()
        for tag, s in pairs(m.memory.shares) do
            P(string.format("share  %-24s size=%s", tag, tostring(s.size)))
        end
    end)

    -- 4) PPU device — does it advertise address spaces (device_memory_interface)?
    P("=== PPU DEVICE ===")
    local ppu = try("ppu lookup", function() return m.devices[":ppu"] end)
    if ppu then
        P("ppu present short=" .. tostring(ppu.shortname))
        try("ppu.spaces", function()
            local any = false
            for n, sp in pairs(ppu.spaces) do
                any = true
                P(string.format("ppu space '%s' bytes=%s", n, tostring(sp.address_mask or "?")))
            end
            if not any then P("ppu has NO address spaces") end
        end)
    else
        P("no :ppu device")
    end

    -- 5) SPC700 sub-CPU (backlog item — confirm reachable, don't build).
    P("=== SOUNDCPU (SPC700) ===")
    local apu = try("soundcpu lookup", function() return m.devices[":soundcpu"] end)
    if apu then
        P("soundcpu present short=" .. tostring(apu.shortname))
        try("apu.spaces", function()
            for n, _ in pairs(apu.spaces) do P("soundcpu space: " .. n) end
        end)
        try("apu.state", function()
            local names = {}
            for k, _ in pairs(apu.state) do names[#names+1] = tostring(k) end
            P("soundcpu state: " .. table.concat(names, " "))
        end)
    else
        P("no :soundcpu device")
    end

    -- 6) maincpu program-space probe: confirm ROM ($00:8000 = NOP 0xEA) vs WRAM ($7E0000).
    P("=== MAINCPU PROBE ===")
    try("maincpu probe", function()
        local cpu = m.devices[":maincpu"]
        local sp  = cpu.spaces["program"]
        P(string.format("byte @ 0x008000 (ROM) = %02x", sp:read_u8(0x008000)))
        P(string.format("byte @ 0x00FFBC (ROM, JML) = %02x", sp:read_u8(0x00FFBC)))
        P(string.format("byte @ 0x7E0000 (WRAM) = %02x", sp:read_u8(0x7E0000)))
    end)

    -- 7) wpset under -debugger none: registers? does the watchpoint list reflect it?
    P("=== WPSET ===")
    local db = try("debugger", function() return m.debugger end)
    if db then
        try("wpset", function()
            db:command("wpset 7e0000,1,w")
        end)
        try("wplist", function()
            db:command("wplist")
        end)
        P("wpset/wplist issued (see console.log lines above/below for confirmation)")
        -- echo last few consolelog lines so we can see wplist output
        try("consolelog tail", function()
            local cl = db.consolelog
            local from = math.max(1, #cl - 8)
            for i = from, #cl do P("clog: " .. tostring(cl[i])) end
        end)
    else
        P("no debugger object (need -debug)")
    end

    P("=== DONE ===")
end

emu.register_periodic(function()
    if done then return end
    ticks = ticks + 1
    if ticks >= 30 then
        done = true
        local ok, err = pcall(dump)
        if not ok then P("FATAL " .. tostring(err)) end
        manager.machine:exit()
    end
end)
