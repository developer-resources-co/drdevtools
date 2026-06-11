#pragma once
// DAP build: satisfy dis816.cpp / dis68000.cpp header deps without TUI.
//
// Included before the disassembler source via a thin wrapper .cpp — provides
// forward declarations that break the moninc.hpp / symbol.hpp dependency chain.

// Forward-declare _object so memory.hpp's MemoryGetLineAddr(object*, int) compiles.
// dis816.cpp never calls MemoryGetLineAddr; a pointer-only declaration is enough.
class _object;

// Minimal _symbolList for the FindHexSymbol / ->Name() calls inside dis816.cpp.
// printCheckForSymbol is always false in the DAP build so Name() is never called,
// but the type must be complete at the call site.
struct _symbolList {
    const char* Name() const { return ""; }
};
_symbolList* FindHexSymbol(unsigned long);
extern struct _symbolList symbolListBase;

// Declare Print* helpers so dis68000.cpp (which doesn't include general.hpp itself)
// can call them.  global.hpp must come first (provides FLAG/UBYTE/UWORD/ULONG);
// it is safe here because linux_compat.hpp is force-included at the target level,
// which stubs out <dos.h> before global.hpp's own #include <dos.h> is reached.
#include "global.hpp"
#include "general.hpp"
