//=============================================================================
//  slio_stub.cpp — link residue for board.cpp (DRMON_MAME_BACKEND=OFF only).
//  Provides asm-export symbols (SwapWord, GetAttention, …) that board.cpp and
//  the expression evaluator reference.  When DRMON_MAME_BACKEND=ON, sliomame.cpp
//  provides these instead and this file is excluded from the build.
//  See docs/plans/2026-06-10-port-drmon-linux.md.
//=============================================================================

extern "C" {

// NB: the shared dev-link globals (slaveCtrlc, slaveBufferCmdc, slaveCtrlBits,
// snesStatus, ...) are already defined via a header pulled in by which.cpp, so
// we deliberately do NOT define them here (that would be a duplicate symbol).

// Functions snesio.asm exported (Public C).
unsigned int  SwapWord(unsigned int a) {
    return (unsigned int)(((a & 0xFF) << 8) | ((a >> 8) & 0xFF));
}
unsigned long SwapLong(unsigned long a) {
    return ((a >> 24) & 0xFFUL) | ((a >> 8) & 0xFF00UL)
         | ((a << 8) & 0xFF0000UL) | ((a << 24) & 0xFF000000UL);
}
unsigned long SwapLongWords(unsigned long a) { return a; }

int  GetAttention(void)   { return 0; }   // no target → never "attention"
void GetSlaveBus(void)    {}
void ReturnSlaveBus(void) {}
void SendCmd(char)        {}

} // extern "C"
