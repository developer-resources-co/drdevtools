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
#ifdef SNES
#include "sliosnes.cpp"
#else
#ifdef GENESIS
#include "sliogen.cpp"
#endif	// GENESIS
#endif	// SNES
#endif	// EMUL
						// different disassemblers
#ifdef SNES
#include "dis816.cpp"				// --- change to dis65816.cpp
#endif
#ifdef GENESIS
#include "dis68000.cpp"
#endif

#ifdef SPC700
#include "dis700.cpp"
#endif

#ifdef MASTERSYSTEM
#include "disZ80.cpp"
#endif

#ifdef NES
#include "dis6502.cpp"
#endif

//=============================================================================

