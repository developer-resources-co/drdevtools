//============================================================================
//	source.cpp: source window handler for drmon
//============================================================================
// dataPtr2 -> current filename
//==============================================================================

#include	<string.h>

#include	"base.hpp"
#include	"global.hpp"

#include	"mem.hpp"
#include	"display.hpp"
#include	"general.hpp"
#include	"config.hpp"
#include	"list.hpp"
#include	"keys.hpp"
#include	"monkeys.hpp"
#include	"input.hpp"
#include	"gadget.hpp"
#include	"object.hpp"
#include	"layer.hpp"
#include	"window.hpp"
#include "menu.hpp"
#include	"filereq.hpp"
#include	"message.hpp"
#include	"error.hpp"
#include	"command.hpp"
#include	"word.hpp"
#include	"textview.hpp"
#include	"reg.hpp"
#include	"control.hpp"
#include "expr.hpp"
#include	"listrect.hpp"
#include	"symbol.hpp"
#include "break.hpp"

#include "source.hpp"
#include "sld.hpp"
#include	"help.hpp"

//============================================================================

FLAG sourceOpen = boolean::FALSE;
_source *sourceObjPtr = NULL;
char *sourcePath = NULL;

//=============================================================================

void
SourceLoadGUI(void *data,char *path,char *fileName)
	{
	PrintError( stSld->SourceLoad(fileName) );
	}

//=============================================================================

void
SourceGotoDSymbol(char *string,void *dataPtr)
{
#if 0
	_source *oPtr;
	_symbolList *sPtr;
	_sld *sld = stSld;
	ULONG sourceAddrTemp;

	oPtr = (_source *)dataPtr;
	UnPadString(string);
	UpCaseString(string);
	if((sPtr = FindSymbol(string)) != 0)
	 {
		sld *tempSld = new sld(stSld);

//		sourceAddrTemp = sld->SourceAddress();
		tempSld->ReSyncSource(sPtr->Address());
//		sld->ReSyncSource(sPtr->Address());
//		sld->SourceAddress(sourceAddrTemp);
	 }
	else
		PrintError(ERROR_NOSUCHVAR);
#endif
}

//============================================================================

FLAG
SourceGotoSymbol(_menuItem *iPtr,_object *oPtr,int choice)
{
	if(oPtr->dataPtr2)
		strcpy(expString,oPtr->dataPtr2);
	else
		*expString = 0;
	PadString(expString,EXPRLEN);
	GetString(SourceGotoDSymbol,oPtr,"Enter Symbol To Goto",oPtr->layer);
	return(boolean::TRUE);
}

//============================================================================

menuItems sourceMenu[] =
{
#if defined( DEBUGDR ) || defined( DEBUGZARDOZ )
	{"&Load SLD File...   Ctrl-L",SendWindowMessage,SOURCE_LOAD},
#endif
	{"&Search...          Ctrl-S",SendWindowMessage,SOURCE_SEARCH},
	{"Sea&rch Label...    Ctrl-R",SendWindowMessage,SOURCE_SEARCHLABEL},
	{"&Next               Ctrl-N",SendWindowMessage,SOURCE_NEXT},
	{"&Goto...            Ctrl-G",SendWindowMessage,SOURCE_GOTO},
	{0,0,0}
};

//=============================================================================

void
SourceCommand(char **s,_object *oPtr, errorcode *error)
{
	ULONG dummy;
	unsigned int word;

	*error = NOERR;
	while (*s && **s && !*error)
	{
		*s = ParseWord(*s, &word);
		switch (word & 0xff)
		{
			case RW_WLOAD:
			default:
				*error = stSld->SourceLoad(*s);
				*s=NULL;
				break;
		}
	}
	return;
}

//=============================================================================

FLAG
SourceInput(_input *in,_object *oPtr)
{
    _window *pWindow;
	_menu *mPtr;
	_menuItem *iPtr;
	_gadget  *gPtr;
	_source *sPtr = (_source *)oPtr;

	_sld *sld = stSld;

	ULONG temp;
	unsigned int xPos,yPos,pWidth;

    FLAG inputUsed;
	FLAG changed = boolean::FALSE;
	FLAG closeWindow = boolean::FALSE;

    pWindow = (_window *)oPtr->layer;
    inputUsed = boolean::TRUE;
	switch(in->inputType)
	 {
		case INP_WINDOW_MESSAGE:
			switch(in->message)
			 {
#if defined( DEBUGDR ) || defined( DEBUGZARDOZ )
				case SOURCE_LOAD:
					strcpy(patternString,"*.sld       ");
					DoFileReq("Load SLD File",10,1,SourceLoadGUI,(void *)oPtr);
					break;
#endif
				case SOURCE_SEARCH:
					GetString(FVSearch,(void *)oPtr,"Enter Search String",pWindow);
					break;
				case SOURCE_SEARCHLABEL:
					GetString(FVSearchLabel,(void *)oPtr,"Enter Search String",oPtr->layer);
					break;
				case SOURCE_NEXT:
					FVNext(oPtr);
					break;
				case SOURCE_GOTO:
					DoTextViewGoto(NULL,oPtr,0);
					break;
				case SOURCE_SETBREAK:
					AddBrkPt(stSld->SourceToAddress(sPtr->currentFileName,oPtr->data1+pWindow->cursorY),0,NULL);
					break;
				case SOURCE_SETBREAKONCE:
					break;
				case SOURCE_CLEARBREAK:
					BreakClear(stSld->SourceToAddress(sPtr->currentFileName,oPtr->data1+pWindow->cursorY));
					break;
				case SOURCE_CLEARALLBREAKS:
					BreakClearAll();
					break;
				case SOURCE_RUNTOHERE:
					AddBrkPt(stSld->SourceToAddress(sPtr->currentFileName,oPtr->data1+pWindow->cursorY),1,NULL);
					DoSlaveRun();
					break;
				case SOURCE_GOTOSYMBOL:
					SourceGotoSymbol(NULL,oPtr,0);
					break;
			 }
			break;
		case INP_KEY:
			switch(in->fullKey)
	 		 {
				case CMD_CLOSEWINDOW:
					closeWindow = boolean::TRUE;
					break;
				case KEY_HOME:
					pWindow->cursorY = 1;
					break;
				case KEY_END:
					pWindow->cursorY = pWindow->ySize-2;
					break;
				case KEY_LEFT:
					if(oPtr->data3 > 0)
						oPtr->data3--;
					break;
				case KEY_RIGHT:
					if(oPtr->data3 < 255)
						oPtr->data3++;
					break;
				case KEY_UP:
					if(pWindow->cursorY > 1)
						pWindow->cursorY--;
					else if(oPtr->data1 > 0)
						oPtr->data1--;
					break;
				case KEY_DOWN:
					if(pWindow->cursorY < pWindow->ySize-2 && pWindow->cursorY+oPtr->data1 < (oPtr->data2-1))
						pWindow->cursorY++;
					else if(oPtr->data1 < oPtr->data2 - (signed int)(pWindow->ySize-2))
						oPtr->data1++;
					break;
				case KEY_PGUP:
					oPtr->data1 -= pWindow->ySize-2;
					if(oPtr->data1 < 0)
						oPtr->data1 = 0;
					break;
				case KEY_PGDWN:
					oPtr->data1 += pWindow->ySize-2;
					if(oPtr->data1 >  oPtr->data2 - (pWindow->ySize-2))
						oPtr->data1 = oPtr->data2 - (pWindow->ySize-2);
					break;
				case KEY_CTRLPGUP:
					oPtr->data1 = 0;
					break;
				case KEY_CTRLPGDWN:
					oPtr->data1 = oPtr->data2 - (pWindow->ySize-2);
					break;
				case CMD_LOCALMENU:
					mPtr = CreateMenuWithItems(sourceMenu,"Source");
					mPtr->xPos = AutoPosition(pWindow->xPos,pWindow->xSize,mPtr->xSize,displayWidth);
					mPtr->yPos = AutoPosition(pWindow->yPos,pWindow->ySize,mPtr->ySize,displayHeight);
					DoMenu(mPtr,oPtr);
					break;
				case CMD_OPENWINDOWHELPFILE:
					OpenRefFile(EXT_HELP,HELPNAME,"Source:");
					break;
				default:
					inputUsed = boolean::FALSE;
	 		 }
			break;
		case INP_MOUSEMOVE:
//			oPtr->Address() = ((long)GetNormalizedPropGadgPos(oPtr,FVGAD_SBOX,in->mouseY - pWindow->yPos,65535) * 256);
//			changed = boolean::TRUE;
			inputUsed = boolean::FALSE;
			break;
		case INP_MOUSE_LEFTBUTTON_UP:
//			if(PropGadgUp(oPtr,in))
//				changed = boolean::TRUE;
//			else
				inputUsed = boolean::FALSE;
			break;
		case INP_MOUSE_LEFTBUTTON_DOWN:
			if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
			 {
				switch(gPtr->gNum)
				 {
					case GGAD_CLOSE:
						closeWindow = boolean::TRUE;
						break;
//					case FVGAD_UP:
//					case FVGAD_DOWN:
//					case  FVGAD_SBOX:
//						PropGadgDown(in,gPtr);
//						changed = boolean::TRUE;
//						break;
					default:
						inputUsed = boolean::FALSE;
						break;

				 }
			 }
			else
			 {
				xPos = in->mouseX - pWindow->xPos;
				yPos = in->mouseY - pWindow->yPos;
				if(xPos > 0 && xPos < (pWindow->xSize-1) && yPos < (pWindow->ySize-1))
					pWindow->cursorY = yPos;
			 }
			break;
		case INP_MOUSE_RIGHTBUTTON_DOWN:
			mPtr = CreateMenuWithItems(sourceMenu,"Source");
//			mPtr->xPos = AutoPosition(pWindow->xPos,pWindow->xSize,mPtr->xSize,displayWidth);
//			mPtr->yPos = AutoPosition(pWindow->yPos,pWindow->ySize,mPtr->ySize,displayHeight);
			DoLocalMenu(mPtr,oPtr,in);
			break;
		case INP_RESIZE:
			AddTextViewGadgets(pWindow);
			break;
		case INP_COMMAND:
			SourceCommand(in->cmdText,oPtr,in->errPtr);
			break;
		default:
			inputUsed = boolean::FALSE;

	 }
	if(closeWindow)
	 {
		((_viewText *)oPtr->dataPtr)->DeleteLines();
		delete pWindow;
//		CloseWindow(pWindow);
		delete oPtr;
//		DeleteObject(oPtr);
		sourceOpen = boolean::FALSE;
		sourceObjPtr = NULL;

		sld->MarkInvalid();

//		if(sld->file)				// kts 02-01-93 00:06am
//			fclose(sld->file);
//		sld->file = NULL;
		inputUsed = boolean::TRUE;
	 }
	if(changed)
	 {
//		UpdateVScrollGadg(oPtr,FVGAD_SBOX,(int)(oPtr->Address()/256));
		DrawGadgets(pWindow);
	 }
    return(inputUsed);
}

//============================================================================

void
SourceRoutine(_object *oPtr)
{
	errorcode error;
	_sld *sld = stSld;
	_source *sPtr = (_source *)oPtr;

	if(slaveRunning)					// if running, don't bother
		return;

	sld->ReSyncSource(GetReg(REG_INSTRUCTIONPOINTER));

	 // now re-sync the file view
	if(strcmp(sPtr->currentFileName,sld->SourceFileName()))
	 {
		strcpy(sPtr->currentFileName,sld->SourceFileName());
		if(*sld->SourceFileName() != 0)
		 {
	   		error = TextViewLoad(oPtr,sld->SourceFileName(),sourcePath);
			if(error)
				PrintError(error);
		 }
	 }

	if(sld->SourceLine() != oPtr->data1+oPtr->data4)
	 {
		_layer *pWindow = (_window *)oPtr->layer;
		int startLine;
		startLine = sld->SourceLine() - ((pWindow->ySize-2)/2);
		if(startLine <= 1)
			startLine = 1;
		TextViewGotoLine(oPtr,startLine);
		pWindow->cursorY = (sld->SourceLine() - oPtr->data1);
	 }

	oPtr->data4 = sld->SourceLine() - oPtr->data1;
	TextViewRoutine(oPtr);
}

//============================================================================

struct _inputKeyRemap sourceKeys[] =
{
	{KEY_CTRLL,SOURCE_LOAD},
	{KEY_CTRLF,SOURCE_SEARCH},
	{KEY_CTRLR,SOURCE_SEARCHLABEL},
	{KEY_CTRLN,SOURCE_NEXT},
	{KEY_CTRLG,SOURCE_GOTO},

	{KEY_CTRLS,SOURCE_SETBREAK},
	{KEY_CTRLO,SOURCE_SETBREAKONCE},
	{KEY_CTRLC,SOURCE_CLEARBREAK},
	{KEY_CTRLA,SOURCE_CLEARALLBREAKS},
	{KEY_CTRLH,SOURCE_RUNTOHERE},
	{KEY_CTRLY,SOURCE_GOTOSYMBOL},
	{0,0}
};

//============================================================================

FLAG
OpenSource(void)
	{
	_viewText* tPtr;
   _object* oPtr;
   _window* pWindow;

	if ( sourceOpen )
		{
		sourceObjPtr->layer->ToFront();
		ActivateFrontWindow();
		return( boolean::TRUE );
		}

	if ( tPtr = (_viewText *)malloc(sizeof(_viewText)) )
		{
		sourceOpen = boolean::TRUE;

		if ( !stSld )
			{
#if defined( DEBUGDR )
			stSld = (_sld *)new drSld();
#elif defined( DEBUGZARDOZ )
			stSld = (_sld*)new zardozSld();
#endif
			}

		if ( !stSld )
			{
			// ? error message ?
			return( boolean::FALSE );
			}

		stSld->MarkInvalid();

		oPtr = (_object *) new _source;
		sourceObjPtr = (_source *)oPtr;
    	pWindow = new _window(5,2,60,20,"Source",(unsigned char)sourceAttr,(char)sourceChar);
		pWindow->data = (void *)oPtr;
		pWindow->xMin = 4;
		pWindow->xMax = displayWidth;
		pWindow->yMin = 4;					// leave enough room for up/down gadgets
		pWindow->yMax = displayHeight;
		pWindow->cursorY = 1;
		pWindow->fillChar = sourceChar;
		pWindow->charAttr = sourceAttr;
    	oPtr->inputRoutine = SourceInput;
		oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;
		oPtr->layer = pWindow;
		oPtr->data1 = 0;
		oPtr->data2 = 0;
		oPtr->data3 = 0;
		oPtr->data4 = 1;

		oPtr->remap.keyArray = sourceKeys;

		InitListBase((_list *)tPtr);
		oPtr->dataPtr = (char *)tPtr;
		oPtr->dataPtr2 = NULL;
		AddSysGadgets(pWindow);
		DrawGadgets(pWindow);
		ActivateFrontWindow();
		return(boolean::TRUE);
		}
	return(boolean::FALSE);
	}

//============================================================================

