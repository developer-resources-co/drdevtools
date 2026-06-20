---
name: mame-lua-debugger-control-flow
description: "MAME 0.277 -debugger none Lua control-flow gotchas; reuse mame_bridge.lua, don't re-derive"
metadata: 
  node_type: memory
  type: feedback
  originSessionId: 64e65808-b40b-48a9-b013-a3cc05d43a01
---

When driving MAME 0.277's debugger from Lua (`-debug -debugger none`, plugin or `-autoboot_script`),
`devsys/tools/drmon/linux/mame_bridge.lua` is the **reference implementation** that already solved
the hard parts. Read it before writing new MAME-debugger Lua — don't rediscover these:

- **Stop/continue must use `emu.pause()` / `emu.unpause()`**, NOT `debugger.execution_state` or
  `cpu.debug:go()`. A machine stopped via `execution_state="stop"` can't be reliably resumed by
  `go()`/`execution_state="run"` in this context (PC stays stuck).
- **Breakpoints are a "pseudo-hold":** when one fires, `execution_state=="stop"` while
  `manager.machine.paused` is still false; the CPU is *not* hard-frozen, so it can run a few
  instructions past the bp before you `emu.pause()` it (the reported stop PC drifts). The C++
  `debuggdbstub.cpp` module freezes exactly; the Lua path does not.
- **The reset notifier (`add_machine_reset_notifier`) does NOT fire under `-autoboot_script`** (the
  reset already passed) — lazy-init `cpu`/`debugger` from the periodic: `if not db then init() return end`.
- **Lua binding signatures (0.277):** `bpset(addr, cond, act)` / `wpset(sp,type,addr,len,cond,act)`
  take `char const*` cond/act with **no defaults** — passing nil → `strlen(nullptr)` → SIGSEGV; pass
  `"1",""`. Clear methods are **`bpclear`/`wpclear`**, not `bpclr`/`wpclr` (the shipped gdbstub
  plugin's names are wrong). These bindings live in `src/frontend/mame/luaengine_debug.cpp`.
- **Headless launch:** `-window` crashes MAME's GL init on a headless host; bare `-video none` exits
  immediately unless an `-autoboot_script` registers a periodic. Use `-video none` + autoboot.

**Why:** I burned many MAME launch/crash cycles re-deriving all of this while building the 65816
gdbstub Lua, when `mame_bridge.lua` (and the SRC "fix-forward / reuse python-tui-lib" ethos) already
encoded it. **How to apply:** for any MAME-debugger Lua task, open `mame_bridge.lua` first and lift
its primitives. Related: [[features-not-platforms]].
