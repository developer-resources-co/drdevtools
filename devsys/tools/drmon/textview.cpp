//=============================================================================
//	textview.cpp: text file viewer
//=============================================================================
// dataPtr-> viewTextBase
// dataPtr2-> current file name
// data1 = currently top line displayed in file
// data2 = # of lines in file
// data3 = x scroll value
// data4 = line pc is on(only for source windows)
//=============================================================================

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
#include	"menu.hpp"
#include	"filereq.hpp"
#include	"message.hpp"
#include	"error.hpp"
#include	"command.hpp"
#include	"word.hpp"
#include	"textview.hpp"
#include	"drmon.hpp"
#include	"expr.hpp"
#include	"help.hpp"

//=============================================================================
// FV = FileView

enum
	{
	FVGAD_UP	= GAD_USER+1,
	FVGAD_DOWN,
	FVGAD_SCROLL,
	FVGAD_SBOX
	};

//=============================================================================

void
AddTextViewGadgets(_window* pWindow)
{
	AddVScrollGadgets(pWindow,FVGAD_UP,FVGAD_DOWN,FVGAD_SCROLL,FVGAD_SBOX);
}

//=============================================================================

void
_viewText::DeleteLines()
{
	_viewText *tPtr2;
	tPtr2 = (_viewText *)link.next;				// skip base

	while(tPtr2)
	 {
		delete tPtr2->textMemory;
		DeleteListNode((_list *)tPtr2);
		tPtr2 = (_viewText *)link.next;				// skip base
	 }
}

//=============================================================================

_viewText *
NewViewLine(_viewText *tBase,memory *tMem,unsigned char attr)
{
	_viewText *tPtr;

	tPtr = (_viewText *)malloc(sizeof(_viewText));
	if(tPtr)
	 {
		AddListNode((_list *)tBase,(_list *)tPtr);
		tPtr->textMemory = tMem;
		tPtr->charAttr = attr;
	 }
	return(tPtr);
}

//=============================================================================

errorcode
TextViewLoad(_object *oPtr, const char *fileName,const char *path)
{
	FILE *inFile = NULL;
	char lineBuffer[256];
	int i,j,lineCount = 0;
	unsigned char c;
	memory *buffPtr;
	_viewText *tPtr;
	_viewText *lastTPtr = NULL;

	_window *pWindow;
	char *string;

	tPtr = (_viewText *)oPtr->dataPtr;

	tPtr->DeleteLines();
	oPtr->data2 = oPtr->data1 = 0;

	const char* delimeters = ";";
	char* p;

	inFile = fopen(fileName,"r");

	if(path)
	 {
		char *pathPtr = DupString(path);
		for ( p=strtok(pathPtr,delimeters); p && !inFile; p=strtok(NULL,delimeters) )
	 	 {
			strcpy(lineBuffer,p);
			if(*(lineBuffer+strlen(lineBuffer)) != '\\')
				strcat(lineBuffer,"\\");
			strcat(lineBuffer,fileName);
			inFile = fopen(lineBuffer,"r");
	 	 }
		free(pathPtr);
	 }

	if(!inFile)
		return(ERROR_FILENOTFOUND);
	else
	 {
		PrintMessageBar("Loading Text File");
		string = DupString(fileName);
		UnPadString(string);						// kts waste
		pWindow = (_window*)oPtr->layer;
//		if(pWindow->title)
//			free(pWindow->title);
//		pWindow->title = string;
////		ForceWindowResize(pWindow,oPtr,oPtr->inputRoutine);		// redraw title bar to show name
//		if(oPtr == frontObj)
//			PrintTitle(pWindow,windowTitleHighlightAttr);
//		else
//			PrintTitle(pWindow,windowTitleAttr);

		if(oPtr == frontObj)
			PrintTitle(pWindow,windowTitleHighlightAttr);
		else
			PrintTitle(pWindow,windowTitleAttr);

		oPtr->dataPtr2 = string;
		i = 0;
		while(!feof(inFile))
		 {
			c = getc(inFile);
			lineBuffer[i++] = c;

			if(c == '\n')
			 {
				lineCount++;
				lineBuffer[i-1] = 0;					// null terminate string

				buffPtr = new memory( (ubyte*)lineBuffer, i );

				if(!buffPtr)
				 {
					tPtr->DeleteLines();
					return(ERROR_NOMEM);
				 }
				i = 0;
/*				if(!NewViewLine(tPtr,buffPtr,textViewAttr))*/
/*				 {*/
/*					delete buffPtr;*/
/*					tPtr->DeleteLines();*/
/*					return(ERROR_NOMEM);*/
/*				 }*/

				_viewText *tlPtr;

				tlPtr = (_viewText *)malloc(sizeof(_viewText));
				if(tlPtr)
				 {
					if(lastTPtr)
						AddListNode((_list*)lastTPtr,(_list*)tlPtr);
					else
						AddListNode((_list *)tPtr,(_list *)tlPtr);
					lastTPtr = tlPtr;
					tlPtr->textMemory = buffPtr;
					tlPtr->charAttr = textViewAttr;
				 }
				else
				 {
					delete buffPtr;
					tPtr->DeleteLines();
					return(ERROR_NOMEM);
				 }
			 }
		 }
		fclose(inFile);
		oPtr->data2 = lineCount;
		ClearMessageBar();
	}
	return NOERR;
}

//=============================================================================

void
TextViewLoadGUI(void *data,char *path,char *fileName)
{
	errorcode error;
	_object *oPtr;

	oPtr = (_object *)data;

	error = TextViewLoad(oPtr,fileName);

	if(error)
		PrintError(error);
}

//=============================================================================

FLAG
DoTextViewLoad(_menuItem *iPtr,_object *oPtr,int choice)
{
	int i;
	DoFileReq("Load Text File",10,1,TextViewLoadGUI,(void *)oPtr);
	return(boolean::TRUE);
}

//=============================================================================

FLAG
SearchLine(char *search,memory *textMemory)
{
	int i,start = 0;
	i = strlen(search);
	while(start < i)
	 {
		if(!textMemory->MemNCmp(start,(ubyte*)search,i))
			return(boolean::TRUE);
		start++;
	 }

	return(boolean::FALSE);
}

//=============================================================================

errorcode
TextViewFind(_object *oPtr,char *str)
{
	_viewText *tPtr;
	int i;

	i = oPtr->data1;
	tPtr = (_viewText *)SkipNodes((_list *)oPtr->dataPtr,i+1);

	UnPadString(str);
	while(tPtr)
	 {
		if(SearchLine(str,tPtr->textMemory))
		 {
			oPtr->data1 = i;					// move to new location in file
			oPtr->layer->cursorY = 1;					// kts 11/4/91
			return(NOERR);
		 }
		i++;
		tPtr = (_viewText *)tPtr->link.next;
	 }
	return(ERROR_STRINGNOTFOUND);
}

//=============================================================================

void
FVFind(_object *oPtr)
{
	errorcode error;
	PrintMessageBar("Searching");
	error = TextViewFind(oPtr,strString);
	if(error)
		PrintError(error);
	else
		ClearMessageBar();
}

//=============================================================================

void
FVSearch(char *string,void *dataPtr)
{
	_object *oPtr;
	oPtr = (_object *)dataPtr;
	FVFind(oPtr);
}

//=============================================================================

FLAG
DoTextViewSearch(_menuItem *iPtr,_object *oPtr,int choice)
{
	GetString(FVSearch,(void *)oPtr,"Enter Search String",oPtr->layer);
	return(boolean::TRUE);
}

//=============================================================================

void
FVNext(_object *oPtr)
{
	if(oPtr->data1 < oPtr->data2)
		oPtr->data1++;
	FVFind(oPtr);
}

//=============================================================================

FLAG
DoTextViewNext(_menuItem *iPtr,_object *oPtr,int choice)
{
	FVNext(oPtr);
	return(boolean::TRUE);
}

//=============================================================================

errorcode
TextViewFindLabel(_object *oPtr, char *str)
{
	_viewText *tPtr;
	int i,j;

	i = oPtr->data1;
	tPtr = (_viewText *)SkipNodes((_list *)oPtr->dataPtr,i+1);

	UnPadString(str);
	while(tPtr)
	 {
		j = strlen(str);
		if(!tPtr->textMemory->MemNCmp(0,(ubyte*)str,j))
		 {
			oPtr->data1 = i;					// move to new location in file
			oPtr->layer->cursorY = 1;					// kts 11/4/91
			return(NOERR);
		 }
		i++;
		tPtr = (_viewText *)tPtr->link.next;
	 }
	return(ERROR_STRINGNOTFOUND);
}

//=============================================================================

void
FVFindLabel(_object *oPtr)
{
	errorcode error;
	PrintMessageBar("Searching for label");
	error = TextViewFindLabel(oPtr,strString);
	if(error)
		PrintError(error);
	else
		ClearMessageBar();
}

//=============================================================================

void
FVSearchLabel(char *string,void *dataPtr)
{
	_object *oPtr;
	oPtr = (_object *)dataPtr;
	FVFindLabel(oPtr);
}

//=============================================================================

FLAG
DoTextViewSearchLabel(_menuItem *iPtr,_object *oPtr,int choice)
{
	GetString(FVSearchLabel,(void *)oPtr,"Enter Search String",oPtr->layer);
	return(boolean::TRUE);
}

//=============================================================================

void
TextViewGotoLine(_object *oPtr,ULONG newLine)
{
	if(--newLine < oPtr->data2)
	 {
		oPtr->data1 = newLine;
		oPtr->layer->cursorY = 1;					// kts 11/4/91
	 }
}

//=============================================================================

void
TextViewGotoGUIRoutine(ULONG newLine,void *dataPtr)
{
	TextViewGotoLine((_object *)dataPtr,newLine);
}

//=============================================================================

FLAG
DoTextViewGoto(_menuItem *iPtr,_object *oPtr,int choice)
{
	GetExpr(TextViewGotoGUIRoutine,(void *)oPtr,"Enter New Line #",oPtr->layer);
	return(boolean::TRUE);
}

//=============================================================================

menuItems textViewMenu[] =
{
	{"&Load File...       Ctrl-L",SendWindowMessage,TEXTVIEW_LOAD},
	{"&Search...          Ctrl-S",SendWindowMessage,TEXTVIEW_SEARCH},
	{"Sea&rch Label...    Ctrl-R",SendWindowMessage,TEXTVIEW_SEARCHLABEL},
	{"&Next               Ctrl-N",SendWindowMessage,TEXTVIEW_NEXT},
	{"&Goto...            Ctrl-G",SendWindowMessage,TEXTVIEW_GOTO},
	{0,0,0}
};

//=============================================================================

void
TextViewRoutine(_object *oPtr)
{
	_viewText *tPtr;
	memory *mPtr;
	char *sPtr;
	_layer *lPtr;
	int y,count,i,j,k;
	char *s;
	unsigned char c,charAttr;

	tPtr = (_viewText *)oPtr->dataPtr;
	tPtr = (_viewText *)tPtr->link.next;			// skip base

	lPtr = oPtr->layer;
	count = lPtr->ySize-2;

	// new code to not re-render if not nessicary

	// end of new code

	FillRectLayer( lPtr,
		1, 1, lPtr->xSize-2, lPtr->ySize-2,
		lPtr->fillChar, lPtr->charAttr );

	for(y=0;y<oPtr->data1;y++)
		tPtr = (_viewText *)tPtr->link.next;

	y = 1;
	while(tPtr && count--)
	 {
		i = k = 0;
		mPtr = tPtr->textMemory;
		while(mPtr->ReadMem(k))
		 {
			c = mPtr->ReadMem(k++);
			if(c == '\t')				// expand tabs before displaying them
			 {
				j = i;
				while(i < ((j - (j % 8)) + 8))
					textBuffer[i++] = ' ';
			 }
			else
				textBuffer[i++] = c;
		 }
		textBuffer[i] = 0;

		if(oPtr->routine == TextViewRoutine)			// do text view
		 {
			if(strlen(textBuffer) > oPtr->data3)
				PrintLayAttrClip(lPtr,textBuffer+oPtr->data3,1,y,lPtr->charAttr,lPtr->xSize-1);
		 }
		else											// do source print
		 {
			charAttr = lPtr->charAttr;
			if(y == oPtr->data4)
			 {
				charAttr = sourcePCAttr;
				PadString(textBuffer,80);					// make very long highlight bar
			 }
			if(y == lPtr->cursorY)
			 {
				charAttr = sourceCursorAttr;
				PadString(textBuffer,80);					// make very long highlight bar
			 }

			if(strlen(textBuffer) > oPtr->data3)
				PrintLayAttrClip(lPtr,textBuffer+oPtr->data3,1,y,charAttr,lPtr->xSize-1);
		 }
		tPtr = (_viewText *)tPtr->link.next;
		y++;
	 }

							// time to render current line # in title bar
	if(oPtr->routine == TextViewRoutine)			// do text view
		{
   	sPtr = PrintString(textBuffer,"Col: ");
		sPtr  = PrintDecimal16Bits(sPtr,oPtr->data3+lPtr->cursorX);
		}
	else
		sPtr = textBuffer;

	sPtr = PrintString(sPtr," Line: ");
	sPtr  = PrintDecimal16Bits(sPtr,oPtr->data1+lPtr->cursorY);
	sPtr = PrintString(sPtr,"/");
	*PrintDecimal16Bits(sPtr,oPtr->data2) = 0;
	sPtr = textBuffer;
	if(strlen(sPtr) > (lPtr->xSize-3))
		sPtr += (strlen(sPtr) - (lPtr->xSize-3));
	PrintLayAttr(lPtr,sPtr,lPtr->xSize-(strlen(sPtr)+2),lPtr->ySize-1,windowBorderAttr);

	int x;
	if(lPtr->title)
		x = strlen(lPtr->title) + 4;
	else
		x = 4;

	if(oPtr->dataPtr2)
		PrintLayAttrClip(lPtr,oPtr->dataPtr2,x,0,windowBorderAttr,lPtr->xSize-4);
}

//=============================================================================

void
TextViewCommand(char **s,_object *oPtr, errorcode *error)
{
	ULONG dummy;
	unsigned int word;
	ulong newLine;

	*error = NOERR;
	while (*s && **s && !*error)
	 {
		*s = ParseWord(*s, &word);
		switch (word & 0xff)
		 {
			// kts define this
//			case RW_GOTO:
//	            *s = GetNumbers(*s,1,&newLine,error);
//				TextViewGotoLine(oPtr,newLine);
//				break;
			case RW_WLOAD:
			default:
				UnPadString(*s);					// kts 01-07-93 09:49am
				*error = TextViewLoad(oPtr, *s);
				*s=NULL;
				break;
		 }
	 }
	return;
}

//=============================================================================

void
TextViewClipCursor(_window *pWindow)
{
	if(pWindow->cursorX >= pWindow->xSize-2)
		pWindow->cursorX = pWindow->xSize-2;
	if(pWindow->cursorY >= pWindow->ySize-2)
		pWindow->cursorY = pWindow->ySize-2;
}

//=============================================================================

FLAG
TextViewInput(_input *in,_object *oPtr)
{
    _window *pWindow;
	_menu *mPtr;
	_menuItem *iPtr;
	_gadget  *gPtr;

	ULONG temp;
	unsigned int xPos,yPos,pWidth;

    FLAG inputUsed;
	FLAG changed = boolean::TRUE;
    char *chr;

    pWindow = (_window *)oPtr->layer;
    inputUsed = boolean::TRUE;
	switch(in->inputType)
	 {
		case INP_WINDOW_MESSAGE:
			switch(in->message)
			 {
				case TEXTVIEW_LOAD:
					DoFileReq("Load Text File",10,1,TextViewLoadGUI,(void *)oPtr);
					break;
				case TEXTVIEW_SEARCH:
					GetString(FVSearch,(void *)oPtr,"Enter Search String",pWindow);
					break;
				case TEXTVIEW_SEARCHLABEL:
					GetString(FVSearchLabel,(void *)oPtr,"Enter Search String",oPtr->layer);
					break;
				case TEXTVIEW_NEXT:
					FVNext(oPtr);
					break;
				case TEXTVIEW_GOTO:
					DoTextViewGoto(NULL,oPtr,0);
					break;
			 }
			break;

		case INP_KEY:
			switch(in->fullKey)
	 		 {
				case CMD_CLOSEWINDOW:
					((_viewText *)oPtr->dataPtr)->DeleteLines();
					delete pWindow;
					delete oPtr;
					inputUsed = boolean::TRUE;
					break;
				case KEY_HOME:
					pWindow->cursorY = 1;
					break;
				case KEY_END:
					pWindow->cursorY = pWindow->ySize-2;
					break;
				case KEY_LEFT:
					if(pWindow->cursorX > 1)
						pWindow->cursorX--;
					else if(oPtr->data3 > 0)
						oPtr->data3--;
					break;
				case KEY_RIGHT:
					if(pWindow->cursorX < pWindow->xSize-2)
						pWindow->cursorX++;
					else if(oPtr->data3+pWindow->cursorX < 255)
						oPtr->data3++;
					break;
				case KEY_UP:
					if(pWindow->cursorY > 1)
						pWindow->cursorY--;
					else if(oPtr->data1 > 0)
						oPtr->data1--;
					break;
				case KEY_DOWN:
					if(pWindow->cursorY < pWindow->ySize-2 && pWindow->cursorY < oPtr->data2)
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
					mPtr = CreateMenuWithItems(textViewMenu,"TextView");
					mPtr->xPos = AutoPosition(pWindow->xPos,pWindow->xSize,mPtr->xSize,displayWidth);
					mPtr->yPos = AutoPosition(pWindow->yPos,pWindow->ySize,mPtr->ySize,displayHeight);
					DoMenu(mPtr,oPtr);
					break;
				case CMD_OPENWINDOWHELPFILE:
					OpenRefFile(EXT_HELP,HELPNAME,"TextView:");
					break;
				default:
					inputUsed = boolean::FALSE;
					changed = boolean::FALSE;
	 		 }
			break;
		case INP_MOUSEMOVE:
			oPtr->data1 = ((long)GetNormalizedPropGadgPos(pWindow,FVGAD_SBOX,in->mouseY - pWindow->yPos,oPtr->data2));
			if(oPtr->data1 >  oPtr->data2 - (pWindow->ySize-2))
				oPtr->data1 = oPtr->data2 - (pWindow->ySize-2);
			inputUsed = boolean::FALSE;
			break;
		case INP_MOUSE_LEFTBUTTON_UP:
			if(!PropGadgUp(oPtr,in))
			 {
				inputUsed = boolean::FALSE;
				changed = boolean::FALSE;
			 }
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
					case  FVGAD_UP:
					if(oPtr->data1 > 0)
						oPtr->data1--;
						break;
					case  FVGAD_DOWN:
					if(oPtr->data1 < oPtr->data2 - (signed int)(pWindow->ySize-2))
						oPtr->data1++;
						break;
					case  FVGAD_SBOX:
						PropGadgDown(oPtr,in,gPtr);
						break;
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
				 {
					pWindow->cursorX = xPos;
					pWindow->cursorY = yPos;
				 }
			 }
			break;
		case INP_MOUSE_RIGHTBUTTON_DOWN:
			mPtr = CreateMenuWithItems(textViewMenu,"TextView");
//			mPtr->xPos = AutoPosition(pWindow->xPos,pWindow->xSize,mPtr->xSize,displayWidth);
//			mPtr->yPos = AutoPosition(pWindow->yPos,pWindow->ySize,mPtr->ySize,displayHeight);
			DoLocalMenu(mPtr,oPtr,in);
			break;
		case INP_RESIZE:
			AddTextViewGadgets(pWindow);
			TextViewClipCursor(pWindow);
			break;
		case INP_COMMAND:
			TextViewCommand(in->cmdText,oPtr,in->errPtr);
			break;
		default:
			changed = boolean::FALSE;
			inputUsed = boolean::FALSE;

	 }
	if(changed)
	 {
		int temp = oPtr->data2 - (pWindow->ySize-2);
		if(temp < 0)
			temp = 0;
		temp = temp ? 65536/temp : 0;
		UpdateVScrollGadg(pWindow,FVGAD_SBOX,(int)(oPtr->data1*temp),
			oPtr->data2 < pWindow->ySize ? ~0 : pWindow->ySize * temp );
		DrawGadgets(pWindow);
	 }
    return(inputUsed);
}

//============================================================================

struct _inputKeyRemap textviewKeys[] =
{
	{KEY_CTRLL,TEXTVIEW_LOAD},
	{KEY_CTRLF,TEXTVIEW_SEARCH},
	{KEY_CTRLR,TEXTVIEW_SEARCHLABEL},
	{KEY_CTRLN,TEXTVIEW_NEXT},
	{KEY_CTRLG,TEXTVIEW_GOTO},
	{0,0}
};

//=============================================================================

FLAG
OpenTextView(void)
{
	_viewText *tPtr;
    _object *oPtr;
    _window *pWindow;

	tPtr = (_viewText *)malloc(sizeof(_viewText));
	if(tPtr)
	 {
    	oPtr = AddObject(TextViewRoutine);
    	pWindow = new _window(5,2,60,20,"TextView",(unsigned char)textViewAttr,(char)textViewChar);
		pWindow->data = (void *)oPtr;
		pWindow->xMin = 4;
		pWindow->xMax = displayWidth;
		pWindow->yMin = 4;					// leave enough room for up/down gadgets
		pWindow->yMax = displayHeight;
		pWindow->cursorX = 1;
		pWindow->cursorY = 1;
    	oPtr->inputRoutine = TextViewInput;
		oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;
		oPtr->layer = pWindow;
		oPtr->data1 = 0;
		oPtr->data2 = 0;
		oPtr->data3 = 0;
		InitListBase((_list *)tPtr);
		oPtr->dataPtr = (char*)tPtr;
		oPtr->dataPtr2 = "(none)";
		oPtr->remap.keyArray = textviewKeys;
		pWindow->cursorOn = boolean::TRUE;
		AddSysGadgets(pWindow);
		AddTextViewGadgets(pWindow);
		DrawGadgets(pWindow);
		ActivateFrontWindow();
		return(boolean::TRUE);
	 }
	return(boolean::FALSE);
}

//=============================================================================

FLAG
OpenHelpFile(void)
{
	char *t;
    _object *oPtr;
	char fileName[_MAX_PATH+13];

	strcpy(fileName,progName);
	t = FindNull(fileName) - 3;
	strcpy(t,"HLP");
	if(OpenNamedTextFile(fileName,"Help",60,20))
		return(boolean::TRUE);
	return(boolean::FALSE);
}


//=============================================================================

_object *
OpenNamedTextFile(char *name,char *title, unsigned int xSize, unsigned int ySize)
{
	_viewText *tPtr;
    _object *oPtr;
    _window *pWindow;

	tPtr = (_viewText *)malloc(sizeof(_viewText));
	if(tPtr)
	 {
    	oPtr = AddObject(TextViewRoutine);
    	pWindow = new _window(5,2,xSize,ySize,title,(unsigned char)textViewAttr,(char)textViewChar);
		pWindow->data = (void *)oPtr;
		pWindow->xMin = 4;
		pWindow->xMax = displayWidth;
		pWindow->yMin = 4;					// leave enough room for up/down gadgets
		pWindow->yMax = displayHeight;
		pWindow->cursorX = 1;
		pWindow->cursorY = 1;
    	oPtr->inputRoutine = TextViewInput;
		oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;
		oPtr->layer = pWindow;
		oPtr->data1 = 0;
		oPtr->data2 = 0;
		oPtr->data3 = 0;
		InitListBase((_list *)tPtr);
		oPtr->dataPtr = (char*)tPtr;
		oPtr->dataPtr2 = "(none)";
		oPtr->remap.keyArray = textviewKeys;
		pWindow->cursorOn = boolean::TRUE;
		AddSysGadgets(pWindow);
		AddTextViewGadgets(pWindow);
		DrawGadgets(pWindow);
		ActivateFrontWindow();

		TextViewLoadGUI((void *)oPtr,"",name);
		return(oPtr);
	 }
	return(NULL);
}

//=============================================================================
