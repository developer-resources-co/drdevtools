//=============================================================================
// which.cpp: which slave are we debugging
//=============================================================================

#include <assert.h>

#include	"moninc.hpp"
#include	"display.hpp"
#include	"dis.hpp"
#include	"memory.hpp"


					// debugger specific libraries
#ifdef EMUL
#include "slioemul.cpp"			// slave emulation code
#else
#ifdef MAMEBACKEND
// SNES (mame_bridge.lua) and Genesis (mame_genesis_bridge.lua) share one TCP-bridge
// client; platform specifics are #ifdef SNES/GENESIS inside sliomame.cpp.  (Genesis
// dropped MAME gdbstub — see docs/plans/2026-06-14-genesis-backend-fold-the-m68k-into-the-lua-bridge.md.)
#  include "linux/sliomame.cpp"
#else
#ifdef SNES
#include "sliosnes.cpp"
#else
#ifdef GENESIS
#include "sliogen.cpp"
#endif	// GENESIS
#endif	// SNES
#endif	// MAMEBACKEND
#endif	// EMUL
						// different disassemblers
#ifdef SNES
#include "dis816.cpp"				// --- change to dis65816.cpp
#endif
#ifdef GENESIS
#include "dis68000.cpp"
#endif

#ifdef SPC700
// #include "dis700.cpp"   // TODO: SPC700 disassembler never written — SPC RAM window is byte/ascii
#endif

//=============================================================================

