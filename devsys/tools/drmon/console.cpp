//=============================================================================
//  console.cpp: debugging text display window
//  scrolling printing window
//=============================================================================

#include <stdarg.h>

#include "moninc.hpp"
#include "display.hpp"
#include "help.hpp"

//=============================================================================

enum
	{
	STGAD_UP = GAD_USER+1,
	STGAD_DOWN,
	STGAD_SCROLL,
	STGAD_SBOX
	};

//=============================================================================

static FLAG statusOpen = boolean::FALSE;
static _object *statusObjPtr = NULL;

static int statusLinesSaved = 300;			// History
static int statLineCount = 0;

_statText statTextBase;

//=============================================================================

void
StatusRoutine( _object* oPtr )
	{
	_statText* tPtr;
	int ySize,lineCount;

	ySize = oPtr->layer->ySize-2;

	tPtr = (_statText *)statTextBase.link.next;
	if(!tPtr)
		return;
	lineCount = CountListItems((_list *)tPtr);
	if(lineCount >= ySize)
		tPtr = (_statText *)SkipNodes((_list *)tPtr,lineCount-(ySize-1));

	FillWindow(oPtr->layer);

#if 1
	for ( int i=0; i<ySize && tPtr; ++i, tPtr=(_statText*)tPtr->link.next )
		PrintLayAttrClip(oPtr->layer,tPtr->textPtr,1,i+1,tPtr->charAttr,oPtr->layer->xSize-1);
#else
	i = 0;
	while(i<ySize && tPtr != 0)
		{
		PrintLayAttrClip(oPtr->layer,tPtr->textPtr,1,i+1,tPtr->charAttr,oPt6r->layer->xSize-1);
		i++;
		tPtr = (_statText *)tPtr->link.next;
		}
#endif
	}

//=============================================================================

void
AddStatusGadgets(_window* pWindow)
{
	_gadget *gPtr;
	AddVScrollGadgets(pWindow,STGAD_UP,STGAD_DOWN,STGAD_SCROLL,STGAD_SBOX);
}

//=============================================================================

FLAG
StatusInput(_input *in,_object *oPtr)
{
    _window *pWindow;
    FLAG inputUsed;
    unsigned char far *chr;
	_gadget *gPtr;

    pWindow = (_window *)oPtr->layer;
    inputUsed = boolean::FALSE;


	switch(in->inputType)
	 {
		case INP_KEY:
			switch(in->fullKey)
	 	 	 {
				case CMD_CLOSEWINDOW:
					statusOpen = boolean::FALSE;
					statusObjPtr = NULL;
//					CloseWindow(pWindow);
					delete pWindow;
//					DeleteObject(oPtr);
					delete oPtr;
					inputUsed = boolean::TRUE;
					break;
				case CMD_CONSOLECLEAR:
					while(statLineCount)
						DeleteStatLine();
					FillWindow(oPtr->layer);
					break;
				case CMD_OPENWINDOWHELPFILE:
					OpenRefFile(EXT_HELP,HELPNAME,"Console:");
					break;
	 	 	 }
			break;
		case INP_MOUSE_LEFTBUTTON_DOWN:
			if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
			 {
				switch(gPtr->gNum)
				 {
					case GGAD_CLOSE:
						statusOpen = boolean::FALSE;
						statusObjPtr = NULL;
//						CloseWindow(pWindow);
						delete pWindow;
//						DeleteObject(oPtr);
						delete oPtr;
						inputUsed = boolean::TRUE;
						break;
					default:
						inputUsed = boolean::FALSE;
						break;
				 }
			 }
			else
			 {
		 		pWindow->cursorX = in->mouseX - pWindow->xPos;
		 		pWindow->cursorY = in->mouseY - pWindow->yPos;
			 }
			break;
		case INP_RESIZE:
			AddStatusGadgets(pWindow);
			break;
	 }
    return(inputUsed);
}

//=============================================================================

FLAG
OpenConsole(void)
{
    _object *oPtr;
    _window *pWindow;
	if(statusOpen == boolean::TRUE)
	 {
		statusObjPtr->layer->ToFront();
		ActivateFrontWindow();
		return(boolean::TRUE);
	 }

    oPtr = AddObject(StatusRoutine);
	statusOpen = boolean::TRUE;
	statusObjPtr = oPtr;
    pWindow = new _window(40,11,40,6,"Console",(unsigned char)statusAttr,(char)statusChar);

	pWindow->xMin = 15;
	pWindow->xMax = displayWidth;
	pWindow->yMin = 4;					// leave enough room for up/down gadgets
	pWindow->yMax = displayHeight;

	pWindow->data = (void *)oPtr;
    oPtr->inputRoutine = StatusInput;
	oPtr->layer = pWindow;
	oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;

	AddSysGadgets(pWindow);
	AddStatusGadgets(pWindow);
	DrawGadgets(pWindow);
	ActivateFrontWindow();
	return(boolean::TRUE);							// in case activated from menu
}

//=============================================================================

_statText *
NewStatLine(char *text,unsigned char attr)
{
	_statText *tPtr;
	tPtr = (_statText *)malloc(sizeof(_statText));
	if(tPtr)
	 {
		AddListNode((_list *)&statTextBase,(_list *)tPtr);
		tPtr->textPtr = text;
		tPtr->charAttr = attr;
		statLineCount++;
	 }
	return(tPtr);
}

//=============================================================================

void
DeleteStatLine(void)
{
	_statText *sPtr;

//	if(statLineCount < statusLinesSaved)
//		return;

	sPtr = (_statText *)statTextBase.link.next;
	if(sPtr)
	 {
		statLineCount--;
		free(sPtr->textPtr);
		DeleteListNode((_list *)sPtr);
	 }
}

//=============================================================================
// print into status window
// note: there can only be one status window open at a time!

void
PrintToConsoleWindow( char* fmt, ... )
	{
	va_list pArg;
	char buffer[ 256 ];
	char* text = buffer;

	va_start( pArg, fmt );
	vsprintf( buffer, fmt, pArg );
	va_end( pArg );

	char*tPtr,*tPtr2;
	int charCount,i;
	while(*text != 0)
	 {
		charCount = StrToNLLen(text);
		tPtr = (char*)malloc(charCount+1);
		if(tPtr)								// kts 10-30-92 01:13am
		 {
			tPtr2 = tPtr;
			for(i=0;i<charCount;++i)
				*tPtr2++ = *text++;
			*tPtr2 = 0;							// terminate line
			if(*text == '\n')
				text++;								// skip newline char
			if(statLineCount >= statusLinesSaved)
				DeleteStatLine();					// for each line added, delete a line
			NewStatLine(tPtr,statusAttr);
		 }
	 }

// causes problems
//	if(statusOpen == 0)
//		OpenStatus();
	}

//=============================================================================
