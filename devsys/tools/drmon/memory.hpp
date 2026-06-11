//==============================================================================
//	Memory.hpp:
//==============================================================================

#ifndef _MEMORY_HPP_
#define _MEMORY_HPP_

//============================================================================


extern	int procMode;

extern ULONG
MemoryGetLineAddr(_object *oPtr,int line);

//============================================================================
// memory commands
enum
{
	MEMORY_DOWNLINE = 1,
	MEMORY_UPLINE,
	MEMORY_PGDWN,
	MEMORY_PGUP,
	MEMORY_DOWN,
	MEMORY_UP,
	MEMORY_RUNTOHERE,
	MEMORY_SETBREAKHERE,
	MEMORY_SETBREAKONCEHERE,
	MEMORY_SETBREAKCOUNTHERE,
	MEMORY_CLEARALLBREAKS,
	MEMORY_CLEARBREAK,
	MEMORY_CHANGEMEM,
	MEMORY_PROCHELP,
	MEMORY_BYTE,
	MEMORY_WORD,
	MEMORY_LONG,
	MEMORY_CODE,
	MEMORY_ASCII,
	MEMORY_GOTO,
#ifdef SPC700
	MEMORY_SPC			// SPC700 APU-RAM view (SPC700 feature)
#endif
};

//==============================================================================

#endif

//==============================================================================
