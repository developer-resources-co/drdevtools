-- gdbstub_autoboot.lua — headless launch shim for the 65816 gdbstub plugin.
--
-- The plugin form (plugins/gdbstub/init.lua + plugin.json) is the real-install deliverable:
-- drop it into MAME's pluginspath and run with `-plugin gdbstub`. But that path needs a video
-- target to keep MAME's event loop alive, and `-window` crashes MAME's GL init on some headless
-- hosts. drmon's bridge sidesteps both by running its Lua via `-autoboot_script` under
-- `-video none` (a registered periodic keeps the run loop alive with no display). This shim does
-- the same for the gdbstub plugin: it loads the *same* init.lua and calls its start function, so
-- there is one source of truth for the protocol logic.
--
-- Usage (see run-mame.sh): GDBSTUB_INIT=<abs path to plugins/gdbstub/init.lua> is read here.
local path = os.getenv("GDBSTUB_INIT")
if not path then
	error("gdbstub_autoboot: set GDBSTUB_INIT to the absolute path of plugins/gdbstub/init.lua")
end
local exports = dofile(path)
exports.startplugin()
