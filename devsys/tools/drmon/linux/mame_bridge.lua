-- mame_bridge.lua: drmon Phase 2 MAME TCP bridge (v2)
-- Listens on 127.0.0.1:41816 (override env DRMON_MAME_ADDR=host:port).
-- Protocol: newline-terminated ASCII, strict request→response, one client.
-- Load via: mame <snes-driver> -cart <rom> -debug -debugger none -autoboot_script mame_bridge.lua
--
-- SNES-only.  Genesis uses -debugger gdbstub (sliogdb.cpp on the client side).
--
-- v2 hold/step design (post-spike):
--   H  → emu.pause()   (true CPU freeze, not execution_state="stop")
--   C  → emu.unpause()
--   ?  → manager.machine.paused → stopped; exec=="stop" while unpaused → bp-auto-pause
--   S <next> [<alt>] → one-shot bp(s), unpause, deferred "stopped <pc> step" reply
--                      watchdog fires after ~500 ms if no bp hit

local DEFAULT_PORT = "41816"

-- per-CPU alias: drmon register name → MAME cpu.state entry name
local REG_ALIASES = {
    ["5a22"] = {
        A="A", X="X", Y="Y", FLAGS="P", EMUL="E", D="D",
        DB="DB", PB="PB", SP="S", PC="PC",
        PCL="GENPC",   -- 24-bit composite (PB<<16|PC)
    },
    -- m68000 alias table kept for hand-debugging with nc; genmon uses gdbstub, not this bridge.
    ["m68000"] = {
        D0="D0", D1="D1", D2="D2", D3="D3",
        D4="D4", D5="D5", D6="D6", D7="D7",
        A0="A0", A1="A1", A2="A2", A3="A3",
        A4="A4", A5="A5", A6="A6",
        A7="SP", USP="USP", SR="SR", PC="PC",
    }
}

-- Names requiring special decomposition on write (SNES PCL → PC + PB)
local PCL_DECOMPOSES = { ["5a22"]=true }

-- Step watchdog: ~500 ms.  Each periodic tick is ~16 ms → 31 ticks.
local STEP_WATCHDOG_TICKS = 31

-- ── global state ──────────────────────────────────────────────────────────────
local cpu         = nil
local db          = nil
local shortname   = nil
local reg_order   = {}          -- drmon names in announced order (from REGS cmd)
local bp_map      = {}          -- addr(int) → bp_index(int)  — user breakpoints
local step_bps    = {}          -- addr(int) → bp_index(int)  — one-shot step bps (separate!)
local clog_pos    = 0           -- last consumed consolelog index
local pending_reply = nil       -- "step" when waiting for a step bp to fire
local step_tick_count = 0       -- watchdog counter
local halt_reason = nil         -- last stop reason: "halt"|"bp"|"step"|"step-timeout"|nil
local srv         = nil         -- emu.file socket
local rx_buf      = ""          -- partial line accumulator

-- ── machine init ──────────────────────────────────────────────────────────────
local function init_machine()
    if not manager or not manager.machine then return end
    cpu = manager.machine.devices[":maincpu"]
    db  = manager.machine.debugger
    if db then clog_pos = #db.consolelog end
    shortname = cpu and cpu.shortname or nil
end

emu.add_machine_reset_notifier(function() init_machine() end)

-- ── socket helpers ────────────────────────────────────────────────────────────
local function open_server()
    local port = DEFAULT_PORT
    local env_addr = os.getenv and os.getenv("DRMON_MAME_ADDR")
    if env_addr then
        local p = env_addr:match(":(%d+)$")
        if p then port = p end
    end
    -- Nil out the old handle first so Lua GC closes the fd before we rebind.
    srv = nil
    collectgarbage()
    srv = emu.file("", 7)
    srv:open("socket.127.0.0.1:" .. port)
    rx_buf = ""
    pending_reply = nil
    step_tick_count = 0
    halt_reason = nil
end

local function sock_send(s)
    if srv then srv:write(s .. "\n") end
end

local function readline()
    if not srv then return nil end
    local chunk = srv:read(4096)
    if chunk and #chunk > 0 then rx_buf = rx_buf .. chunk end
    local line, rest = rx_buf:match("^([^\n]*)\n(.*)")
    if not line then return nil end
    rx_buf = rest
    return line
end

-- ── debugger helpers ──────────────────────────────────────────────────────────
local function get_genpc()
    if not cpu then return 0 end
    local s = cpu.state["GENPC"] or cpu.state["PC"]
    return s and s.value or 0
end

local function is_paused()
    local ok, p = pcall(function() return manager.machine.paused end)
    return ok and p
end

local function exec_state()
    if not db then return "run" end
    local ok, st = pcall(function() return db.execution_state end)
    return ok and st or "run"
end

-- Scan consolelog from clog_pos for a pattern; returns first match or nil.
local function scan_clog(pat)
    if not db then return nil end
    local cl = db.consolelog
    for i = clog_pos + 1, #cl do
        clog_pos = i
        local m = tostring(cl[i]):match(pat)
        if m then return m end
    end
    return nil
end

-- Set a bp (user or step) and return its MAME index; nil on failure.
local function bpset(addr)
    pcall(function() clog_pos = #db.consolelog end)
    db:command(string.format("bpset %x", addr))
    local idx = scan_clog("Breakpoint (%d+) set")
    return idx and tonumber(idx) or nil
end

-- Clear step one-shot bps.
local function clear_step_bps()
    for addr, idx in pairs(step_bps) do
        pcall(function() db:command("bpclear " .. idx) end)
    end
    step_bps = {}
end

-- ── command dispatch ──────────────────────────────────────────────────────────
local function dispatch(line)
    if not cpu or not db then
        sock_send("err no target")
        return
    end

    -- V — version handshake
    if line == "V" then
        sock_send("ok drmon-bridge 1 " .. (shortname or "unknown"))
        return
    end

    -- REGS name,name,... — announce register order
    local regs_arg = line:match("^REGS (.+)$")
    if regs_arg then
        reg_order = {}
        for n in regs_arg:gmatch("[^,]+") do reg_order[#reg_order+1] = n end
        sock_send("ok")
        return
    end

    -- R addr len — memory read
    local r_addr, r_len = line:match("^R (%x+) (%x+)$")
    if r_addr then
        local addr = tonumber(r_addr, 16)
        local len  = math.min(tonumber(r_len, 16), 4096)
        local sp = cpu.spaces["program"]
        if sp then
            local out = {}
            for i = 0, len-1 do
                local ok, b = pcall(function() return sp:read_u8(addr + i) end)
                out[i+1] = ok and string.format("%02x", b) or "00"
            end
            sock_send(table.concat(out))
        else
            sock_send(string.rep("00", len))
        end
        return
    end

    -- W addr hexbytes — memory write
    local w_addr, w_hex = line:match("^W (%x+) (%x+)$")
    if w_addr then
        local addr = tonumber(w_addr, 16)
        local sp = cpu.spaces["program"]
        if sp then
            local i = 0
            w_hex:gsub("%x%x", function(s)
                pcall(function() sp:write_u8(addr + i, tonumber(s, 16)) end)
                i = i + 1
            end)
        end
        sock_send("ok")
        return
    end

    -- G — get registers in announced order
    if line == "G" then
        local aliases = REG_ALIASES[shortname] or {}
        local vals = {}
        for _, dname in ipairs(reg_order) do
            local mname = aliases[dname]
            local entry = mname and cpu.state[mname]
            vals[#vals+1] = entry and string.format("%x", entry.value) or "0"
        end
        sock_send(table.concat(vals, " "))
        return
    end

    -- P hex ... — put registers in announced order
    local p_data = line:match("^P (.+)$")
    if p_data then
        local aliases = REG_ALIASES[shortname] or {}
        local i = 1
        for val_str in p_data:gmatch("%S+") do
            local dname = reg_order[i]
            if dname then
                local val = tonumber(val_str, 16) or 0
                if PCL_DECOMPOSES[shortname] and dname == "PCL" then
                    pcall(function()
                        cpu.state["PC"].value = val & 0xFFFF
                        cpu.state["PB"].value = (val >> 16) & 0xFF
                    end)
                else
                    local mname = aliases[dname]
                    if mname then
                        local entry = cpu.state[mname]
                        if entry then pcall(function() entry.value = val end) end
                    end
                end
            end
            i = i + 1
        end
        sock_send("ok")
        return
    end

    -- B+ addr — set user breakpoint
    local bpset_addr = line:match("^B%+ (%x+)$")
    if bpset_addr then
        local addr = tonumber(bpset_addr, 16)
        local idx = bpset(addr)
        if idx then bp_map[addr] = idx end
        sock_send("ok")
        return
    end

    -- B- addr — clear user breakpoint
    local bpclr_addr = line:match("^B%- (%x+)$")
    if bpclr_addr then
        local addr = tonumber(bpclr_addr, 16)
        local idx = bp_map[addr]
        if idx then
            pcall(function() db:command("bpclear " .. idx) end)
            bp_map[addr] = nil
        end
        sock_send("ok")
        return
    end

    -- S <next> [<alt>] — software single step via one-shot breakpoints
    -- Client (sliomame.cpp) computes candidate next-PC(s) from instruction decoding.
    local s_next, s_alt = line:match("^S (%x+) (%x+)$")
    if not s_next then
        s_next = line:match("^S (%x+)$")
    end
    if s_next then
        local next_addr = tonumber(s_next, 16)
        local alt_addr  = s_alt and tonumber(s_alt, 16) or nil

        -- Set one-shot bps at each candidate (only if no user bp already there;
        -- if a user bp is already there it will hold the stop, and we won't clear it).
        clear_step_bps()
        if not bp_map[next_addr] then
            local idx = bpset(next_addr)
            if idx then step_bps[next_addr] = idx end
        end
        if alt_addr and not bp_map[alt_addr] then
            local idx = bpset(alt_addr)
            if idx then step_bps[alt_addr] = idx end
        end

        emu.unpause()
        halt_reason = nil
        pending_reply = "step"
        step_tick_count = 0
        return  -- reply deferred
    end

    -- C — continue
    if line == "C" then
        clear_step_bps()
        emu.unpause()
        halt_reason = nil
        sock_send("ok")
        return
    end

    -- H — halt (true freeze via emu.pause)
    if line == "H" then
        emu.pause()
        halt_reason = "halt"
        sock_send(string.format("stopped %x halt", get_genpc()))
        return
    end

    -- ? — poll execution state
    if line == "?" then
        if is_paused() then
            sock_send(string.format("stopped %x %s", get_genpc(), halt_reason or "unknown"))
            return
        end
        -- Not scheduler-paused: check for bp pseudo-hold (exec_state == "stop")
        local st = exec_state()
        if st == "stop" then
            -- bp pseudo-hold: convert to true freeze
            emu.pause()
            halt_reason = "bp"
            sock_send(string.format("stopped %x bp", get_genpc()))
        else
            sock_send("running")
        end
        return
    end

    -- RESET — soft reset
    if line == "RESET" then
        manager.machine:soft_reset()
        sock_send("ok")
        return
    end

    -- BYE — client disconnect; reopen listener
    if line == "BYE" then
        bp_map = {}; step_bps = {}; reg_order = {}
        pending_reply = nil; step_tick_count = 0; halt_reason = nil
        open_server()
        return
    end

    sock_send("err unknown: " .. line)
end

-- ── periodic pump ─────────────────────────────────────────────────────────────
open_server()

emu.register_periodic(function()
    if not srv then return end
    if not db then init_machine() return end

    -- Deferred step: waiting for a one-shot bp to fire.
    if pending_reply == "step" then
        local st = exec_state()
        if st == "stop" then
            -- bp (or something) fired: clear one-shots, freeze, reply
            clear_step_bps()
            emu.pause()
            halt_reason = "step"
            sock_send(string.format("stopped %x step", get_genpc()))
            pending_reply = nil
            step_tick_count = 0
        else
            -- Still running: watchdog counter
            step_tick_count = step_tick_count + 1
            if step_tick_count > STEP_WATCHDOG_TICKS then
                clear_step_bps()
                emu.pause()
                halt_reason = "step-timeout"
                sock_send(string.format("stopped %x step-timeout", get_genpc()))
                pending_reply = nil
                step_tick_count = 0
            end
        end
        return
    end

    -- Normal command dispatch (one per tick for predictable latency)
    local line = readline()
    if line and #line > 0 then
        local ok, err = pcall(dispatch, line)
        if not ok then
            sock_send("err internal: " .. tostring(err))
        end
    end
end)
