-- verify_ppu.lua — integration check for the RP (PPU VRAM read) command.
-- Seeds snes_ppu m_vram[0..15] with a known pattern (0xA0..0xAF), then runs the
-- real mame_bridge.lua so a socket client can confirm `RP 0 10` returns the seed.
-- Throwaway test harness; not shipped.

local seeded = false
emu.register_periodic(function()
    if seeded then return end
    local ok = pcall(function()
        local ppu = manager.machine.devices[":ppu"]
        local v   = emu.item(ppu.items["0/m_vram"])
        for i = 0, 15 do v:write(i, (0xA0 + i) & 0xff) end
    end)
    if ok then seeded = true; print("SEED> VRAM[0..15] = A0..AF") end
end)

dofile("mame_bridge.lua")
