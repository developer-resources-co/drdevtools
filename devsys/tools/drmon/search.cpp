//=============================================================================
//  search.cpp:
//=============================================================================

#include	"base.hpp"
#include	"global.hpp"
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
#include	"search.hpp"
#include	"display.hpp"
#include	"expr.hpp"
#include	"error.hpp"
#include	"general.hpp"
#include	"memops.hpp"
#include	"drmon.hpp"
#include	"monwind.hpp"

// Declared in slaveio.hpp, which search.cpp does not include (it uses explicit
// headers, and slaveio.hpp is unguarded).  slaveio.hpp wraps the slave calls in
// extern "C", so match that linkage or the reference goes unresolved at link time.
extern "C" void ReadSlaveData(unsigned long addr, char far *data, unsigned int len);

//=============================================================================

// ---------------------------------------------------------------------------
// Scrollable search-results window.  Mirrors the live symbol/break list windows
// (same AddListRect / ListRectInput / _stringList / _object plumbing).  Hits from
// MemSearchGUI land in searchListBase; Enter/Ctrl-G (or the local menu) on a row
// opens a Memory window at that address.
// ---------------------------------------------------------------------------

enum
	{
	SGAD_UP = GAD_USER+1,
	SGAD_DOWN,
	SGAD_SCROLL,
	SGAD_SBOX,
	SGAD_LIST
	};

_searchList searchListBase;             // list head (results live here)
_object    *searchObjPtr = NULL;        // the open Search window, or NULL
static FLAG searchOpen    = boolean::FALSE;

static void SearchClearList(void);

//=============================================================================

// Row formatter: print the hit address.
char *
SearchPrintListEntry(_stringList *stPtr,unsigned char *attr, unsigned* nLines )
{
	_searchList *sPtr = (_searchList *)stPtr;
	char *string = textBuffer;

	*attr = ATTR_NORMAL;
	string = PrintRaw32Bits(string,sPtr->addr);
	*string = 0;

	*nLines = 1;
	return(textBuffer);
}

//=============================================================================

void
AddSearchGadgets(_window* pWindow,_object* oPtr)
{
	assert( pWindow );

	_listRectDesc *lrPtr;

	lrPtr = AddListRect(pWindow,pWindow->xSize-2,pWindow->ySize-2,(_stringList *)&searchListBase,SGAD_LIST);
	assert( lrPtr );
	lrPtr->drawListEntry = SearchPrintListEntry;
	oPtr->dataPtr3 = (char*)lrPtr;
	UpdateListRect(pWindow,lrPtr,SGAD_LIST);
}

//=============================================================================

menuItems searchMenu[] =
{
	{"&Goto address   Enter",SendWindowMessage,SEARCH_GOTO},
	{"&Clear list       Del",SendWindowMessage,SEARCH_CLEAR},
	{0,0,0}
};

// Ctrl-G / Enter -> goto, Del -> clear (routed to SearchInput as window messages).
struct _inputKeyRemap searchKeys[] =
{
	{KEY_CTRLG,SEARCH_GOTO},
	{KEY_RETURN,SEARCH_GOTO},
	{KEY_DELETE,SEARCH_CLEAR},
	{0,0}
};

//=============================================================================

// Free every result node and refresh the list-rect.
static void
SearchClearList(void)
{
	_searchList *base = &searchListBase;
	while(base->Next())
	 {
		_searchList *node = (_searchList *)base->Next();
		if(node->Name())
			free(node->Name());
		DeleteListNode((_list *)node);
	 }
	if(searchObjPtr)
		ChangeListRect((_window*)searchObjPtr->layer,(_listRectDesc *)searchObjPtr->dataPtr3,(_stringList *)&searchListBase);
}

// Open a Memory window at the currently-selected hit.
static void
SearchGotoSelected(_object *oPtr)
{
	_listRectDesc *lrPtr = (_listRectDesc *)oPtr->dataPtr3;
	if(!lrPtr || lrPtr->itemCount <= 0)
		return;
	_searchList *sPtr = (_searchList *)SkipNodes((_list *)lrPtr->listPtr,lrPtr->selItem+1);
	if(sPtr)
		OpenMemoryAt(sPtr->Address());
}

//=============================================================================

FLAG
SearchInput(_input *in,_object *oPtr)
{
    _window *pWindow;
	_menu *mPtr;

    FLAG inputUsed;
	FLAG listIn;
	_gadget *gPtr;

    pWindow = (_window *)oPtr->layer;
	assert( pWindow );
    inputUsed = boolean::TRUE;

	listIn = ListRectInput(in,oPtr,(_listRectDesc *)oPtr->dataPtr3,SGAD_LIST);
	if((listIn & LRIF_INPUTUSED) == boolean::FALSE)
		switch(in->inputType)
	 	{
			case INP_WINDOW_MESSAGE:
				switch(in->message)
				 {
					case SEARCH_GOTO:
						SearchGotoSelected(oPtr);
						break;
					case SEARCH_CLEAR:
						SearchClearList();
						break;
				 }
				break;
			case INP_KEY:
				switch(in->fullKey)
	 	 	 	{
					case CMD_LOCALMENU:
						mPtr = CreateMenuWithItems(searchMenu,"Search");
						mPtr->xPos = AutoPosition(pWindow->xPos,pWindow->xSize,mPtr->xSize,displayWidth);
						mPtr->yPos = AutoPosition(pWindow->yPos,pWindow->ySize,mPtr->ySize,displayHeight);
						DoMenu(mPtr,oPtr);
						break;
					case CMD_CLOSEWINDOW:
						delete pWindow;
						delete (_listRectDesc *)oPtr->dataPtr3;
						DeleteObject(oPtr);
						oPtr = NULL;
						searchOpen = boolean::FALSE;
						searchObjPtr = NULL;
						inputUsed = boolean::TRUE;
						break;
					default:
						inputUsed = boolean::FALSE;
	 	 	 	}
				break;
			case INP_MOUSE_LEFTBUTTON_DOWN:
				if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0
				   && gPtr->gNum == GGAD_CLOSE)
			 	{
					delete pWindow;
					delete (_listRectDesc *)oPtr->dataPtr3;
					DeleteObject(oPtr);
					oPtr = NULL;
					searchOpen = boolean::FALSE;
					searchObjPtr = NULL;
					inputUsed = boolean::TRUE;
			 	}
				else
					inputUsed = boolean::FALSE;
				break;
			case INP_MOUSE_RIGHTBUTTON_DOWN:
				mPtr = CreateMenuWithItems(searchMenu,"Search");
				DoLocalMenu(mPtr,oPtr,in);
				break;
			default:
				inputUsed = boolean::FALSE;
	 	}

	if(inputUsed && oPtr)
	 {
		UpdateListRect(pWindow,(_listRectDesc *)oPtr->dataPtr3,SGAD_LIST);
		DrawGadgets(pWindow);
	 }
    return(inputUsed);
}

//=============================================================================

FLAG
OpenSearchWindow(void)
{
	if(searchOpen)
	 {
		searchObjPtr->layer->ToFront();
		ActivateFrontWindow();
		return(boolean::TRUE);
	 }

	_object *oPtr = AddObject();
	if(!oPtr)
		return(boolean::FALSE);

	_window *pWindow = new _window(49,4,24,11,"Search List",(unsigned char)exprAttr,(char)exprChar);
	if(!pWindow)
		return(boolean::FALSE);

	searchOpen = boolean::TRUE;
	searchObjPtr = oPtr;

	pWindow->xMin = 12;
	pWindow->xMax = displayWidth;
	pWindow->yMin = 4;
	pWindow->yMax = displayHeight;

	pWindow->data = (void *)oPtr;
	oPtr->inputRoutine = SearchInput;
	oPtr->layer = pWindow;
	oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;
	oPtr->remap.keyArray = searchKeys;

	AddSysGadgets(pWindow);
	AddSearchGadgets(pWindow,oPtr);
	ActivateFrontWindow();
	DrawGadgets(pWindow);
	return(boolean::TRUE);
}

//=============================================================================

void
MemSearchGUI(ULONG value,void * /*dataPtr*/)
{
	// Client-side memory search (the MAME bridge has no search primitive — the
	// legacy SlaveCompMem ran on dev-cart firmware).  Collected during the input
	// chain: globTemp2 = start address, globTemp = # bytes to search, value = the
	// value to find.  The value is matched as a little-endian byte sequence of its
	// natural width (SNES is little-endian): 0x42 -> [42], 0x1234 -> [34 12].  (A
	// value whose high byte is zero is searched at the narrower width — enter a
	// wider range value if you need the explicit width.)  Hits land in the scrollable
	// Search List window (Enter/Ctrl-G on a row jumps a Memory window there).
	unsigned long start = (unsigned long)globTemp2;
	unsigned long len   = (unsigned long)globTemp;

	int w = 1;
	if      (value > 0xFFFFFFUL) w = 4;
	else if (value > 0xFFFFUL)   w = 3;
	else if (value > 0xFFUL)     w = 2;

	if (len < (unsigned long)w)
	 {
		PrintWarning("Search: range smaller than the value width");
		return;
	 }

	unsigned char pat[4];
	for (int i = 0; i < w; i++)
		pat[i] = (unsigned char)((value >> (8 * i)) & 0xFF);

	SearchClearList();                       // fresh results

	const unsigned int CHUNK   = 4096;
	const int          MAXHITS = 1000;       // cap the list (a NOP search can hit thousands)
	unsigned char buf[CHUNK];
	int nmatch = 0;
	int capped = 0;

	unsigned long addr = start;
	unsigned long end  = start + len;                 // exclusive
	while (addr + (unsigned long)w <= end)
	 {
		unsigned long room = end - addr;
		unsigned int  want = (unsigned int)(room < CHUNK ? room : CHUNK);
		ReadSlaveData(addr, (char far *)buf, want);

		unsigned int limit = want - (unsigned int)w + 1;   // valid start positions
		for (unsigned int i = 0; i < limit; i++)
		 {
			int hit = 1;
			for (int k = 0; k < w; k++)
				if (buf[i + k] != pat[k]) { hit = 0; break; }
			if (hit)
			 {
				if (nmatch < MAXHITS)
				 {
					_searchList *node = new _searchList;
					if (node)
					 {
						node->Address(addr + i);
						AddListNode((_list *)&searchListBase, (_list *)node);
					 }
				 }
				else
					capped = 1;
				nmatch++;
			 }
		 }
		if (want < CHUNK) break;                       // final (short) window done
		addr += CHUNK - (unsigned long)(w - 1);        // overlap w-1 for span hits
	 }

	// Show the scrollable results window (created or refreshed) + a one-line summary.
	if (nmatch > 0)
	 {
		OpenSearchWindow();
		if (searchObjPtr)
			ChangeListRect((_window*)searchObjPtr->layer,
			               (_listRectDesc *)searchObjPtr->dataPtr3,(_stringList *)&searchListBase);
	 }

	char msg[80];
	if (nmatch == 0)
		snprintf(msg, sizeof(msg), "Search: no match for %lX in %lu bytes @ %06lX",
		         (unsigned long)value, len, start);
	else
		snprintf(msg, sizeof(msg), "Search: %d%s hit%s for %lX  (see Search List)",
		         (nmatch > MAXHITS ? MAXHITS : nmatch), capped ? "+" : "",
		         nmatch == 1 ? "" : "s", (unsigned long)value);
	PrintWarning(msg);
}

void
GetMemSearchValue(ULONG value,void *dataPtr)
{
	globTemp = value;
    GetExpr(MemSearchGUI,dataPtr,"Enter value to search for",((_object *)dataPtr)->layer);
}

void
GetMemSearchLen(ULONG value,void *dataPtr)
{
	globTemp2 = value;
    GetExpr(GetMemSearchValue,dataPtr,"Enter # of bytes to search",((_object *)dataPtr)->layer);
}

FLAG
DoMemSearch(_menuItem *iPtr,_object *oPtr,int choice)
{
    GetExpr(GetMemSearchLen,oPtr,"Enter starting address",oPtr->layer);
	return(boolean::TRUE);
}

//=============================================================================

void
MemCompGUI(ULONG value,void *dataPtr)
{
	//SlaveCompMem(globTemp,globTemp2,value);
}

void
GetMemCompLen(ULONG value,void *dataPtr)
{
	globTemp2 = value;
    GetExpr(MemCompGUI,dataPtr,"Enter # of bytes to compare",((_object *)dataPtr)->layer);
}

void
GetMemCompDest(ULONG value,void *dataPtr)
{
	globTemp = value;
    GetExpr(GetMemCompLen,dataPtr,"Enter 2nd address to compare",((_object *)dataPtr)->layer);
}

FLAG
DoMemCompare(_menuItem *iPtr,_object *oPtr,int choice)
{
    GetExpr(GetMemCompDest,oPtr,"Enter 1st address to compare",oPtr->layer);
	return(boolean::TRUE);
}

//=============================================================================

#if 0
FLAG
OpenMemSearchWindow(void)
{
    _object *oPtr;
    _window *pWindow;
	_listRectDesc *lrPtr;
	_searchList *sPtr;

    oPtr = AddObject();
    pWindow = new _window(49,13,21,10,"Search List",(unsigned char)exprAttr,(char)exprChar);

	sPtr = (_searchList *)malloc(sizeof(_searchList));
	if(sPtr)
	 {
		oPtr->dataPtr4 = (char*)sPtr;
		InitListBase(&sPtr->list.link);
		sPtr->list.name = NULL;

		pWindow->xMin = 12;
		pWindow->xMax = displayWidth;
		pWindow->yMin = 4;					// leave enough room for up/down gadgets
		pWindow->yMax = displayHeight;

		pWindow->data = (void *)oPtr;
    	oPtr->inputRoutine = SearchInput;
		oPtr->layer = pWindow;
		oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;

		AddSysGadgets(oPtr);
		AddSearchGadgets(pWindow,oPtr);

		ActivateFrontWindow();
		DrawGadgets(oPtr);
	 }
	return(boolean::TRUE);							// in case activated from menu
}

//=============================================================================

FLAG
OpenMemCompareWindow(void)
{
	return(boolean::FALSE);
}

#endif

//=============================================================================

