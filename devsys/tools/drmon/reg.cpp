//=============================================================================
//  reg.c: register window object
//=============================================================================

#include	<ctype.h>
#include	<dos.h>

#include	"moninc.hpp"
//#include	"display.hpp"
#include	"help.hpp"

//==============================================================================

FLAG regOpen = boolean::FALSE;
_object *regObjPtr = NULL;


char regExprTitle[50];      // note: if regNameArray contains any larger strings,
                            // this must be updated

#ifdef GENESIS

enum REGGADHGDFGD
{
    RGGAD_BASE = GAD_USER+1,
    RGGAD_D0 = GAD_USER+1,
    RGGAD_D1,
    RGGAD_D2,
    RGGAD_D3,
    RGGAD_D4,
    RGGAD_D5,
    RGGAD_D6,
    RGGAD_D7,
    RGGAD_A0,
    RGGAD_A1,
    RGGAD_A2,
    RGGAD_A3,
    RGGAD_A4,
    RGGAD_A5,
    RGGAD_A6,
    RGGAD_A7,
    RGGAD_USP,
    RGGAD_SR,
    RGGAD_PC
};

char *regNameArray[NUMREGS] =
{
    "D0",
    "D1",
    "D2",
    "D3",
    "D4",
    "D5",
    "D6",
    "D7",
    "A0",
    "A1",
    "A2",
    "A3",
    "A4",
    "A5",
    "A6",
    "A7",
    "USP",
    "SR",
    "PC"
};

ULONG regMaskArray[NUMREGS] =
{
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0x0000ffff,
    0xffffffff
};

#endif

#ifdef SNES

enum REGGADHGDFGD
{
    RGGAD_BASE = GAD_USER+1,
    RGGAD_A = GAD_USER+1,
    RGGAD_X,
    RGGAD_Y,
    RGGAD_FLAGS,
    RGGAD_XFLAGS,
    RGGAD_D,
    RGGAD_DB,
    RGGAD_PB,
    RGGAD_SP,
    RGGAD_PC
};

char *regNameArray[NUMREGS] =
{
	"A",
	"X",
	"Y",
	"FLAGS",
	"EMUL",
	"D",
	"DB",
	"PB",
	"SP",
	"PC",
	"PCL"
};

ULONG regMaskArray[NUMREGS] =
{
	0xffff,
	0xffff,
	0xffff,
	0x00ff,
	0x0001,
	0xffff,
	0x00ff,
	0x00ff,
	0xffff,
	0xffff,
	0xffffff
};

char *
PrintXFlags(char *buffer,UBYTE flags)
{
	*buffer++ = ((flags & 0x01)?'E':'e');
	return(buffer);
}

#endif

//=============================================================================

ULONG  regs[NUMREGS];	// Initialization not required--ANSI C guarantees 0
#if 0
=
{
#ifdef GENESIS
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifdef SNES
    0,0,0,0,0,0,0,0,0,0,0
#endif
};
#endif

//=============================================================================

FLAG
SetRegAsSymbol(char *string, ULONG v)
{
	int		i;
	char	s[10];
	char 	*sPtr,*sPtr2;

	sPtr = string;
	sPtr2 = s;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses"
	while ( *sPtr2++ = toupper(*sPtr++));
#pragma GCC diagnostic pop

	for ( i = 0 ; i<NUMREGS; i++)
	 {
		if(!strcmp(regNameArray[i],s))
		 {
			SetReg(i,v);
			return boolean::TRUE;
		 }
#ifdef GENESIS
		if(!strcmp("SP",s))
	 	 {
			SetReg(REG_A7,v);
			return boolean::TRUE;
	 	 }
#endif
	}
	return boolean::FALSE;
}

//=============================================================================
// kts note: work on a7/sp/usp/ssp stuff

ULONG
GetRegAsSymbol(char *string)
{
    int i;
//	UpCaseString(string);
    for(i=0;i<NUMREGS; i++)
     {
        if(!stricmp(regNameArray[i],string))
            return(regs[i]);
     }
#ifdef GENESIS
	if(!stricmp("SP",string))
		return(GetSP());
#endif
    return(OUT_OF_BAND);
}

//=============================================================================

void
AddRegGadgets(_window *pWindow)
{
#ifdef GENESIS
    MakeGadget(pWindow, 4,1,8,1,RGGAD_D0);
    MakeGadget(pWindow, 4,2,8,1,RGGAD_D1);
    MakeGadget(pWindow, 4,3,8,1,RGGAD_D2);
    MakeGadget(pWindow, 4,4,8,1,RGGAD_D3);
    MakeGadget(pWindow, 4,5,8,1,RGGAD_D4);
    MakeGadget(pWindow, 4,6,8,1,RGGAD_D5);
    MakeGadget(pWindow, 4,7,8,1,RGGAD_D6);
    MakeGadget(pWindow, 4,8,8,1,RGGAD_D7);
    MakeGadget(pWindow,16,1,8,1,RGGAD_A0);
    MakeGadget(pWindow,16,2,8,1,RGGAD_A1);
    MakeGadget(pWindow,16,3,8,1,RGGAD_A2);
    MakeGadget(pWindow,16,4,8,1,RGGAD_A3);
    MakeGadget(pWindow,16,5,8,1,RGGAD_A4);
    MakeGadget(pWindow,16,6,8,1,RGGAD_A5);
    MakeGadget(pWindow,16,7,8,1,RGGAD_A6);
    MakeGadget(pWindow,16,8,8,1,RGGAD_A7);
    MakeGadget(pWindow,16,9,8,1,RGGAD_USP);
    MakeGadget(pWindow, 4,9,8,1,RGGAD_PC);
    MakeGadget(pWindow, 5,10,16,1,RGGAD_SR);
#endif

#ifdef SNES
#define	REGA_X	3
#define	REGA_Y	1
#define	REGX_X	3
#define	REGX_Y	2
#define	REGY_X	3
#define	REGY_Y	3
#define	REGFLAGS_X	17
#define	REGFLAGS_Y	1
#define	REGXFLAGS_X	24
#define	REGXFLAGS_Y	2
#define	REGD_X	10
#define	REGD_Y	1
#define	REGDB_X	11
#define	REGDB_Y	2
#define	REGPB_X	11
#define	REGPB_Y	3
#define	REGSP_X	18
#define	REGSP_Y	2
#define	REGPC_X	18
#define	REGPC_Y	3

	MakeGadget(pWindow,REGA_X,REGA_Y,4,1,RGGAD_A);		   		// register a
	MakeGadget(pWindow,REGX_X,REGX_Y,4,1,RGGAD_X);				// register x
	MakeGadget(pWindow,REGY_X,REGY_Y,4,1,RGGAD_Y);				// register y
	MakeGadget(pWindow,REGFLAGS_X,REGFLAGS_Y,8,1,RGGAD_FLAGS);	// flags
	MakeGadget(pWindow,REGXFLAGS_X,REGXFLAGS_Y,1,1,RGGAD_XFLAGS); // extended flags
	MakeGadget(pWindow,REGD_X,REGD_Y,4,1,RGGAD_D);				// register d
	MakeGadget(pWindow,REGDB_X,REGDB_Y,2,1,RGGAD_DB);			// register db
	MakeGadget(pWindow,REGPB_X,REGPB_Y,2,1,RGGAD_PB);			// register pb
	MakeGadget(pWindow,REGPC_X,REGPC_Y,4,1,RGGAD_PC);			// register pc
	MakeGadget(pWindow,REGSP_X,REGSP_Y,4,1,RGGAD_SP);			// register sp
#endif

}

//=============================================================================

char *
PrintFlags(char *buffer,UWORD flags)
{
#ifdef GENESIS
    *buffer++ = ((flags & 0x8000)?'-':'-');
    *buffer++ = ((flags & 0x4000)?'-':'-');
    *buffer++ = ((flags & 0x2000)?'S':'s');
    *buffer++ = ((flags & 0x1000)?'-':'-');
    *buffer++ = ((flags & 0x0800)?'-':'-');
    *buffer++ = ((flags & 0x0400)?'I':'i');
    *buffer++ = ((flags & 0x0200)?'I':'i');
    *buffer++ = ((flags & 0x0100)?'I':'i');
    *buffer++ = ((flags & 0x0080)?'-':'-');
    *buffer++ = ((flags & 0x0040)?'-':'-');
    *buffer++ = ((flags & 0x0020)?'-':'-');
    *buffer++ = ((flags & 0x0010)?'X':'x');
    *buffer++ = ((flags & 0x0008)?'N':'n');
    *buffer++ = ((flags & 0x0004)?'Z':'z');
    *buffer++ = ((flags & 0x0002)?'V':'v');
    *buffer++ = ((flags & 0x0001)?'C':'c');
#endif
#ifdef SNES
	if(regs[REG_XFLAGS])
	 {
		*buffer++ = ((flags & 0x80)?'N':'n');
		*buffer++ = ((flags & 0x40)?'V':'v');
		*buffer++ = ((flags & 0x20)?'-':'-');
		*buffer++ = ((flags & 0x10)?'B':'b');
		*buffer++ = ((flags & 0x08)?'D':'d');
		*buffer++ = ((flags & 0x04)?'I':'i');
		*buffer++ = ((flags & 0x02)?'Z':'z');
		*buffer++ = ((flags & 0x01)?'C':'c');
	 }
	else
	 {
		*buffer++ = ((flags & 0x80)?'N':'n');
		*buffer++ = ((flags & 0x40)?'V':'v');
		*buffer++ = ((flags & 0x20)?'M':'m');
		*buffer++ = ((flags & 0x10)?'X':'x');
		*buffer++ = ((flags & 0x08)?'D':'d');
		*buffer++ = ((flags & 0x04)?'I':'i');
		*buffer++ = ((flags & 0x02)?'Z':'z');
		*buffer++ = ((flags & 0x01)?'C':'c');
	 }

#endif
    return(buffer);
}

//=============================================================================

char regText[] =
#ifdef GENESIS
"D0:         A0:        \n"
"D1:         A1:        \n"
"D2:         A2:        \n"
"D3:         A3:        \n"
"D4:         A4:        \n"
"D5:         A5:        \n"
"D6:         A6:        \n"
"D7:         A7:        \n"
"PC:         US:        \n"
"SR:";
#endif

#ifdef SNES
"A:     D:     P:\nX:     DB:    SP:\nY:     PB:    PC:";
#endif


void RegRoutine(_object *oPtr)
{
    unsigned int x,y,seg,pWidth;
    unsigned int *addr;
    _layer *lPtr;

    lPtr = oPtr->layer;

#ifdef GENESIS
    *PrintRaw32Bits(textBuffer,regs[REG_D0]) = 0;
    PrintLay(lPtr,textBuffer,4,1);
    *PrintRaw32Bits(textBuffer,regs[REG_D1]) = 0;
    PrintLay(lPtr,textBuffer,4,2);
    *PrintRaw32Bits(textBuffer,regs[REG_D2]) = 0;
    PrintLay(lPtr,textBuffer,4,3);
    *PrintRaw32Bits(textBuffer,regs[REG_D3]) = 0;
    PrintLay(lPtr,textBuffer,4,4);
    *PrintRaw32Bits(textBuffer,regs[REG_D4]) = 0;
    PrintLay(lPtr,textBuffer,4,5);
    *PrintRaw32Bits(textBuffer,regs[REG_D5]) = 0;
    PrintLay(lPtr,textBuffer,4,6);
    *PrintRaw32Bits(textBuffer,regs[REG_D6]) = 0;
    PrintLay(lPtr,textBuffer,4,7);
    *PrintRaw32Bits(textBuffer,regs[REG_D7]) = 0;
    PrintLay(lPtr,textBuffer,4,8);

    *PrintRaw32Bits(textBuffer,regs[REG_PC]) = 0;
    PrintLay(lPtr,textBuffer,4,9);

    *PrintRaw32Bits(textBuffer,regs[REG_A0]) = 0;
    PrintLay(lPtr,textBuffer,16,1);
    *PrintRaw32Bits(textBuffer,regs[REG_A1]) = 0;
    PrintLay(lPtr,textBuffer,16,2);
    *PrintRaw32Bits(textBuffer,regs[REG_A2]) = 0;
    PrintLay(lPtr,textBuffer,16,3);
    *PrintRaw32Bits(textBuffer,regs[REG_A3]) = 0;
    PrintLay(lPtr,textBuffer,16,4);
    *PrintRaw32Bits(textBuffer,regs[REG_A4]) = 0;
    PrintLay(lPtr,textBuffer,16,5);
    *PrintRaw32Bits(textBuffer,regs[REG_A5]) = 0;
    PrintLay(lPtr,textBuffer,16,6);
    *PrintRaw32Bits(textBuffer,regs[REG_A6]) = 0;
    PrintLay(lPtr,textBuffer,16,7);
    *PrintRaw32Bits(textBuffer,regs[REG_A7]) = 0;
    PrintLay(lPtr,textBuffer,16,8);

    *PrintRaw32Bits(textBuffer,regs[REG_USP]) = 0;
    PrintLay(lPtr,textBuffer,16,9);

    *PrintFlags(textBuffer,regs[REG_SR]) = 0;
    PrintLay(lPtr,textBuffer,5,10);
#endif

#ifdef SNES
	*Print16Bits(textBuffer,regs[REG_A]) = 0;		// zero terminate string
	PrintLay(lPtr,textBuffer,REGA_X,REGA_Y);
	*Print16Bits(textBuffer,regs[REG_X]) = 0;
	PrintLay(lPtr,textBuffer,REGX_X,REGX_Y);
	*Print16Bits(textBuffer,regs[REG_Y]) = 0;
	PrintLay(lPtr,textBuffer,REGY_X,REGY_Y);
	*Print16Bits(textBuffer,regs[REG_D]) = 0;
	PrintLay(lPtr,textBuffer,REGD_X,REGD_Y);
	*Print8Bits(textBuffer,regs[REG_DB]) = 0;
	PrintLay(lPtr,textBuffer,REGDB_X,REGDB_Y);
	*Print8Bits(textBuffer,regs[REG_PB]) = 0;
	PrintLay(lPtr,textBuffer,REGPB_X,REGPB_Y);
	*PrintFlags(textBuffer,regs[REG_FLAGS]) = 0;				// zero terminate string
	PrintLay(lPtr,textBuffer,REGFLAGS_X,REGFLAGS_Y);
	*Print16Bits(textBuffer,regs[REG_SP]) = 0;
	PrintLay(lPtr,textBuffer,REGSP_X,REGSP_Y);
	*PrintXFlags(textBuffer,regs[REG_XFLAGS]) = 0;
	PrintLay(lPtr,textBuffer,REGXFLAGS_X,REGXFLAGS_Y);
	*Print16Bits(textBuffer,regs[REG_PC]) = 0;
	PrintLay(lPtr,textBuffer,REGPC_X,REGPC_Y);
#endif
}

//=============================================================================

#ifdef SNES
void
LoadPCL(void)
{
	regs[REG_PCL] = regs[REG_PC] | (regs[REG_PB]<<16);
}

void
SavePCL(void)
{
	regs[REG_PC] = regs[REG_PCL] & regMaskArray[REG_PC];
	regs[REG_PB] = (regs[REG_PCL]>>16) & regMaskArray[REG_PB];
}

#endif

//==============================================================================

ULONG
GetReg(int reg)
{
    return(regs[reg] & regMaskArray[reg]);
}

//==============================================================================

#define SUP_MODE	0x2000

ULONG
GetSP( void )
	{
#ifdef GENESIS
	return( GetReg(REG_SR) & SUP_MODE ? GetReg(REG_SSP) : GetReg(REG_USP) );
#endif

#ifdef SNES
	return( GetReg(REG_SP) );
#endif
	}

//==============================================================================

void
SetReg(int reg,ULONG value)
{
    if(slaveUpdate)
	 {
		GetRegsFromSlave(regs);
		delay(10);
	 }
    regs[reg] = value & regMaskArray[reg];
#ifdef SNES
	if(reg == REG_PC || reg == REG_PB)
		LoadPCL();
	else if(reg == REG_PCL)
		SavePCL();
#endif
    if(slaveUpdate)
        PutRegsToSlave(regs);
}

//=============================================================================

void
RegEditRout(ULONG value,void *dataPtr)
{
    _gadget *gPtr;
    gPtr = (_gadget *)dataPtr;
    SetReg(gPtr->gNum-RGGAD_BASE,value);
}

//=============================================================================

FLAG
RegInput(_input *in,_object *oPtr)
{
    _window *pWindow;
    FLAG inputUsed;
    unsigned char far *chr;
    _gadget *gPtr;

    pWindow = (_window *)oPtr->layer;
    inputUsed = boolean::TRUE;

    switch(in->inputType)
     {
        case INP_KEY:
            switch(in->fullKey)
             {
				case CMD_CLOSEWINDOW:
					delete pWindow;
					delete oPtr;
					regObjPtr = NULL;
					regOpen = boolean::FALSE;
                    break;
				case CMD_OPENWINDOWHELPFILE:
					OpenRefFile(EXT_HELP,HELPNAME,"Register:");
					break;
                default:
                    inputUsed = boolean::FALSE;
                    break;
             }
            break;
        case INP_MOUSE_LEFTBUTTON_DOWN:
            if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
             {
                switch(gPtr->gNum)
                 {
                    case 1:
						delete pWindow;
						delete oPtr;
						regObjPtr = NULL;
						regOpen = boolean::FALSE;
                        inputUsed = boolean::TRUE;
                        break;
#ifdef GENESIS
                    case RGGAD_A0:
                    case RGGAD_A1:
                    case RGGAD_A2:
                    case RGGAD_A3:
                    case RGGAD_A4:
                    case RGGAD_A5:
                    case RGGAD_A6:
                    case RGGAD_A7:
                    case RGGAD_D0:
                    case RGGAD_D1:
                    case RGGAD_D2:
                    case RGGAD_D3:
                    case RGGAD_PC:
                    case RGGAD_D4:
                    case RGGAD_D5:
                    case RGGAD_D6:
                    case RGGAD_D7:
                    case RGGAD_USP:
                    case RGGAD_SR:

#endif
#ifdef SNES
					case RGGAD_A:
					case RGGAD_X:
					case RGGAD_Y:
					case RGGAD_FLAGS:
					case RGGAD_XFLAGS:
					case RGGAD_PC:
					case RGGAD_D:
					case RGGAD_DB:
					case RGGAD_PB:
					case RGGAD_SP:
#endif
						sprintf(expString,"$%lx",GetReg(gPtr->gNum-RGGAD_BASE));
                        PadString(expString,EXPRLEN);
                        sprintf(regExprTitle,"Enter new value for register %s",regNameArray[gPtr->gNum-RGGAD_BASE]);
                        GetExpr(RegEditRout,gPtr,regExprTitle,oPtr->layer);
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
OpenReg(void)
{
    _object *oPtr;
    _window *pWindow;


	if(regOpen)
	{
		assert( regObjPtr );
		assert( regObjPtr->layer );
		regObjPtr->layer->ToFront();
		ActivateFrontWindow();
		return(boolean::TRUE);
	}

    oPtr = AddObject(RegRoutine);
#ifdef GENESIS
    pWindow = new _window(0 ,1,25,12,"Register",regAttr,regChar);
#endif
#ifdef SNES
    pWindow = new _window(0 ,1,26,5,"Register",regAttr,regChar);
#endif

    if(pWindow)
     {
		regOpen = boolean::TRUE;
		regObjPtr = oPtr;
        pWindow->data = (void *)oPtr;
        oPtr->inputRoutine = RegInput;
        oPtr->layer = pWindow;
        oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;

        PrintLay(pWindow,regText,1,1);
        AddSysGadgets(pWindow);
        AddRegGadgets(pWindow);
        DrawGadgets(pWindow);
        ActivateFrontWindow();
     }
    else
        PrintError(ERROR_NOMEM);
    return(boolean::TRUE);                           // in case activated from menu
}

//=============================================================================
