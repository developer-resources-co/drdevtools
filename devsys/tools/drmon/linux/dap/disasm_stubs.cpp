//=============================================================================
// disasm_stubs.cpp — Print* implementations and stub globals for drmon-dap.
//
// Provides every external symbol that dis816.cpp / dis68000.cpp call through
// general.hpp / memory.hpp, without pulling in general.cpp's TUI dependency
// chain (symbol.hpp → listrect.hpp → object.hpp → moninc.hpp).
//=============================================================================

#include "disasm_preamble.hpp"  // global.hpp + general.hpp pulled in here

#include <string.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

int  procMode            = 0;   // updated from FLAGS before each Disassem call
FLAG printCheckForSymbol = 0;   // always off — no symbol table in DAP build
char hexTable[16]        = { '0','1','2','3','4','5','6','7',
                              '8','9','A','B','C','D','E','F' };

struct _symbolList symbolListBase;

_symbolList* FindHexSymbol(ULONG) { return nullptr; }

//-----------------------------------------------------------------------------
// Print* helpers (subset of general.cpp that the disassemblers use)
//-----------------------------------------------------------------------------

static inline char* h2(char* b, unsigned v) {
    *b++ = hexTable[(v >> 4) & 0xf];
    *b++ = hexTable[v & 0xf];
    return b;
}

char* Print4Bits(char* b, UBYTE n) {
    *b++ = hexTable[n & 0xf];
    return b;
}

char* Print8Bits(char* b, UBYTE n) {
    return h2(b, n);
}

char* Print16Bits(char* b, UWORD n) {
    b = h2(b, (n >> 8) & 0xff);
    b = h2(b, n & 0xff);
    return b;
}

char* PrintRaw24Bits(char* b, ULONG n) {
    b = h2(b, (n >> 16) & 0xff);
    b = h2(b, (n >>  8) & 0xff);
    b = h2(b, n & 0xff);
    return b;
}

char* Print24Bits(char* b, ULONG n) {
    *b++ = '$';
    return PrintRaw24Bits(b, n);
}

char* PrintRaw32Bits(char* b, ULONG n) {
    b = h2(b, (n >> 24) & 0xff);
    b = h2(b, (n >> 16) & 0xff);
    b = h2(b, (n >>  8) & 0xff);
    b = h2(b, n & 0xff);
    return b;
}

char* Print32Bits(char* b, ULONG n) {
    *b++ = '$';
    return PrintRaw32Bits(b, n);
}

char* PrintString(char* b, const char* s) {
    while (*s) *b++ = *s++;
    return b;
}

void PadString(char* b, int off) {
    int i = (int)strlen(b);
    for (; i < off; i++) b[i] = ' ';
    b[off] = 0;
}
