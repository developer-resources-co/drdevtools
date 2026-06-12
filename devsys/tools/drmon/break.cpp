//=============================================================================
//  break.cpp: breakpoint handling
//=============================================================================

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
#include 	"message.hpp"
#include	"menu.hpp"
#include	"break.hpp"
#include	"expr.hpp"
#include	"error.hpp"
#include	"symbol.hpp"
#include	"slaveio.hpp"
#include	"general.hpp"
#include	"drmon.hpp"
#include	"help.hpp"

//============================================================================

enum
	{
	BGAD_UP	= GAD_USER+1,
	BGAD_DOWN,
	BGAD_SCROLL,
	BGAD_SBOX,
	BGAD_LIST
	};

//============================================================================

#define BWIN_YSIZE 7

FLAG breakOpen = boolean::FALSE;
_object *breakObjPtr;

//=============================================================================

_breakList breakListBase;

//=============================================================================

_breakList *
FindBreak(ULONG addr)
{
	_breakList *bPtr;

	bPtr = (_breakList *)breakListBase.Next();
	while(bPtr)
	 {
		if(bPtr->addr == addr)
			break;
		bPtr = (_breakList *)bPtr->Next();
	 }
	if(bPtr && (bPtr->flags & BRKF_ACTIVE))
		return(bPtr);
	return(NULL);
}

//=============================================================================

void
SortBreakList(void)
{
	FLAG cnt;
	int i,sc;
	_stringList *list,*list2;
	_stringList *liPtr;

	liPtr = (_stringList *)&breakListBase;

	cnt = boolean::TRUE;
	while(cnt)
	 {
		list = (_stringList *)liPtr->Next();			// skip header
		cnt = boolean::FALSE;
		while((list2 = (_stringList *)list->Next()) != 0)
		 {
			if(((_breakList *)list)->addr > (((_breakList *)list2)->addr))
			 {
				RemoveListNode((_list *)list);
				InsertListNode((_list *)list2,(_list *)list);
				cnt = boolean::TRUE;
			 }
			else
				list = (_stringList *)list->Next();
			list2 = (_stringList *)list->Next();
		 }
	 }
}

//=============================================================================

void
AddBPt( ULONG addr, int count, char* conditionString )
	{
	_breakList* bPtr = new _breakList;

	if ( bPtr )
		{
		InsertListNode(GetLastNode((_list *)&breakListBase),(_list *)bPtr);
//		bPtr->Name((unsigned char *)NULL);
		bPtr->Condition(conditionString);
		bPtr->Address(addr);
		bPtr->inst = SlaveSetBkPt(addr);
		bPtr->count = count;
		bPtr->flags = BRKF_ACTIVE;

		SortBreakList();
	 	}
	}

//=============================================================================

void
AddBrkPt(ULONG addr,unsigned int count,char* conditionString)
{
	_object *oPtr;
	oPtr = breakObjPtr;

#ifdef	GENESIS
	addr &= 0xfffffffe;						// make even since 68000 is word aligned
#endif
	if(!FindBreak(addr))
	 {
		AddBPt(addr,count,conditionString);
		if(oPtr)
			ChangeListRect((_window*)oPtr->layer,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)&breakListBase);
	 }
}

//=============================================================================

void
BreakAddRout(ULONG breakAddr,void *dataPtr)
{
	AddBrkPt(breakAddr,0,NULL);
}

//=============================================================================

void
BreakSet(_object *oPtr)
{
	GetExpr(BreakAddRout,oPtr,"Enter BreakPoint address",oPtr->layer);
}

//=============================================================================

void
BreakAddOnceRout(ULONG breakAddr,void *dataPtr)
{
	AddBrkPt(breakAddr,1,NULL);
}

//=============================================================================

void
BreakSetOnce(_object *oPtr)
{
	GetExpr(BreakAddOnceRout,oPtr,"Enter break once address",oPtr->layer);
}

//=============================================================================

void
BreakAddCountRout(ULONG breakCount,void *dataPtr)
{
	AddBrkPt((ULONG)dataPtr,breakCount,NULL);
}

//=============================================================================

void
BreakSetGetCount(ULONG breakAddr, void *dataPtr)
{
	GetExpr(BreakAddCountRout,(void *)breakAddr,"Enter break count",((_object *)(dataPtr))->layer);
}

//=============================================================================

void
BreakSetCount(_object *oPtr)
{
	GetExpr(BreakSetGetCount,oPtr,"Enter break with count address",oPtr->layer);
}

//=============================================================================

void
BreakAddConditionalRout(ULONG addr,void *dataPtr)
{
	UnPadString( expString );
	AddBrkPt((ULONG)dataPtr,0,expString);
}

//=============================================================================

void
BreakSetGetCondition(ULONG breakAddr, void *dataPtr)
{
//	GetString(BreakAddConditionalRout,(void *)breakAddr,"Enter conditional expression",((_object *)(dataPtr))->layer);
    *expString = '\0';
	PadString(expString,EXPRLEN);
	GetExpr(BreakAddConditionalRout,(void*)breakAddr,"Enter conditional expression",((_object *)(dataPtr))->layer);
}

//=============================================================================

void
BreakSetConditional(_object *oPtr)
{
	GetExpr(BreakSetGetCondition,oPtr,"Enter conditional break address",oPtr->layer);
}

//=============================================================================

void
BreakWinClear(_breakList *bPtr)
{
	SlaveBkClr(bPtr->addr,bPtr->inst); /* SLS 17 Jul 1991 */
	DeleteListNode((_list *)bPtr);
	if ( breakObjPtr )
		ChangeListRect((_window*)breakObjPtr->layer,(_listRectDesc *)breakObjPtr->dataPtr3,(_stringList *)&breakListBase);
}

//=============================================================================

void
BreakClear(ULONG addr)
	{
	_breakList* bPtr;
	if ( bPtr = FindBreak( addr ) )
		BreakWinClear( bPtr );
	}

//=============================================================================

FLAG
BreakGUIClear(_object *oPtr)
{
	_listRectDesc *lrPtr;
	_breakList *blPtr,*bPtr;
	int selected;

	blPtr = &breakListBase;
	lrPtr = (_listRectDesc *)breakObjPtr->dataPtr3;
	selected = lrPtr->selItem;

	if(blPtr->Next())
	 {
		bPtr = (_breakList *)SkipNodes((_list *)blPtr,selected+1);
		BreakWinClear(bPtr);
	 }
	return(boolean::TRUE);
}

//=============================================================================

FLAG
BreakGUIToggle(_object *oPtr)
{
	_listRectDesc *lrPtr;
	_breakList *blPtr,*bPtr;
	int selected;

	blPtr = &breakListBase;
	lrPtr = (_listRectDesc *)breakObjPtr->dataPtr3;
	selected = lrPtr->selItem;

	if(blPtr->Next())
	 {
		bPtr = (_breakList *)SkipNodes((_list *)blPtr,selected+1);
		if(bPtr->flags & BRKF_ACTIVE)
		 {
			bPtr->flags ^= BRKF_ACTIVE;
			SlaveBkClr(bPtr->addr,bPtr->inst);
			if ( breakObjPtr )
				ChangeListRect((_window*)breakObjPtr->layer,(_listRectDesc *)breakObjPtr->dataPtr3,(_stringList *)&breakListBase);
		 }
		else
		 {
			bPtr->flags ^= BRKF_ACTIVE;
			bPtr->inst = SlaveSetBkPt(bPtr->addr);
			if ( breakObjPtr )
				ChangeListRect((_window*)breakObjPtr->layer,(_listRectDesc *)breakObjPtr->dataPtr3,(_stringList *)&breakListBase);
		 }
	 }
	return(boolean::TRUE);
}

//=============================================================================

void
BreakClearAll(void)
{
	_object *oPtr;
	_breakList *bPtr,*blPtr;
	bPtr = &breakListBase;
	oPtr = breakObjPtr;
	blPtr = &breakListBase;

	while(blPtr->Next())
	 {
		bPtr = (_breakList *)blPtr->Next();
		SlaveBkClr(bPtr->addr,bPtr->inst); /* SLS 17 Jul 1991 */

		DeleteListNode((_list *)bPtr);
		if(oPtr)
			ChangeListRect((_window*)oPtr->layer,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)blPtr);
	 }
}

//=============================================================================

menuItems breakMenu[] =
{
	{"&Set Break...            Ctrl-S",SendWindowMessage,BREAK_SET},
	{"Set Break &Once...       Ctrl-O",SendWindowMessage,BREAK_SETONCE},
	{"Set Break with Cou&nt... Ctrl-N",SendWindowMessage,BREAK_SETCOUNT},
	{"Set Conditional Br&eak.. Ctrl-E",SendWindowMessage,BREAK_SETCOND},
	{"&Clear Break             Ctrl-C",SendWindowMessage,BREAK_CLEAR},
	{"Clear &All Breakpoints   Ctrl-A",SendWindowMessage,BREAK_CLEARALL},
	{"Toggle Breakpoin&t On/Off Ctrl-T",SendWindowMessage,BREAK_TOGGLE},
	{0,0,0}
};

//=============================================================================

void
BreakRoutine(_object *oPtr)
{
}

//=============================================================================

char *
BreakPrintListEntry(_stringList *stPtr, unsigned char *attr, unsigned* nLines )
{
	_breakList *bPtr;
	char *string = textBuffer;
	char check = ' ';

	_symbolList *sPtr;
	bPtr = (_breakList *)stPtr;

	if(bPtr->flags & BRKF_ACTIVE)
	 {
		*attr = ATTR_NORMAL;
		check = '';
	 }
	else
		*attr = ATTR_DKGREY;

	*string++ = check;
	string = PrintRaw32Bits(string,bPtr->addr);			// print address
	if(bPtr->count)
		string += sprintf(string," %d",bPtr->count);
	if((sPtr = FindHexSymbol(bPtr->addr)) != 0)			// print first matching symbol(if any)
	 {
		*string++ = ' ';
		string = PrintString(string,sPtr->Name());
	 }

	*nLines = 1;

	if(bPtr->Condition())
	 {
		*nLines = 2;
		string = PrintString(string,"\n  Cond:");
		string = PrintString(string,bPtr->Condition());
	 }
	*string = 0;
	return(textBuffer);
}

//=============================================================================

void
AddBreakGadgets(_window* pWindow,_object* oPtr)
{
	assert( pWindow );

	_gadget *gPtr;
	_listRectDesc *lrPtr;

	lrPtr = AddListRect(pWindow,pWindow->xSize-2,pWindow->ySize-2,(_stringList *)&breakListBase,BGAD_LIST);
	assert( lrPtr );
	lrPtr->drawListEntry = BreakPrintListEntry;
	oPtr->dataPtr3 = (char *)lrPtr;
	UpdateListRect(pWindow,lrPtr,BGAD_LIST);
}

//=============================================================================

FLAG
BreakInput(_input *in,_object *oPtr)
{
    _window *pWindow;
	_menu *mPtr;

    FLAG inputUsed;
	FLAG listIn;
    char *chr;
	_gadget *gPtr;

    pWindow = (_window *)oPtr->layer;
	assert( pWindow );
    inputUsed = boolean::TRUE;

	listIn = ListRectInput(in,oPtr,(_listRectDesc *)oPtr->dataPtr3,BGAD_LIST);
	if((listIn & LRIF_INPUTUSED) == boolean::FALSE)
		switch(in->inputType)
	 	{
			case INP_WINDOW_MESSAGE:
				switch(in->message)
				 {
					case BREAK_SET:
						BreakSet(oPtr);
						break;
					case BREAK_SETONCE:
						BreakSetOnce(oPtr);
						break;
					case BREAK_SETCOUNT:
						BreakSetCount(oPtr);
						break;
					case BREAK_SETCOND:
						BreakSetConditional(oPtr);
						break;
					case BREAK_CLEAR:
						BreakGUIClear(oPtr);
						break;
					case BREAK_CLEARALL:
						BreakClearAll();
						break;
					case BREAK_TOGGLE:
						BreakGUIToggle(oPtr);
						break;
				 }
				break;
			case INP_KEY:
				switch(in->fullKey)
	 	 	 	{
					case CMD_LOCALMENU:
						mPtr = CreateMenuWithItems(breakMenu,"BreakPoints");
						mPtr->xPos = AutoPosition(pWindow->xPos,pWindow->xSize,mPtr->xSize,displayWidth);
						mPtr->yPos = AutoPosition(pWindow->yPos,pWindow->ySize,mPtr->ySize,displayHeight);
						DoMenu(mPtr,oPtr);
						break;
					case CMD_CLOSEWINDOW:
						delete pWindow;
						delete (_listRectDesc *)oPtr->dataPtr3;
						DeleteObject(oPtr);
						breakOpen = boolean::FALSE;
						breakObjPtr = NULL;
						oPtr = NULL;
						inputUsed = boolean::TRUE;
						breakOpen = boolean::FALSE;
						break;
					case CMD_OPENWINDOWHELPFILE:
						OpenRefFile(EXT_HELP,HELPNAME,"BreakPoint:");
						break;
					default:
						inputUsed = boolean::FALSE;
	 	 	 	}
				break;
			case INP_MOUSEMOVE:
				inputUsed = boolean::FALSE;
				break;
			case INP_MOUSE_LEFTBUTTON_UP:
				break;

			case INP_MOUSE_LEFTBUTTON_DOWN:
				if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
			 	{
					switch(gPtr->gNum)
				 	{
						case GGAD_CLOSE:
							delete pWindow;
							delete (_listRectDesc *)oPtr->dataPtr3;
							DeleteObject(oPtr);
							oPtr = NULL;
							breakOpen = boolean::FALSE;
							breakObjPtr = NULL;
							inputUsed = boolean::TRUE;
							breakOpen = boolean::FALSE;
							break;
						default:
							inputUsed = boolean::FALSE;
							break;
				 	}
			 	}
				break;
			case INP_MOUSE_RIGHTBUTTON_DOWN:
				mPtr = CreateMenuWithItems(breakMenu,"BreakPoints");
//				mPtr->xPos = AutoPosition(pWindow->xPos,pWindow->xSize,mPtr->xSize,displayWidth);
//				mPtr->yPos = AutoPosition(pWindow->yPos,pWindow->ySize,mPtr->ySize,displayHeight);
				DoLocalMenu(mPtr,oPtr,in);
				break;

			case INP_RESIZE:
				delete (_listRectDesc *)oPtr->dataPtr3;
				AddBreakGadgets(pWindow,oPtr);
				break;
			default:
				inputUsed = boolean::FALSE;
	 	}

	if(inputUsed && oPtr)
	 {
		UpdateListRect(pWindow,(_listRectDesc *)oPtr->dataPtr3,BGAD_LIST);
		DrawGadgets(pWindow);
	 }
    return(inputUsed);
}

//=============================================================================

void
InitBreakPts()
{
	InitListBase((_list *)&breakListBase);
}

//=============================================================================

void
ValidateBreakPts(void)
{
	_object *oPtr;
	_breakList *bPtr,*blPtr;
	bPtr = &breakListBase;
	oPtr = breakObjPtr;
	blPtr = &breakListBase;

	while(blPtr->Next())
	 {
		bPtr = (_breakList *)blPtr->Next();
		if(MemRead(bPtr->addr,2) !=  0x4afc)
		 {
			DeleteListNode((_list *)bPtr);
		 }
		else
			blPtr = bPtr;
	 }
	if(oPtr)
		ChangeListRect((_window*)oPtr->layer,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)&breakListBase);
}

//=============================================================================

boolean
BreakPointHit(ULONG addr)
{
	_breakList *bPtr;
	ULONG result;
	if ( bPtr = FindBreak(addr ))
	 {
		if(bPtr->Condition())
		 {
	        if(!DoExp(bPtr->Condition()))
                result = exprAnswer;
	        else
	            result = 1;					// if expression failed, break

			if(!result)
				return(boolean::FALSE);		// continue if expression false
		 }

		if ( bPtr->count )
			if ( !( --bPtr->count ) )
				BreakWinClear( bPtr );
		return(boolean::TRUE);
	 }
	return(boolean::TRUE);
}

//=============================================================================

struct _inputKeyRemap breakKeys[] =
{
	{KEY_INSERT,BREAK_SET},
	{KEY_CTRLS,BREAK_SET},
	{KEY_DELETE,BREAK_CLEAR},
	{KEY_CTRLC,BREAK_CLEAR},
	{KEY_CTRLA,BREAK_CLEARALL},
	{KEY_CTRLT,BREAK_TOGGLE},
	{KEY_CTRLO,BREAK_SETONCE},
	{KEY_CTRLN,BREAK_SETCOUNT},
	{KEY_CTRLE,BREAK_SETCOND},
	{0,0}
};

//=============================================================================

FLAG
OpenBreak(void)
{
    _object *oPtr;
    _window *pWindow;

	if(breakOpen)
	{
		breakObjPtr->layer->ToFront();
		ActivateFrontWindow();
		return(boolean::TRUE);
	}

	if(oPtr = AddObject(BreakRoutine))
	 {
		if ( !(pWindow = new _window(59,17,21,BWIN_YSIZE,"Breakpoints",(unsigned char)breakAttr,(char)breakChar) ) )
			return( boolean::FALSE );

		breakOpen = boolean::TRUE;
		breakObjPtr = oPtr;

		pWindow->xMin = 12;
		pWindow->xMax = displayWidth;
		pWindow->yMin = 4;					// leave enough room for up/down gadgets
		pWindow->yMax = displayHeight;

		pWindow->data = (void*)oPtr;
	   	oPtr->inputRoutine = BreakInput;
		oPtr->layer = pWindow;
		oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;
		oPtr->remap.keyArray = breakKeys;

		pWindow->activeGadget = NULL;

		AddSysGadgets(pWindow);
		AddBreakGadgets(pWindow,oPtr);
		ActivateFrontWindow();
		DrawGadgets(pWindow);
		return(boolean::TRUE);							// in case activated from menu
	 }
	return(boolean::FALSE);
}

//=============================================================================

