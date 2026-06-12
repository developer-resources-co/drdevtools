//=============================================================================
// expr.c: expression parser interface
//=============================================================================

#include	"moninc.hpp"
#include	"display.hpp"
#include	"express.hpp"
#include	"help.hpp"

//=============================================================================

ULONG	exprAnswer;				/* place to put the result */

extern "C" long yyparse();
char* lex_input_string;
int bGetInput;

int
DoExp(const char *isPtr)
{
	int error = 0;

	exprAnswer = rand();

	lex_input_string = (char*)isPtr;
	UnPadString( lex_input_string );
	bGetInput = 1;
	exprAnswer = yyparse();

	return error;
}

//=============================================================================


#define 	EXP_XSIZE 58
#define		EXP_YSIZE 7

#define	EXP_EXPX	3
#define	EXP_EXPY	2

#define EXP_HEXRESX	3
#define EXP_HEXRESY 6

#define EXP_DECRESX	13
#define EXP_DECRESY 6

#define EXP_BINRESX	24
#define EXP_BINRESY 6

#define EXP_ERRORX	3
#define EXP_ERRORY	4

#define EXP_SYMRESX	27
#define EXP_SYMRESY	4

#define	EXP_OKX	51
#define	EXP_OKY	4

#define	EXPGAD_EXP	GAD_USER+1
#define	EXPGAD_RES	EXPGAD_EXP+1
#define	EXPGAD_RESDEC	EXPGAD_RES+1
#define	EXPGAD_RESBIN	EXPGAD_RESDEC+1
#define	EXPGAD_RESSYM	EXPGAD_RESBIN+1
#define	EXPGAD_OK		EXPGAD_RESSYM+1

//=============================================================================

FLAG exprOpen = boolean::FALSE;
_object *exprObjPtr = NULL;

char expString[EXPRLEN+2] = {0};			// start with empty string

//=============================================================================

void
AddExpGadgets(_window *pWindow)
{
	_gadget  *gPtr;
							// input gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 53;
	gPtr->ySize = 1;
	gPtr->xPos = EXP_EXPX;
	gPtr->yPos = EXP_EXPY;
	gPtr->gadgText = expString;
	gPtr->gNum = EXPGAD_EXP;
	gPtr->gadgAttr = windowBorderAttr;
	gPtr->gType = GADTYPE_STRING;
	gPtr->mode = insertDefault?STRGADF_MODE_INSERT:0;

							// ok gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 4;
	gPtr->ySize = 1;
	gPtr->xPos = EXP_OKX;
	gPtr->yPos = EXP_OKY;
	gPtr->gadgText = "[Ok]";
	gPtr->gNum = EXPGAD_OK;
	gPtr->gadgAttr = windowBorderAttr;

							// texts
	PrintLay(pWindow,"Expression",EXP_EXPX,EXP_EXPY-1);
	PrintLay(pWindow,"Error",EXP_ERRORX,EXP_ERRORY-1);
	PrintLayAttr(pWindow,SYSERRORS[0],EXP_ERRORX,EXP_ERRORY,windowBorderAttr);
}

//=============================================================================

void
AddExpWinGadgets(_window *pWindow)
{
	_gadget  *gPtr;
							// input gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 53;
	gPtr->ySize = 1;
	gPtr->xPos = EXP_EXPX;
	gPtr->yPos = EXP_EXPY;
	SetGadgString(gPtr,"                                                     ");
	gPtr->gNum = EXPGAD_EXP;
	gPtr->gadgAttr = windowBorderAttr;
	gPtr->gType = GADTYPE_STRING;

							// result gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 9;
	gPtr->ySize = 1;
	gPtr->xPos = EXP_HEXRESX;
	gPtr->yPos = EXP_HEXRESY;
	SetGadgString(gPtr,"$00000000");
	gPtr->gNum = EXPGAD_RES;
	gPtr->gadgAttr = windowBorderAttr;
	gPtr->gType = GADTYPE_STRING;

							// result(decimal) gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 10;
	gPtr->ySize = 1;
	gPtr->xPos = EXP_DECRESX;
	gPtr->yPos = EXP_DECRESY;
	SetGadgString(gPtr,"0000000000");
	gPtr->gNum = EXPGAD_RESDEC;
	gPtr->gadgAttr = windowBorderAttr;
	gPtr->gType = GADTYPE_STRING;

							// result(binary) gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 32;
	gPtr->ySize = 1;
	gPtr->xPos = EXP_BINRESX;
	gPtr->yPos = EXP_BINRESY;
	SetGadgString(gPtr,"00000000000000000000000000000000");
	gPtr->gNum = EXPGAD_RESBIN;
	gPtr->gadgAttr = windowBorderAttr;
	gPtr->gType = GADTYPE_STRING;

							// result(symbol) gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 29;
	gPtr->ySize = 1;
	gPtr->xPos = EXP_SYMRESX;
	gPtr->yPos = EXP_SYMRESY;
	SetGadgString(gPtr,"                             ");
	gPtr->gNum = EXPGAD_RESSYM;
	gPtr->gadgAttr = windowBorderAttr;
	gPtr->gType = GADTYPE_STRING;

	PrintLay(pWindow,"Hex",EXP_HEXRESX,EXP_HEXRESY-1);
	PrintLay(pWindow,"Decimal",EXP_DECRESX,EXP_DECRESY-1);
	PrintLay(pWindow,"Binary",EXP_BINRESX,EXP_BINRESY-1);
	PrintLay(pWindow,"Symbol",EXP_SYMRESX,EXP_SYMRESY-1);

	PrintLay(pWindow,"Expression",EXP_EXPX,EXP_EXPY-1);
	PrintLay(pWindow,"Error",EXP_ERRORX,EXP_ERRORY-1);
	PrintLayAttr(pWindow,SYSERRORS[0],EXP_ERRORX,EXP_ERRORY,windowBorderAttr);
}

//=============================================================================

HandleExpr(_object *oPtr)
{
// expression parser stuff
	errorcode error;			/* place to put an error */
	char 	*isPtr;				/* pointer to the input stream */
	char 	*cPtr;				// used locally
	FLAG cont;						// used locally
	void (*expRoutine)(ULONG result,void *data);
	_symbolList *sPtr;
	_gadget  *gPtr;

	FLAG leave = boolean::FALSE;
    _window *pWindow;

    pWindow = (_window*)oPtr->layer;
	isPtr = pWindow->activeGadget->gadgText;
	//UnPadString(isPtr);

	cPtr = isPtr;
	cont = boolean::TRUE;
	while(cont)
	 {
		if(oPtr->data1)
		 {
			while(*cPtr != ',' && *cPtr != 0)
				cPtr++;
			if(*cPtr == 0)
				cont = boolean::FALSE;
			else
				*cPtr++ = 0;									// write over comma with null to fool expr parser
			error = DoExp(isPtr);
			isPtr = cPtr;
		 }
		else
		 {
			error = DoExp(isPtr);
			cont = boolean::FALSE;
		 }
		if(error)
		 {
			PrintLayAttr(pWindow,SYSERRORS[error],EXP_ERRORX,EXP_ERRORY,windowBorderAttr);
			cont = 0;
		 }
		else
		 {
			if(oPtr->dataPtr)				// if pop-up
			 {
				expRoutine = (void (*)(unsigned long, void *))oPtr->dataPtr;
				(*expRoutine)(exprAnswer,oPtr->dataPtr2);
				leave = boolean::TRUE;
			 }
			else
			 {
				gPtr = FindGadget(&pWindow->gadgBase,EXPGAD_RES);
				sprintf(gPtr->gadgText,"$%08lx",exprAnswer);
				gPtr = FindGadget(&pWindow->gadgBase,EXPGAD_RESDEC);
				sprintf(gPtr->gadgText,"%010lu",exprAnswer);

				gPtr = FindGadget(&pWindow->gadgBase,EXPGAD_RESBIN);
				*PrintBinary32Bits(gPtr->gadgText,exprAnswer) = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses"
				if(sPtr = FindHexSymbol(exprAnswer))
#pragma GCC diagnostic pop
					*PrintString(textBuffer,sPtr->Name()) = 0;
				else
					textBuffer[0] = 0;
				PadString(textBuffer,80);
				gPtr = FindGadget(&pWindow->gadgBase,EXPGAD_RESSYM);
				strncpy(gPtr->gadgText,textBuffer,gPtr->xSize);
				gPtr->gadgText[gPtr->xSize] = 0;
			 }
			PrintLayAttr(pWindow,SYSERRORS[error],EXP_ERRORX,EXP_ERRORY,windowBorderAttr);
		 }
	 }
	return(leave);
}

//=============================================================================

FLAG
ExpWinInput(_input *in,_object *oPtr)
{
	FLAG inputUsed = boolean::TRUE;
	FLAG iUsed = boolean::FALSE;
	FLAG leave = boolean::FALSE;

	_gadget  *gPtr;
    _window *pWindow;

    pWindow = (_window *)oPtr->layer;

	switch(in->inputType)
	 {
		case INP_KEY:
			if(pWindow->activeGadget)
				iUsed = StrGadgInput(pWindow,pWindow->activeGadget,in);
			if(iUsed == boolean::FALSE)
				switch(in->fullKey)
	 		 	 {
					case CMD_LIST_ABORT:
					case CMD_CLOSEWINDOW:
						leave = boolean::TRUE;
						break;
					case KEY_RETURN:
						switch(pWindow->activeGadget->gNum)
						 {
							case EXPGAD_EXP:
								leave = HandleExpr(oPtr);
								break;
						}
						break;
					case CMD_OPENWINDOWHELPFILE:
						OpenRefFile(EXT_HELP,HELPNAME,"Expression:");
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
					case GGAD_CLOSE:
						leave = boolean::TRUE;
						break;
					case EXPGAD_EXP:
						pWindow->activeGadget = gPtr;
						ActivateStrGadget(pWindow,gPtr);
						break;
					case EXPGAD_OK:									// note: ok gadget only on pop-up
						leave = HandleExpr(oPtr);
						break;
					default:
						if(gPtr->gType == GADTYPE_STRING)
						 {
							pWindow->activeGadget = gPtr;
							ActivateStrGadget(pWindow,gPtr);
						 }
						else
							inputUsed = boolean::FALSE;
						break;
				 }
			 }
			break;
		default:
			inputUsed = boolean::FALSE;
			break;
	 }

	if(inputUsed)
	 {
		DrawGadgets(pWindow);
	 }

	if(leave)
	 {
		FLAG notPopup = !oPtr->dataPtr;	// capture BEFORE delete — reading oPtr->dataPtr
//		CloseWindow(pWindow);			// after `delete oPtr` is a use-after-free (ASan)
		delete pWindow;
//		DeleteObject(oPtr);
		delete oPtr;
		if(notPopup)					// if not pop-up
		 {
			exprOpen = boolean::FALSE;
			exprObjPtr = NULL;
		 }
	 }
	return(inputUsed);
}

//=============================================================================

void
OpenExp(int xPos,int yPos)
{
	_object *oPtr;
	_window *pWindow;

	if(exprOpen == boolean::TRUE)
	{
		exprObjPtr->layer->ToFront();
		ActivateFrontWindow();
		return;
	}

    pWindow = new _window(xPos,yPos,EXP_XSIZE,EXP_YSIZE+2,"Expression",(unsigned char)exprAttr,(char)exprChar);
	pWindow->cursorOn = boolean::TRUE;
	oPtr = AddObject();

	exprOpen = boolean::TRUE;
	exprObjPtr = oPtr;
	oPtr->dataPtr = NULL;
	oPtr->inputFlags = INPF_KEY|INPF_MOUSEBUTTONS|INPF_CASESENSE;
	oPtr->inputRoutine = ExpWinInput;
	oPtr->layer =pWindow;
	pWindow->data = (void *)oPtr;

	AddSysGadgets(pWindow);
	AddExpWinGadgets(pWindow);
	DrawGadgets(pWindow);
	pWindow->activeGadget = FindGadget(&pWindow->gadgBase,EXPGAD_EXP);
	ActivateStrGadget(pWindow,pWindow->activeGadget);
	ActivateFrontWindow();
}

//=============================================================================

FLAG
OpenExpWindow(void)
{
	OpenExp(0,15);
	return(boolean::TRUE);
}

//=============================================================================
// dataPtr->routine to execute when expression parsed
// dataPtr2-> ?? whatever user wants, will get passed to final routine

void
GetExpr(void (*expRoutine)(ULONG result,void *dataPtr),void *dataPtr,char *title,_layer *backLPtr)
{
	_object *oPtr;
	_window *pWindow;
	int xPos,yPos;
	if(backLPtr)
	 {
   		xPos = AutoPosition(backLPtr->xPos,backLPtr->xSize,EXP_XSIZE,displayWidth);
		yPos = AutoPosition(backLPtr->yPos,backLPtr->ySize,EXP_YSIZE,displayHeight);
	 }
	else
	 {
   		xPos = 0;
		yPos = 0;
	 }

    pWindow = new _window(xPos,yPos,EXP_XSIZE,EXP_YSIZE,title,(unsigned char)exprAttr,(char)exprChar);
	if(pWindow)
	 {
		pWindow->cursorOn = boolean::TRUE;

		oPtr = AddObject();
		oPtr->inputFlags = INPF_KEY|INPF_MOUSEBUTTONS|INPF_CASESENSE;
		oPtr->inputRoutine = ExpWinInput;
		oPtr->layer =pWindow;
		oPtr->data1 = 0;
		oPtr->dataPtr = (char *)expRoutine;
		oPtr->dataPtr2 = (char *)dataPtr;
		pWindow->data = (void *)oPtr;

		AddSysGadgets(pWindow);
		AddExpGadgets(pWindow);
		pWindow->activeGadget = FindGadget(&pWindow->gadgBase,EXPGAD_EXP);
		ActivateStrGadget(pWindow,pWindow->activeGadget);
		ActivateFrontWindow();
		DrawGadgets(pWindow);
	 }
	else
		PrintError(ERROR_NOMEM);
}

//=============================================================================
// dataPtr->routine to execute when expression parsed
// (if dataPtr == 0, then must be normal parser)
// dataPtr2-> ?? whatever user wants, will get passed to final routine
// data1 = flag, 0 = normal expression, 1 = multi-expression

void
GetMultiExpr(void (*expRoutine)(ULONG result,void *dataPtr),void *dataPtr,char *title,_layer *backLPtr)
{
	_object *oPtr;
	_window *pWindow;
	int xPos,yPos;

	xPos = AutoPosition(backLPtr->xPos,backLPtr->xSize,EXP_XSIZE,displayWidth);
	yPos = AutoPosition(backLPtr->yPos,backLPtr->ySize,EXP_YSIZE,displayHeight);

    pWindow = new _window(xPos,yPos,EXP_XSIZE,EXP_YSIZE,title,(unsigned char)exprAttr,(char)exprChar);
	if(pWindow)
	 {
		pWindow->cursorOn = boolean::TRUE;

		oPtr = AddObject();
		oPtr->inputFlags = INPF_KEY|INPF_MOUSEBUTTONS|INPF_CASESENSE;
		oPtr->inputRoutine = ExpWinInput;
		oPtr->layer =pWindow;
		oPtr->data1 = 1;
		oPtr->dataPtr = (char *)expRoutine;
		oPtr->dataPtr2 = (char *)dataPtr;
		pWindow->data = (void *)oPtr;

		AddSysGadgets(pWindow);
		AddExpGadgets(pWindow);
		pWindow->activeGadget = FindGadget(&pWindow->gadgBase,EXPGAD_EXP);
		ActivateStrGadget(pWindow,pWindow->activeGadget);
		ActivateFrontWindow();
		DrawGadgets(pWindow);
	 }
	else
		PrintError(ERROR_NOMEM);
}

//=============================================================================

ULONG LookupVar(char *string)			// given a null terminated string.
{									// find symbol, if exists
	_symbolList *sPtr;
	ULONG result;


	// Registers
	if ((result = GetRegAsSymbol(string)) != OUT_OF_BAND)
		return(result);

	// symbols
	if ((sPtr = FindSymbol(string)) != NULL)
		return(sPtr->Eval());

	// Parameters (like environment variables)
	return(GetParamAsSymbol(string));
}

//=============================================================================

errorcode EvalVar( ULONG * lvalue, token * * p )
{
	int	i;
	ULONG j,k;
	j = 0L;
	for (i = 0; i < 32 ; i += 8 ) {
		k = (ULONG) ((ULONG)(*(*p)++) << i);
		j |= k;
		}
//	*lvalue = *(ULONG *)j ;
	*lvalue = j;
	return NOERR;
}

//=============================================================================
