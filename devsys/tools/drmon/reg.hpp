//=============================================================================
//  reg.h: register window header file
//=============================================================================

#if !defined(REG_H)
#define REG_H

#ifdef GENESIS
enum REGDEF
{
    REG_D0,
    REG_D1,
    REG_D2,
    REG_D3,
    REG_D4,
    REG_D5,
    REG_D6,
    REG_D7,
    REG_A0,
    REG_A1,
    REG_A2,
    REG_A3,
    REG_A4,
    REG_A5,
    REG_A6,
    REG_A7,
	REG_SSP = REG_A7,
    REG_USP,
    REG_SR,
    REG_PC
};

#define NUMREGS 19


#endif

#ifdef SNES

enum REGDEF
{
    REG_A,
    REG_X,
    REG_Y,
    REG_FLAGS,
    REG_XFLAGS,
    REG_D,
    REG_DB,
    REG_PB,
    REG_SP,
    REG_PC,
		REG_PCL
};

#define NUMREGS 11
extern void LoadPCPB(void);
extern void LoadPCL( void );

#endif

//=============================================================================

#ifdef GENESIS
#define REG_INSTRUCTIONPOINTER	REG_PC
#define REG_STACKPOINTER			REG_A7
#endif
#ifdef SNES
#define REG_INSTRUCTIONPOINTER	REG_PCL
#define REG_STACKPOINTER			REG_SP
#endif

//==============================================================================

extern ULONG regs[NUMREGS];
extern ULONG regMaskArray[NUMREGS];
extern char* regNameArray[NUMREGS];

//==============================================================================

FLAG OpenReg(void);
ULONG GetReg(int reg);
ULONG GetSP( void );

void
SetReg(int reg,ULONG value);

ULONG GetRegAsSymbol(char *string);

FLAG
SetRegAsSymbol(char *string, ULONG v);

//==============================================================================

#endif

//=============================================================================

