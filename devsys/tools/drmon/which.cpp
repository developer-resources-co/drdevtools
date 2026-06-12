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
#  ifdef SNES
#    include "linux/sliomame.cpp"   // Phase 2: SNES — TCP bridge to mame_bridge.lua
#  else
#    include "linux/sliogdb.cpp"    // Phase 2: Genesis — MAME native GDB RSP (-debugger gdbstub)
#  endif
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

