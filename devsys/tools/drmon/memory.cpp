//=============================================================================
//  memory.c: memory window
//=============================================================================
// Memory object structure usage:
// addr contains the address we are looking at
// dataPtr2-> dynamic expression, or == NULL if none
// longData = addr(in update)
// longData2 = address to change in dohex
//=============================================================================

#define data1 mType
#define data3 pLabels
#define data4 lineInc
#define data5 mode816Size
#define data6 disMode
#define data7 updateMode

//==============================================================================

#include "moninc.hpp"
#include "help.hpp"
#include "display.hpp"
#include "word.hpp"
#include "express.hpp"
#include "memory.hpp"

//=============================================================================
#define ADDR_SIZE 6

#ifdef GENESIS
#define OPCODE_SIZE 2
#endif

#ifdef SNES
#define OPCODE_SIZE 1
#endif

//==============================================================================
// size of data to write to memory depending on display mode

int memorySize[] =
{
	OPCODE_SIZE,1,2,4,1,                        // dis, byte, word, long, ascii
#ifdef GENESIS
	2,2,2,                                                        // 3 vdp modes
#endif
#ifdef SNES
	2,                                                                // 1 ppu mode
#endif
	2                                                                // diff mode(not implemented)
};

int dWidthTbl[] =
{
	2,3,5,9,1,
	5,5,5,5
};

//=============================================================================


//==============================================================================
// data1 = memory dump type:

enum
{
	MTYPE_DISS,
	MTYPE_BYTE,
	MTYPE_WORD,
	MTYPE_LONG,
	MTYPE_ASCII,
#ifdef GENESIS
	MTYPE_VDP,
	MTYPE_VDPCO,
	MTYPE_VDPVS,
#endif
#ifdef SNES
	MTYPE_PPU,
#endif
	MTYPE_DIFF
};

//==============================================================================

char *mTypeText[] =
{
	"Disassembly\xC4",
	"Byte\xC4",
	"Word\xC4",
	"Long\xC4",
	"ASCII\xC4",
#ifdef GENESIS
	"VDP\xC4",
	"VDP Color\xC4",
	"VDP VScroll\xC4",
#endif
#ifdef SNES
	"PPU ",
#endif
	0
};

char *mSizeText[] =
{
	"Byte\xC4",
	"Word\xC4",
	"Long\xC4"
};

char *updText[] =
{
	"Static->",
	"DynamicR->",
	"Dynamic->"
};

// pLabels = print labels: 0 = no, 1 = just do lines, 2 = just do parameters, 3 = yes

// data4 = # of bytes to add to move forward one line

// data5 = dissasemble mode:(snes only)
//        0          Source
//        1          Automatic
//        2          Emulation
//        3           8A  8XY
//        4           8A 16XY
//        5          16A  8XY
//        6          16A 16XY

// data6 = diss mode
//        0 = automatic
//  1 = code only
//  2 = addr & code
//  3 = addr, hex & code

// data7 = update mode:
//        0 = static
//  1 = stynamic(dynamic while running)
//  2 = dynamic


enum
{
	UPDATE_STATIC,
	UPDATE_DYNAMICRUNNING,
	UPDATE_DYNAMIC
};

//=============================================================================

#define        DGAD_UP        GAD_USER+1
#define DGAD_DOWN DGAD_UP+1
#define DGAD_SCROLL DGAD_DOWN+1
#define DGAD_SBOX DGAD_SCROLL+1

#ifdef SNES
int        procMode = 0;
#define MEMORY_HEXWIDTH 37
#define MEMORY_ADDRWIDTH 26
#define BYTESPERLINE 4
#define MEMORYMINWIDTH 4
#endif

#ifdef GENESIS
#define MEMORYMINWIDTH 4
#define BYTESPERLINE 10
#define MEMORY_HEXWIDTH 66
#define MEMORY_ADDRWIDTH 41
#endif

//=============================================================================

FLAG
MemoryType(_menuItem *iPtr,_object *oPtr,int choice)
{
	_layer *lPtr;
	oPtr->mType = choice;
	lPtr = oPtr->layer;
	DrawBottomBorder(lPtr,lPtr->border,lPtr->xSize,lPtr->ySize);                        // just in case status line changes
	return(boolean::TRUE);
}

FLAG
MemoryDiss(_menuItem *iPtr,_object *oPtr,int choice)
{
	oPtr->disMode = choice;
	return(boolean::TRUE);
}

FLAG
MemoryLabel(_menuItem *iPtr,_object *oPtr,int choice)
{
	oPtr->pLabels = choice;
	return(boolean::TRUE);
}

//=============================================================================

void
MemoryGotoDRout(ULONG memoryAddr,void *dataPtr)
	{
	_object* oPtr;
	char* string;

	UnPadString( expString );

	oPtr = (_object*)dataPtr;
	if ( string = (char*)malloc( strlen( expString ) + 1 ) )
		{
		if ( oPtr->dataPtr2 )
			free( oPtr->dataPtr2 ), oPtr->dataPtr2 = NULL;
	    oPtr->dataPtr2 = string;
	    strcpy( string, expString );
	    oPtr->addr = memoryAddr;
	    oPtr->updateMode = UPDATE_DYNAMIC;
	 	}
	}

FLAG
MemoryGotoDynamic(_menuItem *iPtr,_object *oPtr,int choice)
{
	if(oPtr->dataPtr2)
	        strcpy(expString,oPtr->dataPtr2);
	else
	        *expString = '\0';
	PadString(expString,EXPRLEN);
	GetExpr(MemoryGotoDRout,oPtr,"Enter New Dynamic Memory Address",oPtr->layer);
	return(boolean::TRUE);
}


void
MemoryGotoDRoutRunning(ULONG memoryAddr,void *dataPtr)
{
	_object *oPtr;
	char *string;

	oPtr = (_object *)dataPtr;
	UnPadString(expString);
	if(oPtr->dataPtr2)
	        free(oPtr->dataPtr2);
	oPtr->dataPtr2 = NULL;
	string = (char *)malloc(strlen(expString)+1);
	if(string)
	 {
	        oPtr->dataPtr2 = string;
	        strcpy(string,expString);
	        oPtr->addr = memoryAddr;
	        oPtr->updateMode = UPDATE_DYNAMICRUNNING;
	        oPtr->longData = 0xffffffff;                                // force re-calc
	 }
}

FLAG
MemoryGotoDynamicRunning(_menuItem *iPtr,_object *oPtr,int choice)
{
	if(oPtr->dataPtr2)
	        strcpy(expString,oPtr->dataPtr2);
	else
	        *expString = 0;
	PadString(expString,EXPRLEN);
	GetExpr(MemoryGotoDRoutRunning,oPtr,"Enter New Dynamic Running Memory Address",oPtr->layer);
	return(boolean::TRUE);
}


void
MemoryGotoSRout(ULONG memoryAddr,void *dataPtr)
{
	_object *oPtr;
	oPtr = (_object *)dataPtr;
	if(oPtr->dataPtr2)
	        free(oPtr->dataPtr2);
	oPtr->dataPtr2 = NULL;
	oPtr->addr = memoryAddr;
	oPtr->updateMode = UPDATE_STATIC;
}

FLAG
MemoryGotoStatic(_menuItem *iPtr,_object *oPtr,int choice)
{
	oPtr->updateMode = choice;
	sprintf(expString,"$%lx",oPtr->addr);
	PadString(expString,EXPRLEN);
	GetExpr(MemoryGotoSRout,oPtr,"Enter New Static Memory Address",oPtr->layer);
	return(boolean::TRUE);
}

//=============================================================================

menuItems memoryTypeMenu[] =
{
	{"Co&de        Ctrl-D",MemoryType,0},
	{"&Byte        Ctrl-B",MemoryType,0},
	{"&Word        Ctrl-W",MemoryType,0},
	{"&Long        Ctrl-L",MemoryType,0},
	{"Ascii       Ctrl-Y",MemoryType,0},
#ifdef GENESIS
	{"VDP        ",MemoryType,0},
	{"VDP Color  ",MemoryType,0},
	{"VDP VScroll",MemoryType,0},
#endif
#ifdef SNES
	{"PPU        ",MemoryType,0},
#endif
	{0,0,0}
};

menuItems memoryDissMenu[] =
{
	{"Automatic       ",MemoryDiss,0},
	{"Code only       ",MemoryDiss,0},
	{"Addr & code     ",MemoryDiss,0},
	{"Addr, hex & code",MemoryDiss,0},
	{0,0,0}
};

menuItems memoryLabelMenu[] =
{
	{"All Labels Off",MemoryLabel,0},
	{"Line Labels Only",MemoryLabel,0},
	{"Parameter Labels Only",MemoryLabel,0},
	{"All Labels On ",MemoryLabel,0},
	{0,0,0}
};

//=============================================================================

FLAG
DoMemoryTypeMenu(_menuItem *iPtr,_object *oPtr,int choice)
{

	DoSubMenu(iPtr,oPtr,memoryTypeMenu,"Type");
	return(boolean::FALSE);
}

//=============================================================================

FLAG
DoMemoryDissMenu(_menuItem *iPtr,_object *oPtr,int choice)
{
	DoSubMenu(iPtr,oPtr,memoryDissMenu,"Dis Mode");
	return(boolean::FALSE);
}

//=============================================================================

menuItems memoryGotoMenu[] =
{
	{"&Static...                ",MemoryGotoStatic,0},
	{"Dynamic Runnin&g... Ctrl-G",MemoryGotoDynamicRunning,0},
	{"&Dynamic...               ",MemoryGotoDynamic,0},
	{0,0,0}
};

//=============================================================================

FLAG
DoMemoryGotoMenu(_menuItem *iPtr,_object *oPtr,int choice)
{
	DoSubMenu(iPtr,oPtr,memoryGotoMenu,"Updates");
	return(boolean::FALSE);
}

//=============================================================================

FLAG
DoMemoryLabelMenu(_menuItem *iPtr,_object *oPtr,int choice)
{
	DoSubMenu(iPtr,oPtr,memoryLabelMenu,"Labels");
	return(boolean::FALSE);
}

//=============================================================================

FLAG
DoMemOpsMenu(_menuItem *iPtr,_object *oPtr,int choice)
{
	DoSubMenu(iPtr,oPtr,memOpsMenu,"MemOps");
	return(boolean::FALSE);
}


//============================================================================

#ifdef SNES

FLAG
MemoryDissFlags(_menuItem *iPtr,_object *oPtr,int choice)
{
	oPtr->mode816Size = choice;
	return(boolean::TRUE);
}

menuItems memoryDisFlagsMenu[] =
{
	{"Source   ",MemoryDissFlags,0},
	{"Automatic",MemoryDissFlags,0},
	{"Emulation",MemoryDissFlags,0},
	{" 8A  8XY ",MemoryDissFlags,0},
	{" 8A 16XY ",MemoryDissFlags,0},
	{"16A  8XY ",MemoryDissFlags,0},
	{"16A 16XY ",MemoryDissFlags,0},
	{0,0,0}
};

//============================================================================

FLAG
DoMemoryDisFlagsMenu(_menuItem *iPtr,_object *oPtr,int choice)
{
	DoSubMenu(iPtr,oPtr,memoryDisFlagsMenu,"Dis Flags");
	return(boolean::FALSE);
}

int
GetProcMode(unsigned long addr, int menuMode)
{
	ULONG temp;
	switch(menuMode)
	 {
	        case 0:                                // source mode
	                return(0);                // for now
	        case 1:                                // automatic(Reads from processor flags)
	                if(GetReg(REG_XFLAGS))
	                        return(0);
	                temp = GetReg(REG_FLAGS);
	                return((((temp >> 5) & 1) | ((temp >> 3) & 2)) ^ 0x3);
	        case 2:
	                return(0);                // emulation mode
	        case 3:
	                return(0);
	        case 4:
	                return(2);
	        case 5:
	                return(1);
	        case 6:
	                return(3);
	 }
	return(0);
}

#endif

//=============================================================================

menuItems memoryMenu[] =
{
	{"Memory Address >>",DoMemoryGotoMenu,   0},
	{"Memory Type    >>",DoMemoryTypeMenu,   0},
	{"Disp Labels    >>",DoMemoryLabelMenu,  0},
	{"Dis Mode       >>",DoMemoryDissMenu,   0},
	{"MemOps         >>",DoMemOpsMenu,       0},
#ifdef SNES
	{"Dis Flags      >>",DoMemoryDisFlagsMenu,0},
#endif
	{0,0,0}
};

//=============================================================================

void
AddMemoryGadgets(_window* pWindow)
{
	AddVScrollGadgets(pWindow,DGAD_UP,DGAD_DOWN,DGAD_SCROLL,DGAD_SBOX);
}

//=============================================================================

void
MemoryWriteASCII(char *string,void *dataPtr)
{
	_object *oPtr;
	oPtr = (_object *)dataPtr;

	UnPadString(string);
	WriteSlaveData((ULONG)oPtr->longData2,(char far *)string,strlen(string));
}

//=============================================================================

void
MemoryWriteHex(ULONG value,void *dataPtr)
{
	ULONG tempVal;
	_object *oPtr;

	oPtr = (_object *)dataPtr;
	tempVal = value;

	switch(memorySize[oPtr->mType])
	        {
	        case 1:
	                WriteSlaveData((ULONG)(oPtr->longData2),(char far *)&tempVal,1);
	                oPtr->longData2++;
	                break;
	        case 2:
#if WORDSWAP
	                tempVal = SwapWord(tempVal);
#endif
	                WriteSlaveData((ULONG)(oPtr->longData2),(char far *)&tempVal,2);
	                oPtr->longData2+= 2;
	                break;
	        case 4:
#if WORDSWAP
	                tempVal = SwapLong(tempVal);
#endif
	                WriteSlaveData((ULONG)(oPtr->longData2),(char far *)&tempVal,4);
	                oPtr->longData2+= 4;
	                break;
	        }
}

//=============================================================================

void
MemoryCommand(char **s,_object *oPtr, errorcode *error)
{
	ULONG memoryAddr;
	unsigned int word,symbolFlag;
	FLAG        dynamicFlag;
	char        *string;

	dynamicFlag = 0;

	*error = NOERR;
	while (*s && **s && !*error)
	{
	        *s = ParseWord(*s, &word);
	        switch (word & 0xff)
	        {
#ifdef GENESIS
	                case        RW_VDP:
	                        oPtr->mType = MTYPE_VDP;
	                        break;

	                case        RW_VSR:
	                        oPtr->mType = MTYPE_VDPVS;
	                        break;

	                case        RW_VCR:
	                        oPtr->mType = MTYPE_VDPCO;
	                        break;
#endif
#ifdef SNES
	                case        RW_PPU:
	                        oPtr->mType = MTYPE_PPU;
	                        break;
#endif
	                case        RW_CODE:
	                        oPtr->mType = MTYPE_DISS;
	                        break;

	                case        RW_ASCII:
	                        oPtr->mType = MTYPE_ASCII;
	                        break;

	                case        RW_BYTE:
	                        oPtr->mType = MTYPE_BYTE;
	                        break;

	                case        RW_WORD:
	                        oPtr->mType = MTYPE_WORD;
	                        break;

	                case        RW_LONG:
	                        oPtr->mType = MTYPE_LONG;
	                        break;

	                case        RW_DYNAMIC:
	                        dynamicFlag = boolean::TRUE;
	                        oPtr->updateMode = UPDATE_DYNAMIC;
	                        break;

	                case        RW_DYNAMICRUNNING:
	                        dynamicFlag = boolean::TRUE;
	                        oPtr->updateMode = UPDATE_DYNAMICRUNNING;
	                        oPtr->longData = 0xffffffff;                                // force re-calc
	                        break;

	                case        RW_STATIC:
	                        dynamicFlag = boolean::FALSE;
	                        oPtr->updateMode = UPDATE_STATIC;
	                        oPtr->dataPtr2 = NULL;

	                        break;

	                case        RW_SYMBOL:
	                        *s = ParseWord(*s, &symbolFlag);
	                        switch (symbolFlag & 0xff)
	                        {
	                                case        RW_OFF:
	                                        oPtr->pLabels = 0;
	                                        break;
	                                case        RW_LINE:
	                                        oPtr->pLabels = 1;
	                                        break;
	                                case        RW_PARAM:
	                                        oPtr->pLabels = 2;
	                                        break;
	                                case        RW_ON:
	                                        oPtr->pLabels = 3;
	                                        break;
	                                default:
	                                        *error = ERROR_SYNTAX;
	                                        break;
	                        }
	                        break;

	                default:
	                        {
	                                strcpy(textBuffer,*s);        /* save a copy */
	                                *s = GetNumbers(*s,1,&memoryAddr,error);
	                                if (!*error)
	                                {
	                                        if(oPtr->dataPtr2)                                // free the old one first
	                                                free(oPtr->dataPtr2);
	                                        oPtr->dataPtr2 = NULL;

	                                        if(dynamicFlag)
	                                        {
	                                                string = (char *)malloc(strlen(textBuffer)+1);
	                                                if(string)
	                                                 {
	                                                        dynamicFlag = boolean::FALSE;
	                                                        oPtr->dataPtr2 = string;
	                                                        strcpy(string,textBuffer);
	                                                        oPtr->addr = memoryAddr;
	                                                 }
	                                        }
	                                        else
	                                                oPtr->addr = memoryAddr;
	                                }
	                        }

	                        break;
	        }
	}
	if (dynamicFlag)
	        *error = ERROR_MISSINGPARAM;
	return;
}

//=============================================================================

FLAG
MemoryInput(_input *in,_object *oPtr)
{
    _window *pWindow;
	_menu *mPtr;
	_menuItem *iPtr;
	_gadget  *gPtr;
	char *s;

	ULONG temp;
	unsigned int xPos,yPos;
	int pWidth;

    FLAG inputUsed;
	FLAG changed = boolean::FALSE;
    unsigned char far *chr;

    pWindow = (_window *)oPtr->layer;
    inputUsed = boolean::TRUE;
	switch(in->inputType)
	 {
			case INP_WINDOW_MESSAGE:
				switch(in->message)
				 {
					case MEMORY_DOWNLINE:
						if(pWindow->cursorY >= pWindow->ySize-3)
							oPtr->addr+=oPtr->lineInc;
						else
							pWindow->cursorY++;
						changed = boolean::TRUE;
						break;
					case MEMORY_UPLINE:
	                    if(pWindow->cursorY > 0)
                            pWindow->cursorY--;
	                    else
                            oPtr->addr-=oPtr->lineInc;
	                    changed = boolean::TRUE;
						break;
					case MEMORY_PGDWN:
	                    oPtr->addr += oPtr->lineInc*(oPtr->layer->ySize-BORDER_Y);
	                    changed = boolean::TRUE;
						break;
					case MEMORY_PGUP:
	                    oPtr->addr -= oPtr->lineInc*(oPtr->layer->ySize-BORDER_X);
	                    changed = boolean::TRUE;
						break;
					case MEMORY_DOWN:
	                    oPtr->addr -= OPCODE_SIZE;                        // move throught memory word(or byte) at a time
						break;
					case MEMORY_UP:
	                    oPtr->addr += OPCODE_SIZE;
						break;
					case MEMORY_RUNTOHERE:
	                    AddBrkPt(MemoryGetLineAddr(oPtr,oPtr->layer->cursorY),1,NULL);
	                    DoSlaveRun();
						break;
					case MEMORY_SETBREAKHERE:
	                    AddBrkPt(MemoryGetLineAddr(oPtr,oPtr->layer->cursorY),0,NULL);
						break;
					case MEMORY_SETBREAKONCEHERE:
                        AddBrkPt(MemoryGetLineAddr(oPtr,oPtr->layer->cursorY),1,NULL);
						break;
					case MEMORY_SETBREAKCOUNTHERE:
                        BreakSetGetCount(MemoryGetLineAddr(oPtr,oPtr->layer->cursorY),oPtr);
						break;
					case MEMORY_CLEARALLBREAKS:
                        BreakClearAll();
						break;
					case MEMORY_CLEARBREAK:
                        BreakClear(MemoryGetLineAddr(oPtr,oPtr->layer->cursorY));
						break;
					case MEMORY_CHANGEMEM:
	                    oPtr->longData2 = MemoryGetLineAddr(oPtr,oPtr->layer->cursorY);
	                    switch(oPtr->mType)                                                // which dump type are we
	                        {
	                            case MTYPE_DISS:
	                            case MTYPE_BYTE:
	                            case MTYPE_WORD:
	                            case MTYPE_LONG:
	                                    sprintf(textBuffer,"Enter New Data for $%lx",oPtr->longData2);
	                                    GetMultiExpr(MemoryWriteHex,oPtr,textBuffer,oPtr->layer);
	                                    break;
	                            case MTYPE_ASCII:
	                                    GetString(MemoryWriteASCII,(void *)oPtr,"Enter New Text",oPtr->layer);
	                                    break;
	                        }
						break;
					case MEMORY_BYTE:
						oPtr->mType = MTYPE_BYTE;
						break;
					case MEMORY_WORD:
						oPtr->mType = MTYPE_WORD;
						break;
					case MEMORY_LONG:
						oPtr->mType = MTYPE_LONG;
						break;
					case MEMORY_CODE:
						oPtr->mType = MTYPE_DISS;
						break;
					case MEMORY_ASCII:
						oPtr->mType = MTYPE_ASCII;
						break;
					case MEMORY_GOTO:
		                MemoryGotoDynamicRunning(NULL,oPtr,0);
						break;

					case MEMORY_PROCHELP:
	                    oPtr->longData2 = MemoryGetLineAddr(oPtr,oPtr->layer->cursorY);
	                    ReadSlaveData(oPtr->longData2,xferBuffer,BYTESPERLINE);
	                    Disassem(oPtr->longData2,xferBuffer,textBuffer,0);                 // just to get length of op-code

	                    s = textBuffer;
	                    while(*s && *s != '.' && *s != ' ')
	                            s++;
	                    *s = 0;
	                    s = DupString(textBuffer);
	                    OpenRefFile(EXT_PROCESSOR,PROGNAME,s);
	                    free(s);
						break;
				 }
				break;

	        case INP_KEY:
					switch(in->fullKey)
	                  {
						case CMD_OPENWINDOWHELPFILE:
							OpenRefFile(EXT_HELP,HELPNAME,"Memory:");
							break;
						case CMD_CLOSEWINDOW:
							delete pWindow;
							delete oPtr;
							inputUsed = boolean::TRUE;
							break;
						case CMD_LOCALMENU:
							mPtr = CreateMenuWithItems(memoryMenu,"Memory");
							mPtr->xPos = AutoPosition(pWindow->xPos,pWindow->xSize,mPtr->xSize,displayWidth);
							mPtr->yPos = AutoPosition(pWindow->yPos,pWindow->ySize,mPtr->ySize,displayHeight);
							DoMenu(mPtr,oPtr);
							changed = boolean::TRUE;
							break;
						default:
							inputUsed = boolean::FALSE;
	                  }
	                break;
	        case INP_MOUSEMOVE:
	                if(pGadgDown && pGadgDown->gNum == DGAD_SBOX)
	                 {
	                    oPtr->addr = ((ULONG)GetNormalizedPropGadgPos(pWindow,DGAD_SBOX,in->mouseY - oPtr->layer->yPos,65535) * 256);
	                    changed = boolean::TRUE;
	                 }
	                else
                        inputUsed = boolean::FALSE;
	                break;
	        case INP_MOUSE_LEFTBUTTON_UP:
	                if(PropGadgUp(oPtr,in))
	                        changed = boolean::TRUE;
	                else
	                        inputUsed = boolean::FALSE;
	                break;
	        case INP_MOUSE_LEFTBUTTON_DOWN:
	                if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
	                 {
	                        switch(gPtr->gNum)
	                         {
	                                case GGAD_CLOSE:
											delete pWindow;
											delete oPtr;
	                                        inputUsed = boolean::TRUE;
	                                        break;
	                                case DGAD_UP:
	                                        oPtr->addr-=oPtr->lineInc;
	                                        break;
	                                case DGAD_DOWN:
	                                        oPtr->addr+=oPtr->lineInc;
	                                        break;
	                                case  DGAD_SBOX:
	                                        PropGadgDown(oPtr,in,gPtr);
	                                        changed = boolean::TRUE;
	                                        break;
	                                default:
	                                        inputUsed = boolean::FALSE;
	                         }
	                 }
	                else
	                 {
	                        switch(oPtr->mType)                                                // which dump type are we
	                          {
	                                case MTYPE_DISS:
	                                        xPos = in->mouseX - pWindow->xPos;
	                                        yPos = in->mouseY - pWindow->yPos;
	                                        if(xPos > 0 && xPos < (pWindow->xSize-1) && yPos < (pWindow->ySize-1))
	                                                pWindow->cursorY = yPos-1;
	                                        break;
	                                case MTYPE_BYTE:
	                                case MTYPE_WORD:
	                                case MTYPE_LONG:
	                                        xPos = in->mouseX - pWindow->xPos;
	                                        pWidth = ((pWindow->xSize-(2+6))/dWidthTbl[oPtr->mType]);
	                                        if(xPos >= 8 & xPos < (pWidth*dWidthTbl[oPtr->mType])+8)
	                                         {
	                                                pWindow = (_window *)oPtr->layer;
	                                                xPos = in->mouseX - pWindow->xPos;
	                                                yPos = (in->mouseY - pWindow->yPos)-1;

	                                                temp = (xPos - 8)/dWidthTbl[oPtr->mType];
	                                                temp *= memorySize[oPtr->mType];
	                                                temp += MemoryGetLineAddr(oPtr,yPos);
	                                                oPtr->longData2 = temp;

	                                                sprintf(textBuffer,"Enter New Data for $%lx",oPtr->longData2);
	                                                GetMultiExpr(MemoryWriteHex,oPtr,textBuffer,oPtr->layer);
	                                         }
	                                        break;
	                                case MTYPE_ASCII:
	                                        xPos = in->mouseX - pWindow->xPos;
	                                        yPos = in->mouseY - pWindow->yPos;
                                            pWidth = ((pWindow->xSize-(2+7)));
	                                        if(xPos >= 8)
	                                         {
	                                                temp = xPos - 8;
	                                                temp += (yPos-1) * pWidth;
	                                                oPtr->longData2 = oPtr->addr+temp;
	                                                GetString(MemoryWriteASCII,(void *)oPtr,"Enter New Text",oPtr->layer);
	                                         }
	                                        break;
	                          }
	                 }
	                break;
	        case INP_MOUSE_RIGHTBUTTON_DOWN:
	                mPtr = CreateMenuWithItems(memoryMenu,"Memory");
//	                mPtr->xPos = AutoPosition(pWindow->xPos,pWindow->xSize,mPtr->xSize,displayWidth);
//	                mPtr->yPos = AutoPosition(pWindow->yPos,pWindow->ySize,mPtr->ySize,displayHeight);
	                DoLocalMenu(mPtr,oPtr,in);
	                break;
	        case INP_RESIZE:
	                AddMemoryGadgets(pWindow);
	                if(pWindow->cursorY >= pWindow->ySize-3)
	                        pWindow->cursorY = pWindow->ySize-3;
	                break;
	        case INP_COMMAND:
	                MemoryCommand(in->cmdText,oPtr,in->errPtr);
	                break;
	        default:
	                inputUsed = boolean::FALSE;

	 }
	if(changed)
	 {
	        UpdateVScrollGadg(pWindow,DGAD_SBOX,(int)(oPtr->addr/256));
	        DrawGadgets(pWindow);
	 }
    return(inputUsed);
}

//=============================================================================
//note: if new features are added that may change the line to addr ratio,
// this routine must be upated

ULONG
MemoryGetLineAddr(_object *oPtr,int line)
{
	_breakList *bPtr;
	ULONG addr;
	unsigned int didLabel,doit,y;
    int pWidth,pHeight;
    _layer *lPtr;
	unsigned int offset;
	char far *buff;

    lPtr = oPtr->layer;
	addr = oPtr->addr;

	buff = xferBuffer;
	switch(oPtr->mType)                                                // which dump type are we
	 {
	        case MTYPE_DISS:
	                ReadSlaveData( addr, xferBuffer,lPtr->ySize*BYTESPERLINE);
	                printCheckForSymbol = boolean::FALSE;
#ifdef SNES
	                procMode = GetProcMode(addr,oPtr->mode816Size);
#endif
	                oPtr->lineInc = Disassem(addr,buff,textBuffer,0);                 // just to get length of op-code
	                printCheckForSymbol = boolean::TRUE;
	                didLabel = boolean::FALSE;
	                for(y=1;y<lPtr->ySize-1;y++)
	                  {
	                        doit = boolean::TRUE;
	                        if((oPtr->pLabels & 1) && didLabel == boolean::FALSE)
	                         {
	                                _symbolList *sPtr;
	                                if(FindHexSymbol(addr))
	                                 {
	                                        didLabel = boolean::TRUE;
	                                        doit = boolean::FALSE;
	                                 }
	                                if(y == line+1)                                // check if we are displaying line with cursor
	                                        break;
	                         }
	                        if(doit)
	                         {
	                                didLabel = boolean::FALSE;
	                                if(bPtr = FindBreak(addr))
	                                  {
#ifdef SNES
	                                        *buff = (UBYTE) (bPtr->inst);
#endif
#ifdef GENESIS
	                                        *buff = (UBYTE) (bPtr->inst >> 8);
	                                        *(buff+1) = (UBYTE) bPtr->inst;
#endif
	                                  }
	                                if(y == line+1)                                // check if we are on the line with the cursor
	                                        break;                                                        // abort, we are done
	                                printCheckForSymbol = boolean::FALSE;
	                                offset = Disassem(addr,buff,textBuffer,0);
	                                printCheckForSymbol = boolean::TRUE;
	                                buff += offset;
	                                addr += offset;
	                         }
	                  }
	                break;
	        case MTYPE_BYTE:
	        case MTYPE_WORD:
	        case MTYPE_LONG:
#ifdef GENESIS
	        case MTYPE_VDP:
	        case MTYPE_VDPCO:
	        case MTYPE_VDPVS:
#endif
#ifdef SNES
	        case MTYPE_PPU:
#endif
                    pWidth = lPtr->xSize-(2+6);
	                if(pWidth < 0 )
	                        pWidth = 0;
	                pWidth /= dWidthTbl[oPtr->mType];
	                pHeight = lPtr->ySize-1;
	                didLabel = boolean::FALSE;

	                switch(oPtr->mType)                                                // which Memory type are we
	                 {
	                        case MTYPE_BYTE:
	                        case MTYPE_WORD:
	                        case MTYPE_LONG:
	                                break;
#ifdef GENESIS
	                        case MTYPE_VDP:
	                                addr &= 0xffff;
	                                break;
	                        case MTYPE_VDPCO:
	                                addr = 0;
	                                break;
	                        case MTYPE_VDPVS:
	                                addr = 0;
	                                break;
#endif
#ifdef SNES
	                        case MTYPE_PPU:
	                                addr &= 0xffff;
	                                break;
#endif
	                 }
                    for(y=1;y<pHeight;y++)
                      {
	                        doit = boolean::TRUE;
	                        if((oPtr->pLabels & 1) && didLabel == boolean::FALSE)
	                         {
	                                _symbolList *sPtr;
	                                if(FindHexSymbol(addr))
	                                 {
	                                        didLabel = boolean::TRUE;
	                                        doit = boolean::FALSE;
	                                 }
	                         }
	                        if(doit)
	                         {
	                                didLabel = boolean::FALSE;
	                                if(y == line+1)                                // check if we are displaying line with cursor
	                                        break;
	                                addr += pWidth * (memorySize[oPtr->mType]);
	                         }
                      }
	                break;
	        case MTYPE_ASCII:
                    pWidth = ((lPtr->xSize-(2+7)));
	                if(pWidth < 0)
	                        pWidth = 0;
                    for(y=1;y<lPtr->ySize-1;y++)
                      {
	                        if(y == lPtr->cursorY+1)                                // check if we are displaying line with cursor
	                                break;
	                        addr += pWidth;
                      }
	                break;
	 }

	return(addr);
}

//=============================================================================

void
MemoryRoutine(_object *oPtr)
{
	_breakList *bPtr;
	char *sPtr;
	unsigned int dissType,data2,didLabel,doit,attr;
    unsigned int x,y;
	int pWidth,pHeight,bytesNeeded;
	ULONG addr;
	unsigned int offset;
    _layer *lPtr;
	char far *buff;
	char *buffer;
	FLAG abort = boolean::FALSE;

    lPtr = oPtr->layer;

	// first draw window status line
	FillString(textBuffer,'\xC4',160);
	textBuffer[159] = 0;
	sPtr = PrintString(textBuffer,"Type: ");
	sPtr = PrintString(sPtr,mTypeText[oPtr->mType]);
	sPtr = PrintString(sPtr,"\xC4Upd: ");
	sPtr = PrintString(sPtr,updText[oPtr->updateMode]);

	if(oPtr->updateMode == UPDATE_STATIC)
	 {
	        sprintf(sPtr,"$%lx",oPtr->addr);
	        sPtr = FindNull(sPtr);
	        *sPtr = '\xC4';
	 }
	else
	        sPtr = PrintString(sPtr,oPtr->dataPtr2);
	sPtr = textBuffer;
	PrintLayAttrClip(lPtr,sPtr,2,lPtr->ySize-1,windowBorderAttr,lPtr->xSize-1);

	if(!slaveUpdate)
	        return;                                                // dont bother

	switch(oPtr->updateMode)
	 {
	        case UPDATE_STATIC:
	                addr = oPtr->addr;
	                break;
	        case UPDATE_DYNAMICRUNNING:
	                        if(DoExp(oPtr->dataPtr2))
	                                exprAnswer = 0;                                        // if error occured, just go to zero
	                        if(exprAnswer != oPtr->longData)
	                         {
	                                addr = exprAnswer;
	                                   oPtr->longData = addr;
	                                oPtr->layer->cursorY = 0;
	                         }
	                        else
	                                addr = oPtr->addr;
	                break;
	        case UPDATE_DYNAMIC:
	                  if(!DoExp(oPtr->dataPtr2))
	                          addr = exprAnswer;
	                  else
	                        addr = 0;
	                break;
	 }
	oPtr->addr = addr;                                        // stash it back for modify

	buff = xferBuffer;
	switch(oPtr->mType)                                                // which Memory type are we
	 {
	        case MTYPE_DISS:
	                ReadSlaveData(addr,xferBuffer,lPtr->ySize*BYTESPERLINE);
                    if (CheckSlaveAlive())
	                        break;                                                // if slave dead, dont update
	                FillWindow(lPtr);

	                oPtr->lineInc = 1;
	                if(oPtr->disMode == 0)
	                 {
	                        dissType = 2;
	                        if(lPtr->xSize < MEMORY_HEXWIDTH)
	                                if(lPtr->xSize < MEMORY_ADDRWIDTH)
	                                        dissType = 0;
	                                else
	                                        dissType = 1;
	                 }
	                else
	                        dissType = oPtr->disMode-1;

	                printCheckForSymbol = boolean::FALSE;
#ifdef SNES
	                procMode = GetProcMode(addr,oPtr->mode816Size);
#endif
	                oPtr->lineInc = Disassem(addr,buff,textBuffer,0);                 // just to get length of op-code
	                printCheckForSymbol = boolean::TRUE;
	                didLabel = boolean::FALSE;
	                for(y=1;y<lPtr->ySize-1;y++)
	                  {
	                         attr = lPtr->charAttr;

	                        doit = boolean::TRUE;
	                        buffer = textBuffer;
	                        if((oPtr->pLabels & 1) && didLabel == boolean::FALSE)
	                         {
	                                _symbolList *sPtr;
	                                if(sPtr = FindHexSymbol(addr))
	                                 {
	                                        buffer = PrintString(buffer,sPtr->Name());
	                                        didLabel = boolean::TRUE;
	                                        doit = boolean::FALSE;
	                                        *buffer = ':';
	                                        *buffer = 0;
	                                 }
	                         }
	                        if(doit)
	                         {
	                                didLabel = boolean::FALSE;
	                                if(bPtr = FindBreak(addr))
	                                  {
	                                        attr = dumpBreakAttr;
#ifdef SNES
	                                        *buff = (UBYTE) (bPtr->inst);
#endif
#ifdef GENESIS
	                                        *buff = (UBYTE) (bPtr->inst >> 8);
	                                        *(buff+1) = (UBYTE) bPtr->inst;
#endif
	                                  }
	                                if(addr == GetReg(REG_INSTRUCTIONPOINTER))
	                                        attr = dumpPCAttr;
	                                printCheckForSymbol = oPtr->pLabels & 2;
	                                offset = Disassem(addr,buff,textBuffer,dissType);
	                                printCheckForSymbol = boolean::TRUE;
	                                buff += offset;
	                                addr += offset;
	                         }
	                        if(y == lPtr->cursorY+1)                                // check if we are displaying line with cursor
	                                attr = dumpCursorAttr;
	                        PrintLayAttrClip(lPtr,textBuffer,1,y,attr,lPtr->xSize-1);
	                  }
	                break;
#ifdef GENESIS
	        case MTYPE_VDP:
	        case MTYPE_VDPCO:
	        case MTYPE_VDPVS:
#endif
#ifdef SNES
	        case MTYPE_PPU:
#endif
	        case MTYPE_BYTE:
	        case MTYPE_WORD:
	        case MTYPE_LONG:
                    pWidth = lPtr->xSize-(2+6);
	                if(pWidth < 0)
	                        pWidth = 0;
	                pHeight = lPtr->ySize-1;
	                bytesNeeded = (pWidth * pHeight)/2;
	                pWidth /= dWidthTbl[oPtr->mType];
	                switch(oPtr->mType)                                                // which dump type are we
	                 {
	                        case MTYPE_BYTE:
	                        case MTYPE_WORD:
	                        case MTYPE_LONG:
	                                ReadSlaveData(addr,xferBuffer,bytesNeeded);
	                                break;
#ifdef GENESIS
	                        case MTYPE_VDP:
	                                addr &= 0xffff;
	                                ReadSlaveVDP(addr,xferBuffer,bytesNeeded);
	                                break;
	                        case MTYPE_VDPCO:
	                                addr = 0;
	                                ReadSlaveCRAM(xferBuffer);
	                                break;
	                        case MTYPE_VDPVS:
	                                addr = 0;
	                                ReadSlaveVSRAM(xferBuffer);
	                                break;
#endif
#ifdef SNES
	                        case MTYPE_PPU:
	                                addr &= 0xffff;
	                                ReadSlavePPU(addr,xferBuffer,bytesNeeded);
	                                break;
#endif
	                 }
                    if (CheckSlaveAlive())
	                        break;                                                // if slave dead, dont update
	                FillWindow(lPtr);                                // clear window

	                didLabel = boolean::FALSE;
                    for(y=1;y<pHeight && abort == boolean::FALSE;y++)                // print data dump
                      {
	                         attr = lPtr->charAttr;
	                        doit = boolean::TRUE;
	                        buffer = textBuffer;
	                        if((oPtr->pLabels & 1) && didLabel == boolean::FALSE)
	                         {
	                                _symbolList *sPtr;
	                                if((sPtr = FindHexSymbol(addr)) != 0)
	                                 {
	                                        buffer = PrintString(buffer,sPtr->Name());
	                                        didLabel = boolean::TRUE;
	                                        doit = boolean::FALSE;
	                                        *buffer = ':';
	                                 }
	                         }
	                        if(doit)
	                         {
	                                didLabel = boolean::FALSE;
	                                buffer = PrintRaw24Bits(buffer,addr);
	                                oPtr->lineInc = pWidth * memorySize[oPtr->mType];
	                                addr += oPtr->lineInc;

	                                *buffer++ = ':';

	                                for(x=0;x<pWidth && abort == boolean::FALSE;x++)
	                                  {
	                                        switch(oPtr->mType)
	                                          {
	                                                case MTYPE_BYTE:
	                                                    buffer = Print8Bits(buffer,*buff++);
	                                                        break;
	                                                case MTYPE_WORD:
#ifdef SNES
	                                                case MTYPE_PPU:
#endif
#ifdef GENESIS
	                                                case MTYPE_VDP:
	                                                case MTYPE_VDPCO:
	                                                case MTYPE_VDPVS:
#endif
#if WORDSWAP
	                                                    buffer = Print8Bits(buffer,*buff++);
	                                                    buffer = Print8Bits(buffer,*buff++);
#endif
#if !WORDSWAP
	                                                    buffer = Print8Bits(buffer,*(buff+1));
	                                                    buffer = Print8Bits(buffer,*buff);
	                                                        buff += 2;
#endif
	                                                        break;
	                                                case MTYPE_LONG:
#ifdef GENESIS
	                                                    buffer = Print8Bits(buffer,*buff++);
	                                                    buffer = Print8Bits(buffer,*buff++);
	                                                    buffer = Print8Bits(buffer,*buff++);
	                                                    buffer = Print8Bits(buffer,*buff++);
#endif
#ifdef SNES
	                                                    buffer = Print8Bits(buffer,*(buff+3));
	                                                    buffer = Print8Bits(buffer,*(buff+2));
	                                                    buffer = Print8Bits(buffer,*(buff+1));
	                                                    buffer = Print8Bits(buffer,*buff);
	                                                        buff += 4;
#endif
	                                                        break;
	                                          }
	                                        if(x != pWidth-1)
	                                                *buffer++ = ' ';
	                                        switch(oPtr->mType)
	                                          {
	                                                case MTYPE_BYTE:
	                                                case MTYPE_WORD:
	                                                case MTYPE_LONG:
	                                                        break;
#ifdef GENESIS
	                                                case MTYPE_VDP:
	                                                        if(addr > 0xffff)
	                                                                abort = boolean::TRUE;
	                                                        break;
	                                                case MTYPE_VDPCO:
	                                                        if(addr >= 128)
	                                                                abort = boolean::TRUE;
	                                                        break;
	                                                case MTYPE_VDPVS:
	                                                        if(addr >= 80)
	                                                                abort = boolean::TRUE;
	                                                        break;
#endif
#ifdef SNES
	                                                case MTYPE_PPU:
	                                                        if(addr > 0xffff)
	                                                                abort = boolean::TRUE;
	                                                        break;
#endif
	                                          }
	                                 }
	                         }
	                        *buffer = 0;
	                        if(y == lPtr->cursorY+1)                                // check if we are displaying line with cursor
	                                attr = dumpCursorAttr;
	                        PrintLayAttrClip(lPtr,textBuffer,1,y,attr,lPtr->xSize-1);
                      }
	                break;
	        case MTYPE_ASCII:
                    pWidth = ((lPtr->xSize-(2+7)));
	                if(pWidth < 0)
	                        pWidth = 0;
	                pHeight = lPtr->ySize-1;
	                oPtr->lineInc = pWidth;
	                ReadSlaveData(addr,xferBuffer,pWidth*pHeight);
                    if (CheckSlaveAlive())
	                        break;                                                // if slave dead, dont update

	                FillWindow(lPtr);

                    for(y=1;y<lPtr->ySize-1;y++)
                      {
	                         attr = lPtr->charAttr;
	                        buffer = PrintRaw24Bits(textBuffer,addr);
	                        addr += pWidth;
	                        *buffer++ = ':';

	                        for(x=0;x<pWidth;x++)
	                            buffer = PrintASCII(buffer,*buff++);
	                        *buffer = 0;
	                        if(y == lPtr->cursorY+1)                                // check if we are displaying line with cursor
	                                attr = dumpCursorAttr;
	                        PrintLayAttrClip(lPtr,textBuffer,1,y,attr,lPtr->xSize-1);
                      }
	                break;
	 }
}

//=============================================================================

struct _inputKeyRemap memoryKeys[] =
{
	{KEY_DOWN ,MEMORY_DOWNLINE        },
	{KEY_UP   ,MEMORY_UPLINE          },
	{KEY_PGDWN,MEMORY_PGDWN           },
	{KEY_PGUP ,MEMORY_PGUP            },
	{KEY_LEFT ,MEMORY_DOWN            },
	{KEY_RIGHT,MEMORY_UP              },
	{KEY_CTRLH,MEMORY_RUNTOHERE       },
	{KEY_CTRLS,MEMORY_SETBREAKHERE    },
	{KEY_CTRLO,MEMORY_SETBREAKONCEHERE},
	{KEY_CTRLN,MEMORY_SETBREAKCOUNTHERE},
	{KEY_CTRLA,MEMORY_CLEARALLBREAKS  },
	{KEY_CTRLC,MEMORY_CLEARBREAK      },
	{KEY_CTRLM,MEMORY_CHANGEMEM       },
	{KEY_CTRLP,MEMORY_PROCHELP        },
	{KEY_CTRLB,MEMORY_BYTE	       },
	{KEY_CTRLW,MEMORY_WORD	       },
	{KEY_CTRLL,MEMORY_LONG	       },
	{KEY_CTRLD,MEMORY_CODE	       },
	{KEY_CTRLY,MEMORY_ASCII      },
	{KEY_CTRLG,MEMORY_GOTO        },
	{0,0}
};

//=============================================================================

FLAG
OpenMemory(void)
{
    _object *oPtr;
    _window *pWindow;
    oPtr = AddObject(MemoryRoutine);
    pWindow = new _window(0,6,42,11,"Memory",(unsigned char)dumpAttr,(char)dumpChar);
	pWindow->data = (void *)oPtr;
	pWindow->xMin = MEMORYMINWIDTH;
	pWindow->xMax = displayWidth;
	pWindow->yMin = 4;                                        // leave enough room for up/down gadgets
	pWindow->yMax = displayHeight;
	pWindow->cursorY = 0;                                        // dump doesn't use hardware cursor, uses highlight
    oPtr->inputRoutine = MemoryInput;
	oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;
	oPtr->layer = pWindow;
	oPtr->addr = 0;                          // default address to 0
	oPtr->mType = MTYPE_DISS;                // default to dissasembly
	oPtr->pLabels = 3;                                // default to print no labels
	oPtr->lineInc = 1;                                // line increment
	oPtr->mode816Size = 1;
	oPtr->disMode = 0;
	oPtr->updateMode = UPDATE_STATIC;
	oPtr->dataPtr2 = NULL;
	oPtr->remap.keyArray = memoryKeys;

	AddSysGadgets(pWindow);
	AddMemoryGadgets(pWindow);
	DrawGadgets(pWindow);
	ActivateFrontWindow();
	return(boolean::TRUE);
}

//=============================================================================

// Open a Memory window focused at a specific address, in byte view.  Used by the
// search-results window's "go to hit" action.  Mirrors OpenMemory() but seeds the
// address and a byte dump (more useful than disassembly for "a value lives here").
FLAG
OpenMemoryAt(ULONG addr)
{
    _object *oPtr;
    _window *pWindow;
    oPtr = AddObject(MemoryRoutine);
    pWindow = new _window(0,6,42,11,"Memory",(unsigned char)dumpAttr,(char)dumpChar);
	pWindow->data = (void *)oPtr;
	pWindow->xMin = MEMORYMINWIDTH;
	pWindow->xMax = displayWidth;
	pWindow->yMin = 4;
	pWindow->yMax = displayHeight;
	pWindow->cursorY = 0;
    oPtr->inputRoutine = MemoryInput;
	oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;
	oPtr->layer = pWindow;
	oPtr->addr = addr;                       // focus the requested hit address
	oPtr->mType = MTYPE_BYTE;                // byte view for "value found here"
	oPtr->pLabels = 3;
	oPtr->lineInc = 1;
	oPtr->mode816Size = 1;
	oPtr->disMode = 0;
	oPtr->updateMode = UPDATE_STATIC;
	oPtr->dataPtr2 = NULL;
	oPtr->remap.keyArray = memoryKeys;

	AddSysGadgets(pWindow);
	AddMemoryGadgets(pWindow);
	DrawGadgets(pWindow);
	ActivateFrontWindow();
	return(boolean::TRUE);
}

//=============================================================================

