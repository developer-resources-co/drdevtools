#pragma once
// Per-system register tables for drmon-dap.
// Mirrors regNameArray[] / NUMREGS in reg.cpp but without any TUI dependency.

#include <cstddef>

struct RegTable {
    const char* const* names;
    const unsigned long* masks;
    int count;
    int pcIndex;   // index of the PC (or PCL for SNES) register
    int pbIndex;   // index of the bank register, or -1
};

// ---------------------------------------------------------------------------
// SNES / 65816 (NUMREGS=11, indices match reg.hpp enum REGDEF for SNES)
// REG_A=0 REG_X=1 REG_Y=2 REG_FLAGS=3 REG_XFLAGS=4(EMUL)
// REG_D=5  REG_DB=6 REG_PB=7 REG_SP=8 REG_PC=9 REG_PCL=10
// ---------------------------------------------------------------------------
static const char* kSnesRegNames[11] = {
    "A","X","Y","FLAGS","EMUL","D","DB","PB","SP","PC","PCL"
};
static const unsigned long kSnesRegMasks[11] = {
    0xffff,0xffff,0xffff,0xff,0x01,0xffff,0xff,0xff,0xffff,0xffff,0xffffff
};
// P register byte layout (FLAGS = index 3):
static const int kSnesRegFlagsIdx = 3;
// PCL (full 24-bit PC) is the canonical instruction pointer
static const int kSnesPCLIdx = 10;
// PB is REG_PB = index 7
static const int kSnesPBIdx  = 7;

static constexpr RegTable kSnesRegs = {
    kSnesRegNames, kSnesRegMasks, 11,
    /*pcIndex=*/10, /*pbIndex=*/7
};

// ---------------------------------------------------------------------------
// Genesis / 68000 (NUMREGS=19, indices match reg.hpp enum REGDEF for GENESIS)
// D0-D7 = 0-7, A0-A6 = 8-14, A7(SSP) = 15, USP = 16, SR = 17, PC = 18
// ---------------------------------------------------------------------------
static const char* kGenRegNames[19] = {
    "D0","D1","D2","D3","D4","D5","D6","D7",
    "A0","A1","A2","A3","A4","A5","A6","A7",
    "USP","SR","PC"
};
static const unsigned long kGenRegMasks[19] = {
    0xffffffff,0xffffffff,0xffffffff,0xffffffff,
    0xffffffff,0xffffffff,0xffffffff,0xffffffff,
    0xffffffff,0xffffffff,0xffffffff,0xffffffff,
    0xffffffff,0xffffffff,0xffffffff,0xffffffff,
    0xffffffff,0x0000ffff,0xffffffff
};

static constexpr RegTable kGenRegs = {
    kGenRegNames, kGenRegMasks, 19,
    /*pcIndex=*/18, /*pbIndex=*/-1
};
