//=============================================================================
//  watch.c: watch window handling
//=============================================================================
// dataPtr3-> listRectDesc
// dataPtr4-> stringBase(watchListBase)

#include <assert.h>

#include	"base.hpp"
#include	"global.hpp"
#include	"mem.hpp"
#include	"display.hpp"
#include	"config.hpp"
#include	"list.hpp"
#include	"keys.hpp"
#include	"monkeys.hpp"
#include	"input.hpp"
#include	"gadget.hpp"
#include	"object.hpp"
#include	"listrect.hpp"
#include	"layer.hpp"
#include	"window.hpp"
#include "message.hpp"
#include	"menu.hpp"
#include	"watch.hpp"
#include	"expr.hpp"
#include	"error.hpp"
#include	"slaveio.hpp"
#include	"general.hpp"
#include	"control.hpp"
#include	"command.hpp"
#include	"word.hpp"
#include	"drmon.hpp"
#include	"help.hpp"
#include	"symbol.hpp"

//=============================================================================

#define	WGAD_UP	GAD_USER+1
#define WGAD_DOWN WGAD_UP+1
#define WGAD_SCROLL WGAD_DOWN+1
#define WGAD_SBOX WGAD_SCROLL+1
#define WGAD_LIST WGAD_SBOX+1

#define WWIN_XPOS	29
#define WWIN_YPOS	17
#define WWIN_XSIZE	51
#define WWIN_YSIZE	7

//=============================================================================

errorcode
AddWatch(_object *oPtr,char *expr)
{
	_watchList *wPtr;
	char *string;

	wPtr = new _watchList;

//	if((wPtr = (_watchList *)malloc(sizeof(_watchList))) != 0)
	if(wPtr)
	 {
		wPtr->Name( NULL );
		string = DupString(expr);
		if(string)
		 {
	 		InsertListNode(GetLastNode((_list *)oPtr->dataPtr4),(_list *)wPtr);
			wPtr->expr = string;
			if ( oPtr )
				ChangeListRect((_window*)oPtr->layer,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)oPtr->dataPtr4);
			return(NOERR);
		 }
#if 0
		else
//			free(wPtr);
			delete wPtr;
#endif
	 }
	return(ERROR_NOMEM);
}

//=============================================================================

void
WatchAddRout(ULONG dummy,void *dataPtr)
{
	_object *oPtr = (_object *)dataPtr;
	errorcode error = AddWatch(oPtr,expString);
	if ( error )
		PrintError( error );
}

//=============================================================================

void
WatchSet(_object *oPtr)
{
	GetExpr(WatchAddRout,oPtr,"Enter Watch expression",oPtr->layer);
}

//=============================================================================

void
WatchClear(_object *oPtr,_watchList *wPtr)
{
	assert( oPtr );
	assert( wPtr );
	//free(wPtr->list.name);
	free(wPtr->expr);
	DeleteListNode((_list *)wPtr);
	ChangeListRect((_window*)oPtr->layer,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)oPtr->dataPtr4);
}

//=============================================================================

FLAG
WatchGUIClear(_object *oPtr)
{
	_listRectDesc *lrPtr;
	_watchList *wlPtr,*wPtr;
	int selected;

	wlPtr = (_watchList *)oPtr->dataPtr4;
	lrPtr = (_listRectDesc *)oPtr->dataPtr3;
	selected = lrPtr->selItem;

	if(wlPtr->Next())
	 {
		wPtr = (_watchList *)SkipNodes((_list *)wlPtr,selected+1);
		WatchClear(oPtr,wPtr);
	 }
	return(boolean::TRUE);
}

//=============================================================================

void
WatchClearAll(_object *oPtr)
{
	assert( oPtr );

	_watchList *wPtr,*wlPtr;
	wPtr = (_watchList *)oPtr->dataPtr4;
	wlPtr = wPtr;

	while(wlPtr->Next())
	 {
		wPtr = (_watchList *)wlPtr->Next();
		//free(wPtr->list.name);
		free(wPtr->expr);
		DeleteListNode((_list *)wPtr);
		ChangeListRect((_window*)oPtr->layer,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)wlPtr);
	 }
}

//============================================================================

menuItems watchMenu[] =
{
	{"&Set Watch...       Ctrl-S",SendWindowMessage,WATCH_INSERT},
	{"&Clear Watch        Ctrl-C",SendWindowMessage,WATCH_DELETE},
	{"Clear &All Watches  Ctrl-A",SendWindowMessage,WATCH_CLEARALL},
	{0,0,0}
};

//=============================================================================

void
WatchRoutine(_object *oPtr)
{
	assert( oPtr );

	_watch* pWatch = (_watch*)oPtr;
    unsigned int x,y,seg,pWidth;
    unsigned int *addr;
    _layer *lPtr;

	if(slaveUpdate)
	 {
		printCheckForSymbol = pWatch->PrintSymbols();
		ChangeListRect((_window*)oPtr->layer,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)oPtr->dataPtr4);
	 }
}

//=============================================================================

char *
WatchPrintListEntry(_stringList *stPtr,unsigned char *attr, unsigned* nLines )
{
	_watchList *wPtr;
	char *string;
	wPtr = (_watchList *)stPtr;
	string = textBuffer;

	*attr = ATTR_NORMAL;

	exprAnswer = 0;
	if(DoExp(wPtr->expr) || CheckSlaveAlive())
		string = PrintString(string,"????????");
	else
		string = PrintRaw32Bits(string,exprAnswer);
	string = PrintString(string,": ");

	_symbolList *sPtr;
	if(printCheckForSymbol && (sPtr = FindHexSymbol(exprAnswer)) != 0)			// print first matching symbol(if any)
	 {
		string = PrintString(string,sPtr->Name());
		*string++ = ':';
		*string++ = ' ';
	 }

	string = PrintString(string,wPtr->expr);

	*string = 0;
	*nLines = 1;

	return(textBuffer);
}

//=============================================================================

FLAG
AddWatchGadgets(_window *pWindow, _object *pObject)
{
	_listRectDesc *lrPtr;

	lrPtr = AddListRect(pWindow,pWindow->xSize-2,pWindow->ySize-2,(_stringList *)pObject->dataPtr4,WGAD_LIST);
	pObject->dataPtr3 = (char*)lrPtr;
	if(lrPtr)
	 {
		lrPtr->drawListEntry = WatchPrintListEntry;
		return(boolean::TRUE);
	 }
	return(boolean::FALSE);
}

//=============================================================================

void
WatchCommand(char **s,_object *oPtr, errorcode *error)
{
	ULONG dummy;
	unsigned int word;

	*error = NOERR;
	while (*s && **s && !*error)
	{
		*s = ParseWord(*s, &word);
		switch (word & 0xff)
		{
			case RW_CLEAR:
				*s = ParseWord(*s,&word);
				if ((word & 0xff) != RW_ALL)
					*error = ERROR_SYNTAX;
				else
					WatchClearAll(oPtr);
				break;

			case RW_ADD:
			default:
				{
					char *s2;
					s2 = *s;
					*s = GetNumbers(*s,1,&dummy,error);
					if (!*error)
						AddWatch(oPtr, s2);
				}
				break;
		}
	}
	return;
}

//=============================================================================

FLAG
WatchInput(_input *in,_object *oPtr)
{
    _window *pWindow;
	_menu *mPtr;
	_watch* pWatch = (_watch*)oPtr;

    FLAG inputUsed;
	FLAG fileIn;
    unsigned char *chr;
	_gadget *gPtr;

    pWindow = (_window *)oPtr->layer;
	assert( pWindow );
    inputUsed = boolean::TRUE;

	fileIn = ListRectInput(in,oPtr,(_listRectDesc *)oPtr->dataPtr3,WGAD_LIST);
	if((fileIn & LRIF_INPUTUSED) == boolean::FALSE)
		switch(in->inputType)
	 	{
			case INP_WINDOW_MESSAGE:
				switch(in->message)
				 {
					case WATCH_INSERT:
						WatchSet(oPtr);
						break;
					case WATCH_DELETE:
						WatchGUIClear(oPtr);
						break;
					case WATCH_CLEARALL:
						WatchClearAll(oPtr);
						break;
				 }
				break;
			case INP_KEY:
				switch(in->fullKey)
	 	 	 	{
					case CMD_LOCALMENU:
						mPtr = CreateMenuWithItems(watchMenu,"WatchPoints");
						mPtr->xPos = AutoPosition(pWindow->xPos,pWindow->xSize,mPtr->xSize,displayWidth);
						mPtr->yPos = AutoPosition(pWindow->yPos,pWindow->ySize,mPtr->ySize,displayHeight);
						DoMenu(mPtr,oPtr);
						break;
					case CMD_CLOSEWINDOW:
						delete pWindow;
						WatchClearAll(oPtr);
						delete (_listRectDesc *)oPtr->dataPtr3;
						delete oPtr;
						oPtr = NULL;
						inputUsed = boolean::TRUE;
						break;
					case CMD_OPENWINDOWHELPFILE:
						OpenRefFile(EXT_HELP,HELPNAME,"Watch:");
						break;
					default:
						inputUsed = boolean::FALSE;
	 	 	 	}
				break;
			case INP_MOUSE_LEFTBUTTON_DOWN:
				if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
			 	{
					switch(gPtr->gNum)
				 	{
						case GGAD_CLOSE:
							delete pWindow;
							WatchClearAll(oPtr);
							delete (_listRectDesc *)oPtr->dataPtr3;
							delete pWatch;
							oPtr = NULL;
							ActivateFrontWindow();
							inputUsed = boolean::TRUE;
							break;
						default:
							inputUsed = boolean::FALSE;
							break;
				 	}
			 	}
				break;
			case INP_MOUSE_RIGHTBUTTON_DOWN:
				mPtr = CreateMenuWithItems(watchMenu,"WatchPoints");
				DoLocalMenu(mPtr,oPtr,in);
				break;
			case INP_RESIZE:
				delete (_listRectDesc *)oPtr->dataPtr3;
				if(!AddWatchGadgets(pWindow,oPtr))
					PrintError(ERROR_NOMEM);
				break;
			case INP_COMMAND:
				WatchCommand(in->cmdText,oPtr,in->errPtr);
				break;
			default:
				inputUsed = boolean::FALSE;
	 	}

	if(inputUsed && oPtr)
	 {
		UpdateListRect(pWindow,(_listRectDesc *)oPtr->dataPtr3,WGAD_LIST);
		DrawGadgets(pWindow);
	 }
    return(inputUsed);
}

//=============================================================================

struct _inputKeyRemap watchKeys[] =
{
	{KEY_INSERT,WATCH_INSERT},
	{KEY_DELETE,WATCH_DELETE},
	{CMD_SETWATCH,WATCH_INSERT},
	{CMD_CLEARWATCH,WATCH_DELETE},
	{CMD_CLEARALLWATCHS,WATCH_CLEARALL},
	{0,0}
};

//============================================================================

FLAG
OpenWatch(void)
{
    _object* oPtr;
    _window* pWindow;
	_watchList* wPtr;
	_watch* pWatch;

	pWatch = new _watch();
	assert(pWatch);

    oPtr = AddObject(WatchRoutine,pWatch);
    pWindow = new _window(WWIN_XPOS,WWIN_YPOS,WWIN_XSIZE,WWIN_YSIZE,"Watch",(unsigned char)watchAttr,(char)watchChar);

	assert( pWindow );
	pWindow->xMin = 12;
	pWindow->xMax = displayWidth;
	pWindow->yMin = 4;					// leave enough room for up/down gadgets
	pWindow->yMax = displayHeight;

	pWindow->data = (void *)oPtr;
    oPtr->inputRoutine = WatchInput;
	oPtr->layer = pWindow;
	oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;
	pWindow->activeGadget = NULL;
	oPtr->remap.keyArray = watchKeys;

	wPtr = new _watchList;
	assert(wPtr);

	if(wPtr)
	 {
		oPtr->dataPtr4 = (char*)wPtr;
		wPtr->Name( NULL );
		AddSysGadgets(pWindow);
		if(!AddWatchGadgets(pWindow,oPtr))
			PrintError(ERROR_NOMEM);
		ActivateFrontWindow();
		DrawGadgets(pWindow);
	 }
	else
	 {
		// kts write code here
	 }
	return(boolean::TRUE);							// in case activated from menu
}

//=============================================================================
