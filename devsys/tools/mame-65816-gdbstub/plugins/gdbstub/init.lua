-- license:BSD-3-Clause
-- copyright-holders: Carl
--
-- 65816 (5a22 / w65c816 / w65c802) register-map fork of MAME's shipped gdbstub plugin.
-- Base: /usr/share/games/mame/plugins/gdbstub/init.lua (MAME 0.277, author Carl, BSD-3-Clause).
-- Additions (drdevtools, 2026-06-13), all gated so the i386 path is behaviorally unchanged:
--   * regmaps["5a22"] with per-register widths (65816 mixes 16/8/1/24-bit registers).
--   * qSupported + qXfer:features:read serving a hand-authored target.xml (XML-aware clients
--     like IDA need this; the shipped plugin serves none).
--   * Z0 software breakpoints wired to bpset (shipped plugin left Z0 unimplemented).
--   * 24-bit PC in stop replies (shipped code used the scalar regsize -> 16-bit truncation).
-- See docs/plans/2026-06-13-mame-65816-gdbstub-map.md.
local exports = {
	name = "gdbstub",
	version = "0.0.1",
	description = "GDB stub plugin",
	license = "BSD-3-Clause",
	author = { name = "Carl" } }

local gdbstub = exports

-- percpu mapping of mame registers to gdb register order
local regmaps = {
	i386 = {
		togdb = {
			EAX = 1, ECX = 2, EDX = 3, EBX = 4, ESP = 5, EBP = 6, ESI = 7, EDI = 8, EIP = 9, EFLAGS = 10, CS = 11, SS = 12,
			DS = 13, ES = 14, FS = 15, GS = 16 },
		fromgdb = {
			"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI", "EIP", "EFLAGS", "CS", "SS", "DS", "ES", "FS", "GS" },
		regsize = 4,
		addrsize = 4,
		pcreg = "EIP"
	}
}
regmaps.i486 = regmaps.i386
regmaps.pentium = regmaps.i386

-- 65816 family. The g65816 core exposes itself under three device short-names:
-- 5a22 (SNES / NSS / sfcbox), w65c816 (Apple IIgs, Foenix-class), w65c802. All share one map.
-- Mixed register widths, so `regs` is an ordered list of {name, size(bytes), type} instead of a
-- single scalar regsize. Order defines both the g-packet byte order and the target.xml <reg> order.
-- PC is the 24-bit linear value (bank in the high byte) zero-extended to 32 bits for client
-- compatibility. Verified against MAME 0.277 + DKC: cpu.shortname == "5a22"; A/X/Y/P/E/S/D/DB/PB/PC
-- all read via cpu.state[name].value; PC == 0x8AB523 (24-bit).
regmaps["5a22"] = {
	regs = {
		{ name = "A",  size = 2, type = "int"      },
		{ name = "X",  size = 2, type = "int"      },
		{ name = "Y",  size = 2, type = "int"      },
		{ name = "P",  size = 1, type = "int"      },
		{ name = "E",  size = 1, type = "int"      },
		{ name = "S",  size = 2, type = "data_ptr" },
		{ name = "D",  size = 2, type = "int"      },
		{ name = "DB", size = 1, type = "int"      },
		{ name = "PB", size = 1, type = "int"      },
		{ name = "PC", size = 4, type = "code_ptr" },
	},
	pcreg = "PC",
	pcsize = 4,
	addrsize = 3,           -- 24-bit address space
	arch = "w65c816",       -- proposed gdb arch name (none official exists)
	feature = "mame.w65c816",
}
regmaps["w65c816"] = regmaps["5a22"]
regmaps["w65c802"] = regmaps["5a22"]

-- Build a gdb target description (target.xml) from an ordered per-register map.
local function build_target_xml(map)
	local parts = {
		'<?xml version="1.0"?>',
		'<!DOCTYPE target SYSTEM "gdb-target.dtd">',
		'<target version="1.0">',
		'  <architecture>' .. map.arch .. '</architecture>',
		'  <feature name="' .. map.feature .. '">',
	}
	for _, r in ipairs(map.regs) do
		parts[#parts + 1] = string.format('    <reg name="%s" bitsize="%d" type="%s"/>',
			r.name, r.size * 8, r.type)
	end
	parts[#parts + 1] = '  </feature>'
	parts[#parts + 1] = '</target>'
	return table.concat(parts, "\n") .. "\n"
end

local target_xml_cache = {}   -- per-map cached XML string

local reset_subscription, stop_subscription

function gdbstub.startplugin()
	local debugger
	local debug
	local cpu
	local breaks
	local watches
	local consolelog
	local consolelast
	local awaiting_stop = false   -- true after c/s: emit one stop reply when the machine halts
	local stop_grace = 0          -- ticks to skip before sampling state (let the scheduler run)

	-- Acquire the debugger + maincpu and reset breakpoint state. Driven from the machine-reset
	-- notifier AND lazily from the periodic: under -autoboot_script the initial reset has already
	-- passed by the time this code runs, so the notifier never fires for it. drmon's bridge uses
	-- the same lazy pattern (`if not db then init() return end`). setup_attempted gates the
	-- decision so we retry only while the debugger isn't ready yet (no per-tick spam afterwards).
	local setup_attempted = false
	local function setup()
		debugger = manager.machine.debugger
		if not debugger then
			return   -- debugger core not up yet; retry next tick (cpu stays nil)
		end
		setup_attempted = true
		cpu = manager.machine.devices[":maincpu"]
		if cpu and not regmaps[cpu.shortname] then
			print("gdbstub: no register map for cpu " .. cpu.shortname)
			cpu = nil
		end
		if cpu then
			consolelog = debugger.consolelog
			consolelast = 0
			breaks = {byaddr = {}, byidx = {}}
			watches = {byaddr = {}, byidx = {}}
			awaiting_stop = false
			stop_grace = 0
		end
	end

	reset_subscription = emu.add_machine_reset_notifier(function ()
		setup_attempted = false
		setup()
	end)

	stop_subscription = emu.add_machine_stop_notifier(function ()
		consolelog = nil
		cpu = nil
		debug = nil
	end)

	local socket = emu.file("", 7)
	local connected = false
	socket:open("socket.127.0.0.1:2159")

	emu.register_periodic(function ()
		if not cpu then
			if not setup_attempted then setup() end   -- lazy attach (autoboot_script path)
			if not cpu then return end
		end

		local function chksum(str)
			local sum = 0
			str:gsub(".", function(s) sum = sum + s:byte() end)
			return string.format("%.2x", sum & 0xff)
		end

		local function makebestr(val, len)
			local str = ""
			for count = 0, len - 1 do
				str = str .. string.format("%.2x", (val >> (count * 8)) & 0xff)
			end
			return str
		end

		-- send a "+$<data>#<cksum>" response packet
		local function reply(data)
			socket:write("+$" .. data .. "#" .. chksum(data))
		end

		-- Single stop notifier. After c/s (or a breakpoint/watchpoint fires during a continue)
		-- the machine transitions to execution_state == "stop"; emit exactly ONE stop reply.
		-- stop_grace skips a tick first so the scheduler actually resumes (continue) or executes
		-- the instruction (step) before we sample the state — otherwise we'd report the pre-c/s
		-- stopped state immediately. The client reads `g` after a stop to get PC/registers.
		-- (The shipped plugin's consolelog-scraping path is removed: it emitted a 2nd, duplicate
		-- stop packet per event, desyncing strict clients, and relied on fragile log-text matching.)
		if awaiting_stop then
			if stop_grace > 0 then
				stop_grace = stop_grace - 1
			else
				-- A stop is either a true scheduler freeze (emu.pause, from break-in/step) or a
				-- breakpoint "pseudo-hold" (execution_state=="stop" while still unpaused). Convert
				-- the latter to a true freeze, then emit exactly one stop reply (client reads `g`).
				local paused = manager.machine.paused
				if paused or debugger.execution_state == "stop" then
					if not paused then emu.pause() end
					awaiting_stop = false
					socket:write("$S05#B8")
				end
			end
		end

		local data = ""

		repeat
			local read = socket:read(100)
			data = data .. read
		until #read == 0
		if #data == 0 then
			return
		end
		if data == "\x03" then
			emu.pause()             -- true scheduler freeze (drmon's proven approach; an
			awaiting_stop = false   -- execution_state="stop" break-in can't be reliably resumed)
			socket:write("$S05#B8")
			return
		end
		local packet, checksum = data:match("%$([^#]+)#(%x%x)")
		if packet then
			packet:gsub("}(.)", function(s) return string.char(string.byte(s) ~ 0x20) end)
			local cmd = packet:sub(1, 1)
			local map = regmaps[cpu.shortname]
			if cmd == "g" then
				local data
				if map.regs then
					-- per-register widths in canonical order
					local parts = {}
					for _, r in ipairs(map.regs) do
						parts[#parts + 1] = makebestr(cpu.state[r.name].value, r.size)
					end
					data = table.concat(parts)
				else
					local regs = {}
					for reg, idx in pairs(map.togdb) do
						regs[idx] = makebestr(cpu.state[reg].value, map.regsize)
					end
					data = table.concat(regs)
				end
				socket:write("+$" .. data .. "#" .. chksum(data))
			elseif cmd == "G" then
				if map.regs then
					-- consume each register's own width (little-endian hex)
					local hex = packet:sub(2)
					local pos = 1
					for _, r in ipairs(map.regs) do
						local nch = r.size * 2
						local chunk = hex:sub(pos, pos + nch - 1)
						pos = pos + nch
						local val = 0
						for b = 0, r.size - 1 do
							local byte = tonumber(chunk:sub(b * 2 + 1, b * 2 + 2), 16) or 0
							val = val | (byte << (b * 8))
						end
						cpu.state[r.name].value = val
					end
					socket:write("+$OK#9a")
				else
					local count = 0
					packet:sub(2):gsub(string.rep("%x", map.regsize * 2), function(s)
							count = count + 1
							cpu.state[map.fromgdb[count]].value = tonumber(s,16)
						end)
					socket:write("+$OK#9a")
				end
			elseif cmd == "m" then
				local addr, len = packet:match("m(%x+),(%x+)")
				if addr and len then
					addr = tonumber(addr, 16)
					len = tonumber(len, 16)
					local data = ""
					local space = cpu.spaces["program"]
					for count = 1, len do
						data = data .. string.format("%.2x", space:readv_u8(addr))
						addr = addr + 1
					end
					socket:write("+$" .. data .. "#" .. chksum(data))
				else
					socket:write("+$E00#a5") -- fix error
				end
			elseif cmd == "M" then
				local count = 0
				-- RSP write-memory is "M addr,len:data" (colon before the hex bytes); the shipped
				-- plugin matched a comma here, so every write silently failed with E00.
				local addr, len, data = packet:match("M(%x+),(%x+):(%x+)")
				if addr and len and data then
					addr = tonumber(addr, 16)
					local space = cpu.spaces["program"]
					data:gsub("%x%x", function(s) space:writev_u8(addr + count, tonumber(s, 16)) count = count + 1 end)
					socket:write("+$OK#9a")
				else
					socket:write("+$E00#a5")
				end
			elseif cmd == "s" then
				if #packet == 1 then
					-- ack only; the single stop notifier emits the stop reply once the step lands.
					cpu.debug:step()   -- arm single-step (execute exactly one instruction)
					emu.unpause()      -- let the scheduler run that one instruction; it then breaks
					                   -- (execution_state=="stop") and the notifier re-pauses us
					awaiting_stop = true
					stop_grace = 1
					socket:write("+")
				else
					socket:write("+$E00#a5")
				end
			elseif cmd == "c" then
				if #packet == 1 then
					-- ack only; the stop reply follows when a breakpoint/watchpoint or break-in halts.
					-- emu.unpause() resumes from the freeze; if we were holding at a bp pseudo-stop,
					-- go() releases the debugger break so the machine actually runs.
					cpu.debug:go()
					emu.unpause()
					awaiting_stop = true
					stop_grace = 1
					socket:write("+")
				else
					socket:write("+$E00#a5")
				end
			elseif cmd == "q" then
				-- query packets: target description + capability/handshake stubs
				if packet:find("^qSupported") then
					reply("PacketSize=1000;qXfer:features:read+;swbreak+;hwbreak+")
				elseif packet:find("^qXfer:features:read:target.xml:") then
					local off, len = packet:match("qXfer:features:read:target.xml:(%x+),(%x+)")
					if off and len and map.regs then
						off = tonumber(off, 16)
						len = tonumber(len, 16)
						local xml = target_xml_cache[map.feature]
						if not xml then
							xml = build_target_xml(map)
							target_xml_cache[map.feature] = xml
						end
						local total = #xml
						if off >= total then
							reply("l")
						else
							local last = math.min(off + len, total)
							local chunk = xml:sub(off + 1, last)
							reply((last >= total and "l" or "m") .. chunk)
						end
					else
						reply("")   -- no target.xml for uniform-width maps (i386)
					end
				elseif packet:find("^qAttached") then
					reply("1")
				elseif packet:find("^qfThreadInfo") then
					reply("l")     -- end of thread list
				elseif packet:find("^qC") then
					reply("")      -- no current thread id
				else
					reply("")      -- qOffsets/qTStatus/qSymbol/... : unsupported
				end
			elseif cmd == "H" then
				reply("OK")        -- thread-select: single-threaded, always OK
			elseif cmd == "v" then
				reply("")          -- vMustReplyEmpty and friends
			elseif cmd == "Z" then
				local btype, addr, kind = packet:match("Z([0-4]),(%x+),(.*)")
				addr = tonumber(addr, 16)
				if btype == "0" or btype == "1" then
					-- software (Z0) and hardware (Z1) breakpoints both map to bpset
					if breaks.byaddr[addr] then
						socket:write("+$E00#a5")
						return
					end
					-- cond/act are char const* with no defaults in MAME 0.277; passing nil
					-- segfaults breakpoint_set (strlen on null). "1" = unconditional.
					local idx = cpu.debug:bpset(addr, "1", "")
					breaks.byaddr[addr] = idx
					breaks.byidx[idx] = addr
					socket:write("+$OK#9a")
				elseif btype == "2" then
					if watches.byaddr[addr] then
						socket:write("+$E00#a5")
						return
					end
					local idx = cpu.debug:wpset(cpu.spaces["program"], "w", addr, 1, "1", "")
					watches.byaddr[addr] = idx
					watches.byidx[idx] = {addr = addr, type = "watch"}
					socket:write("+$OK#9a")
				elseif btype == "3" then
					if watches.byaddr[addr] then
						socket:write("+$E00#a5")
						return
					end
					local idx = cpu.debug:wpset(cpu.spaces["program"], "r", addr, 1, "1", "")
					watches.byaddr[addr] = idx
					watches.byidx[idx] = {addr = addr, type = "rwatch"}
					socket:write("+$OK#9a")
				elseif btype == "4" then
					if watches.byaddr[addr] then
						socket:write("+$E00#a5")
						return
					end
					local idx = cpu.debug:wpset(cpu.spaces["program"], "rw", addr, 1, "1", "")
					watches.byaddr[addr] = idx
					watches.byidx[idx] = {addr = addr, type = "awatch"}
					socket:write("+$OK#9a")
				end
			elseif cmd == "z" then
				local btype, addr, kind = packet:match("z([0-4]),(%x+),(.*)")
				addr = tonumber(addr, 16)
				if btype == "0" or btype == "1" then
					if not breaks.byaddr[addr] then
						socket:write("+$E00#a5")
						return
					end
					local idx = breaks.byaddr[addr]
					cpu.debug:bpclear(idx)   -- MAME 0.277 binding is bpclear, not bpclr
					breaks.byaddr[addr] = nil
					breaks.byidx[idx] = nil
					socket:write("+$OK#9a")
				elseif btype == "2" or btype == "3" or btype == "4" then
					if not watches.byaddr[addr] then
						socket:write("+$E00#a5")
						return
					end
					local idx = watches.byaddr[addr]
					cpu.debug:wpclear(idx)   -- MAME 0.277 binding is wpclear, not wpclr
					watches.byaddr[addr] = nil
					watches.byidx[idx] = nil
					socket:write("+$OK#9a")
				end
			elseif cmd == "?" then
				socket:write("+$S05#B8")
			else
				socket:write("+$#00")
			end
		end
	end)
end

return exports
