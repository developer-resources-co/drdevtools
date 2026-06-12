//=============================================================================
// spc_reg.cpp: SPC700 (SNES audio co-CPU) register window.
// Mirrors reg.cpp, trimmed to the 6 SPC700 registers (PC A X Y SP PSW).
// Editable like the main Register window: live read via the bridge `GA` command,
// click-to-edit via GetExpr, write-back via `PA`.  Whole TU is #ifdef SPC700, so
// it compiles to nothing on platforms that don't declare the feature.
//=============================================================================

#include	"moninc.hpp"
#include	"help.hpp"

#ifdef SPC700

//=============================================================================

FLAG spcRegOpen = boolean::FALSE;
_object *spcRegObjPtr = NULL;

ULONG spcRegs[NUMSPCREGS];			// PC A X Y SP PSW (ANSI guarantees 0-init)

static char spcRegExprTitle[60];

static char *spcRegNameArray[NUMSPCREGS] =
{
	"PC", "A", "X", "Y", "SP", "PSW"
};

static ULONG spcRegMaskArray[NUMSPCREGS] =
{
	0xffff, 0xff, 0xff, 0xff, 0xff, 0xff
};

enum SPCREGGAD
{
	SPCGAD_BASE = GAD_USER+1,
	SPCGAD_PC   = GAD_USER+1,
	SPCGAD_A,
	SPCGAD_X,
	SPCGAD_Y,
	SPCGAD_SP,
	SPCGAD_PSW
};

// window content (3 rows); values are PrintLay'd over the blanks after each label.
// Layout mirrors the 65816 Register window: A/X/Y left, PSW/SP/PC right.
static char spcRegText[] =
"A:          PSW:\n"
"X:            SP:\n"
"Y:            PC:";

#define SPCA_X    3
#define SPCA_Y    1
#define SPCX_X    3
#define SPCX_Y    2
#define SPCY_X    3
#define SPCY_Y    3
#define SPCPSW_X  17
#define SPCPSW_Y  1
#define SPCSP_X   18
#define SPCSP_Y   2
#define SPCPC_X   18
#define SPCPC_Y   3

//=============================================================================

static char *
PrintSpcFlags(char *buffer, UBYTE psw)
{
	// SPC700 PSW: N V P B H I Z C
	*buffer++ = ((psw & 0x80)?'N':'n');
	*buffer++ = ((psw & 0x40)?'V':'v');
	*buffer++ = ((psw & 0x20)?'P':'p');
	*buffer++ = ((psw & 0x10)?'B':'b');
	*buffer++ = ((psw & 0x08)?'H':'h');
	*buffer++ = ((psw & 0x04)?'I':'i');
	*buffer++ = ((psw & 0x02)?'Z':'z');
	*buffer++ = ((psw & 0x01)?'C':'c');
	return(buffer);
}

//=============================================================================

void
AddSpcRegGadgets(_window *pWindow)
{
	MakeGadget(pWindow, SPCPC_X,  SPCPC_Y,  4,1, SPCGAD_PC);
	MakeGadget(pWindow, SPCSP_X,  SPCSP_Y,  2,1, SPCGAD_SP);
	MakeGadget(pWindow, SPCA_X,   SPCA_Y,   2,1, SPCGAD_A);
	MakeGadget(pWindow, SPCX_X,   SPCX_Y,   2,1, SPCGAD_X);
	MakeGadget(pWindow, SPCY_X,   SPCY_Y,   2,1, SPCGAD_Y);
	MakeGadget(pWindow, SPCPSW_X, SPCPSW_Y, 8,1, SPCGAD_PSW);
}

//=============================================================================

void
SpcRegRoutine(_object *oPtr)
{
	_layer *lPtr = oPtr->layer;

	if(slaveUpdate)
		GetSpc700Regs(spcRegs);

	*Print16Bits(textBuffer, spcRegs[SPCREG_PC]) = 0;
	PrintLay(lPtr, textBuffer, SPCPC_X, SPCPC_Y);
	*Print8Bits(textBuffer, spcRegs[SPCREG_A]) = 0;
	PrintLay(lPtr, textBuffer, SPCA_X, SPCA_Y);
	*Print8Bits(textBuffer, spcRegs[SPCREG_X]) = 0;
	PrintLay(lPtr, textBuffer, SPCX_X, SPCX_Y);
	*Print8Bits(textBuffer, spcRegs[SPCREG_Y]) = 0;
	PrintLay(lPtr, textBuffer, SPCY_X, SPCY_Y);
	*Print8Bits(textBuffer, spcRegs[SPCREG_SP]) = 0;
	PrintLay(lPtr, textBuffer, SPCSP_X, SPCSP_Y);
	*PrintSpcFlags(textBuffer, spcRegs[SPCREG_PSW]) = 0;
	PrintLay(lPtr, textBuffer, SPCPSW_X, SPCPSW_Y);
}

//=============================================================================

void
SetSpcReg(int reg, ULONG value)
{
	if(slaveUpdate)
		GetSpc700Regs(spcRegs);
	spcRegs[reg] = value & spcRegMaskArray[reg];
	if(slaveUpdate)
		PutSpc700Regs(spcRegs);
}

void
SpcRegEditRout(ULONG value, void *dataPtr)
{
	_gadget *gPtr = (_gadget *)dataPtr;
	SetSpcReg(gPtr->gNum - SPCGAD_BASE, value);
}

//=============================================================================

FLAG
SpcRegInput(_input *in, _object *oPtr)
{
	_window *pWindow = (_window *)oPtr->layer;
	FLAG inputUsed = boolean::TRUE;
	_gadget *gPtr;

	switch(in->inputType)
	 {
		case INP_KEY:
			switch(in->fullKey)
			 {
				case CMD_CLOSEWINDOW:
					delete pWindow;
					delete oPtr;
					spcRegObjPtr = NULL;
					spcRegOpen = boolean::FALSE;
					break;
				default:
					inputUsed = boolean::FALSE;
					break;
			 }
			break;
		case INP_MOUSE_LEFTBUTTON_DOWN:
			if((gPtr = GadgetHit(&pWindow->gadgBase, in->mouseX - pWindow->xPos, in->mouseY - pWindow->yPos)) != 0)
			 {
				switch(gPtr->gNum)
				 {
					case 1:							// system close gadget
						delete pWindow;
						delete oPtr;
						spcRegObjPtr = NULL;
						spcRegOpen = boolean::FALSE;
						inputUsed = boolean::TRUE;
						break;
					case SPCGAD_PC:
					case SPCGAD_A:
					case SPCGAD_X:
					case SPCGAD_Y:
					case SPCGAD_SP:
					case SPCGAD_PSW:
						sprintf(expString, "$%lx", spcRegs[gPtr->gNum - SPCGAD_BASE]);
						PadString(expString, EXPRLEN);
						sprintf(spcRegExprTitle, "Enter new value for SPC register %s",
						        spcRegNameArray[gPtr->gNum - SPCGAD_BASE]);
						GetExpr(SpcRegEditRout, gPtr, spcRegExprTitle, oPtr->layer);
						break;
					default:
						inputUsed = boolean::FALSE;
						break;
				 }
			 }
			break;
		default:
			inputUsed = boolean::FALSE;
			break;
	 }
	return(inputUsed);
}

//=============================================================================

FLAG
OpenSpcReg(void)
{
	_object *oPtr;
	_window *pWindow;

	if(spcRegOpen)
	 {
		assert( spcRegObjPtr );
		spcRegObjPtr->layer->ToFront();
		ActivateFrontWindow();
		return(boolean::TRUE);
	 }

	oPtr = AddObject(SpcRegRoutine);
	pWindow = new _window(0, 7, 26, 5, "SPC Register", regAttr, regChar);
	if(pWindow)
	 {
		spcRegOpen = boolean::TRUE;
		spcRegObjPtr = oPtr;
		pWindow->data = (void *)oPtr;
		oPtr->inputRoutine = SpcRegInput;
		oPtr->layer = pWindow;
		oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;

		PrintLay(pWindow, spcRegText, 1, 1);
		AddSysGadgets(pWindow);
		AddSpcRegGadgets(pWindow);
		DrawGadgets(pWindow);
		ActivateFrontWindow();
	 }
	else
		PrintError(ERROR_NOMEM);
	return(boolean::TRUE);
}

#endif // SPC700
