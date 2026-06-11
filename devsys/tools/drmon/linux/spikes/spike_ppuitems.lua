-- spike_ppuitems.lua — Step 0 follow-up: can we read SNES PPU VRAM/CGRAM/OAM via
-- MAME's save-state item interface?  (They are not exposed as spaces/shares/regions.)
-- Probes device.items on :ppu and the emu.item read API.  Throwaway; prints "SPIKE>".

local done, ticks = false, 0
local function P(s) print("SPIKE> " .. s) end
local function try(label, fn)
    local ok, res = pcall(fn)
    if ok then return res else P(label .. " ERROR: " .. tostring(res)); return nil end
end

local function dump()
    local m = manager.machine
    local ppu = m.devices[":ppu"]

    P("=== PPU SAVE ITEMS ===")
    try("ppu.items", function()
        local n = 0
        for name, idx in pairs(ppu.items) do
            P(string.format("item %-28s id=%s", name, tostring(idx)))
            n = n + 1
        end
        P("total items = " .. n)
    end)

    -- Try to read VRAM/CGRAM/OAM through emu.item, matching by substring.
    P("=== EMU.ITEM READ TEST ===")
    try("read items", function()
        for name, idx in pairs(ppu.items) do
            local ln = name:lower()
            if ln:find("vram") or ln:find("cgram") or ln:find("oam") then
                local it = emu.item(idx)
                local sz    = try("size",  function() return it.size end)
                local cnt   = try("count", function() return it.count end)
                local b0    = try("read0", function() return it:read(0) end)
                local b1    = try("read1", function() return it:read(1) end)
                P(string.format("READ %-28s size=%s count=%s [0]=%s [1]=%s",
                    name, tostring(sz), tostring(cnt), tostring(b0), tostring(b1)))
            end
        end
    end)

    -- Also dump soundcpu items (aram is a share already, but confirm spc regs/ram path).
    P("=== SOUNDCPU ITEMS (filtered) ===")
    try("soundcpu.items", function()
        local apu = m.devices[":soundcpu"]
        for name, idx in pairs(apu.items) do
            if name:lower():find("ram") then P("scpu item " .. name) end
        end
    end)

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
