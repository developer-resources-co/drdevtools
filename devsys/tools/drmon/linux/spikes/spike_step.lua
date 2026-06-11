-- spike_step.lua — Phase 2 single-step investigation spike.
-- Tested the emu.pause() + db:command("step") combination and one-shot bp approach.
--
-- Findings (recorded in docs/plans/2026-06-11-drmon-mame-backend.md §Finding 2):
--   • emu.pause() + db:command("step") does NOT give clean single-instruction stepping:
--     the scheduler pause and the debugger step do not compose — the machine often
--     executes 0 or many instructions instead of exactly 1.
--   • db:command("bpset 0xNNNN") sets a one-shot breakpoint via the MAME debugger CLI.
--     When paired with emu.unpause(), the machine runs until the bp fires, then
--     execution_state → "stop" in the next periodic tick (where it must be frozen
--     immediately with emu.pause()).
--   • For software single-step the client (sliomame.cpp) computes the candidate
--     next-PC address(es) via instruction decoding, sends them as `S <next> [<alt>]`,
--     and the bridge sets one-shot bps at each candidate.
--
-- Note: db:command() is the safe API.  cpu.debug:bpset() PANICS in MAME 0.277.
--
-- This spike is not meant to be run again; it is archived for reference.

local log = print

-- Tested approach 1: pause + step command (BROKEN)
-- emu.pause()
-- db:command("step")   -- sometimes executes 0 instructions, sometimes many

-- Tested approach 2: one-shot bp + unpause (WORKING, adopted in mame_bridge.lua v2)
-- local function step_via_bp(next_addr)
--     db:command(string.format("bpset %x", next_addr))
--     emu.unpause()
--     -- In the next periodic tick where execution_state == "stop":
--     --   db:command("bpclear <idx>")
--     --   emu.pause()
-- end

log("spike_step.lua: archived reference — see mame_bridge.lua v2 for the adopted design")
