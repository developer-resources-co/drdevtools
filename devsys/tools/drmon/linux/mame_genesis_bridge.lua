-- mame_genesis_bridge.lua: drmon companion bridge for Genesis non-CPU state.
-- Runs alongside -debugger gdbstub.  gdbstub handles M68K CPU/step/bps on
-- port 23946; this script exposes VDP VRAM/CRAM/VSRAM and Z80 program space
-- on port 41817 (override: DRMON_GEN_BRIDGE_ADDR=host:port env var).
--
-- Load via:
--   mame genesis -cart rom.bin -debug -debugger gdbstub \
--     -autoboot_script mame_genesis_bridge.lua
--
-- Protocol: newline-terminated ASCII, strict request→response, one client.
-- Commands:
--   V            → ok drmon-genesis-bridge 1 <cpu-shortname>
--   RV addr len  → 2×len hex chars (VDP VRAM; addr/len in hex)
--   RC           → 256 hex chars   (CRAM; always 64×u16 = 128 bytes)
--   RS           → 160 hex chars   (VSRAM; always 40×u16 = 80 bytes)
--   RZ addr len  → 2×len hex chars (Z80 program space; addr/len in hex)
--   BYE          → (disconnect + reopen listener)

local DEFAULT_PORT = "41817"

-- EOF detection: after this many consecutive nil reads, treat peer as gone.
local EOF_TICKS = 300

-- ── global state ──────────────────────────────────────────────────────────────
local cpu       = nil
local shortname = nil
local vdp       = nil
local vdp_vram  = nil   -- emu.item for m_vram (byte array, 64K)
local vdp_cram  = nil   -- emu.item for m_cram (u16 array, 64 entries)
local vdp_vsram = nil   -- emu.item for m_vsram (u16 array, 40 entries)
local z80_space = nil   -- :z80 program address space
local srv       = nil
local rx_buf    = ""
local ever_connected  = false
local nil_read_ticks  = 0

-- ── machine init ──────────────────────────────────────────────────────────────
local function init_machine()
    if not manager or not manager.machine then return end
    -- Guard the maincpu probe: at autoboot-load time device indexing can throw, and an
    -- unguarded throw here would abort the whole script before open_server() runs (the
    -- periodic then bails on `not srv`, so the listener would never open).
    pcall(function()
        cpu = manager.machine.devices[":maincpu"]
        shortname = cpu and cpu.shortname or "m68000"
    end)
    if not shortname then shortname = "m68000" end

    -- VDP VRAM (byte array, 64K) via save-state item.
    vdp = nil; vdp_vram = nil; vdp_cram = nil; vdp_vsram = nil
    pcall(function()
        vdp = manager.machine.devices[":vdp"]
        if vdp and vdp.items then
            if vdp.items["0/m_vram"]  then vdp_vram  = emu.item(vdp.items["0/m_vram"])  end
            if vdp.items["0/m_cram"]  then vdp_cram  = emu.item(vdp.items["0/m_cram"])  end
            if vdp.items["0/m_vsram"] then vdp_vsram = emu.item(vdp.items["0/m_vsram"]) end
        end
    end)

    -- Z80 sound CPU program space.
    z80_space = nil
    pcall(function()
        local z = manager.machine.devices[":z80"] or manager.machine.devices[":soundcpu"]
        if z then z80_space = z.spaces["program"] end
    end)
end

emu.add_machine_reset_notifier(function() init_machine() end)

-- ── socket helpers ────────────────────────────────────────────────────────────
local function open_server()
    local port = DEFAULT_PORT
    local env = os.getenv and os.getenv("DRMON_GEN_BRIDGE_ADDR")
    if env then
        local p = env:match(":(%d+)$")
        if p then port = p end
    end
    srv = nil
    collectgarbage()
    srv = emu.file("", 7)
    srv:open("socket.127.0.0.1:" .. port)
    rx_buf = ""
    ever_connected = false
    nil_read_ticks = 0
end

local function sock_send(s)
    if srv then srv:write(s .. "\n") end
end

local function readline()
    if not srv then return nil end
    local chunk = srv:read(4096)
    if chunk and #chunk > 0 then
        rx_buf = rx_buf .. chunk
        nil_read_ticks = 0
    else
        if ever_connected then nil_read_ticks = nil_read_ticks + 1 end
        return nil
    end
    local nl = rx_buf:find("\n")
    if not nl then return nil end
    local line = rx_buf:sub(1, nl - 1):gsub("\r", "")
    rx_buf = rx_buf:sub(nl + 1)
    return line
end

-- ── hex helpers ───────────────────────────────────────────────────────────────

-- Read len bytes from a byte-per-element emu.item starting at byte offset addr.
local function item_bytes_hex(item, addr, len)
    if not item then return string.rep("00", len) end
    local out = {}
    for i = 0, len - 1 do
        local ok, v = pcall(function() return item:read(addr + i) end)
        out[i + 1] = string.format("%02x", ok and (v & 0xff) or 0)
    end
    return table.concat(out)
end

-- Read n u16 elements from an emu.item, output as big-endian byte pairs.
local function item_u16_hex(item, n)
    if not item then return string.rep("00", n * 2) end
    local out = {}
    for i = 0, n - 1 do
        local ok, v = pcall(function() return item:read(i) end)
        local val = ok and (v or 0) or 0
        out[2 * i + 1] = string.format("%02x", (val >> 8) & 0xff)
        out[2 * i + 2] = string.format("%02x", val & 0xff)
    end
    return table.concat(out)
end

-- Read len bytes from a MAME address space starting at addr.
local function space_bytes_hex(space, addr, len)
    if not space then return string.rep("00", len) end
    local out = {}
    for i = 0, len - 1 do
        local ok, b = pcall(function() return space:read_u8((addr + i) & 0xffff) end)
        out[i + 1] = string.format("%02x", ok and (b & 0xff) or 0)
    end
    return table.concat(out)
end

-- ── command dispatch ──────────────────────────────────────────────────────────
local function dispatch(line)
    ever_connected = true
    nil_read_ticks = 0

    -- V — version handshake
    if line == "V" then
        sock_send("ok drmon-genesis-bridge 1 " .. (shortname or "m68000"))
        return
    end

    -- RV addr len — VDP VRAM read (byte item, up to 4096 bytes per call)
    local rv_addr, rv_len = line:match("^RV (%x+) (%x+)$")
    if rv_addr then
        local addr = tonumber(rv_addr, 16)
        local len  = math.min(tonumber(rv_len, 16), 4096)
        sock_send(item_bytes_hex(vdp_vram, addr, len))
        return
    end

    -- RC — CRAM read (64 × u16 = 128 bytes, always full, big-endian pairs)
    if line == "RC" then
        sock_send(item_u16_hex(vdp_cram, 64))
        return
    end

    -- RS — VSRAM read (40 × u16 = 80 bytes, always full, big-endian pairs)
    if line == "RS" then
        sock_send(item_u16_hex(vdp_vsram, 40))
        return
    end

    -- RZ addr len — Z80 program space read (up to 4096 bytes per call)
    local rz_addr, rz_len = line:match("^RZ (%x+) (%x+)$")
    if rz_addr then
        local addr = tonumber(rz_addr, 16)
        local len  = math.min(tonumber(rz_len, 16), 4096)
        sock_send(space_bytes_hex(z80_space, addr, len))
        return
    end

    -- BYE — graceful disconnect; reopen listener
    if line == "BYE" then
        open_server()
        return
    end

    sock_send("err unknown command")
end

-- ── periodic pump ─────────────────────────────────────────────────────────────
emu.register_periodic(function()
    if not srv then return end

    -- Lazily init machine state
    if not cpu then init_machine() end

    -- EOF detection: too many empty reads → reopen
    if ever_connected and nil_read_ticks >= EOF_TICKS then
        open_server()
        return
    end

    local line = readline()
    if line and #line > 0 then
        dispatch(line)
    end
end)

-- ── startup ───────────────────────────────────────────────────────────────────
-- Open the listener FIRST: a throw while probing devices must never leave the bridge
-- without a socket.  (The SNES bridge sidesteps this by deferring device init to the
-- reset notifier + lazy periodic init; we open-then-probe for the same guarantee.)
open_server()
init_machine()
