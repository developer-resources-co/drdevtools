//=============================================================================
// spc_reg.hpp: SPC700 (SNES audio co-CPU) register window
//=============================================================================

#ifdef SPC700

enum SPCREGDEF
{
	SPCREG_PC,
	SPCREG_A,
	SPCREG_X,
	SPCREG_Y,
	SPCREG_SP,
	SPCREG_PSW
};
#define NUMSPCREGS 6

FLAG OpenSpcReg(void);

#endif
