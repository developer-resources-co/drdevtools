-- spike_hold.lua — Phase 2 hold-semantics investigation spike.
-- Ran under MAME 0.277 to determine whether execution_state="stop" provides
-- a stable freeze or a transient one.
--
-- Findings (recorded in docs/plans/2026-06-11-drmon-mame-backend.md §Finding 1):
--   • execution_state transitions from "run"→"stop"→"run" within the same
--     Lua periodic tick when a breakpoint fires under -debugger none.
--     The "stop" state is NOT stable — polling it one tick later sees "run" again.
--   • emu.pause() provides a true scheduler freeze that persists until emu.unpause().
--   • emu.pause is a global on the emu table (NOT manager.machine:pause()).
--     manager.machine.paused is a read-only property.
--
-- This spike is not meant to be run again; it is archived for reference.

local log = print

emu.register_periodic(function()
    local db = manager.machine.debugger
    if not db then return end

    local st = db.execution_state
    log("execution_state = " .. tostring(st))

    -- Attempt a bp, observe state transitions
    if st == "stop" then
        log("Caught stop — now checking one tick later")
        -- Will log "run" the next tick, proving transience.
    end
end)

-- Hold via emu.pause (the correct approach):
-- emu.pause()          -- true freeze (confirmed working)
-- emu.unpause()        -- resume
-- manager.machine.paused  -- read-only: true when paused
