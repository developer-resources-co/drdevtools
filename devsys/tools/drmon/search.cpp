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

// Declared in slaveio.hpp, which search.cpp does not include (it uses explicit
// headers, and slaveio.hpp is unguarded).  slaveio.hpp wraps the slave calls in
// extern "C", so match that linkage or the reference goes unresolved at link time.
extern "C" void ReadSlaveData(unsigned long addr, char far *data, unsigned int len);

//=============================================================================

// ---------------------------------------------------------------------------
// Scrollable search-results window — scaffold from the DOS original, never
// ported/compiled on Linux.  Disabled: its local-menu calls use the old parallel
// char*[]/routine-array form, but CreateMenuWithItems now takes a menuItems[]
// table (API drifted during the port).  The first-cut search (MemSearchGUI below)
// reports hits on the message bar and needs none of this.  Reviving the results
// window is future work — see TODO (DRMON — UI/UX).
// ---------------------------------------------------------------------------
#if 0
enum
	{
	SGAD_UP = GAD_USER+1,
	SGAD_DOWN,
	SGAD_SCROLL,
	SGAD_SBOX,
	SGAD_LIST
	};

//=============================================================================

FLAG
MenuSearchSearch(_menuItem *iPtr,_object *oPtr,int choice)
{
	return(boolean::TRUE);
}

FLAG
MenuSearchList(_menuItem *iPtr,_object *oPtr,int choice)
{
	return(boolean::TRUE);
}

FLAG
MenuSearchClear(_menuItem *iPtr,_object *oPtr,int choice)
{
	return(boolean::TRUE);
}

//=============================================================================

char *searchMenuItems[] =
{
	"&Search...      ",
	"Search &List... ",
	"&Clear List     ",
	0
};

FLAG ((*searchRoutines[])(_menuItem *iPtr,_object *oPtr,int choice)) =
{
	MenuSearchSearch,
	MenuSearchList,
	MenuSearchClear,
};

//=============================================================================

char *
SearchPrintListEntry(_stringList *stPtr,unsigned char *attr, unsigned* nLines )
{
	_searchList *sPtr;
	char*string;
	sPtr = (_searchList *)stPtr;

	*attr = ATTR_NORMAL;

	string = textBuffer;
	*PrintRaw32Bits(string,sPtr->addr)= 0;

	*nLines = 1;

	return(textBuffer);
}

//=============================================================================

void
AddSearchGadgets(_window* pWindow,_object* oPtr)
{
	assert( pWindow );

	_gadget *gPtr;
	_listRectDesc *lrPtr;

	lrPtr = AddListRect(pWindow,pWindow->xSize-2,pWindow->ySize-2,(_stringList *)oPtr->dataPtr4,SGAD_LIST);
	lrPtr->drawListEntry = SearchPrintListEntry;
	oPtr->dataPtr3 = (char*)lrPtr;
	UpdateListRect(pWindow,lrPtr,SGAD_LIST);
}

//=============================================================================

FLAG
SearchInput(_input *in,_object *oPtr)
{
    _window *pWindow;
	_menu *mPtr;

    FLAG inputUsed;
	FLAG listIn;
    unsigned char far *chr;
	_gadget *gPtr;

    pWindow = (_window *)oPtr->layer;
    inputUsed = boolean::TRUE;

	listIn = ListRectInput(in,oPtr,(_listRectDesc *)oPtr->dataPtr3,SGAD_LIST);
	if((listIn & LRIF_INPUTUSED) == boolean::FALSE)
		switch(in->inputType)
	 	{
			case INP_KEY:
				switch(in->fullKey)
	 	 	 	{
					case CMD_LOCALMENU:
						mPtr = CreateMenuWithItems(searchMenuItems,searchRoutines,"Search");
						mPtr->xPos = AutoPosition(pWindow->xPos,pWindow->xSize,mPtr->xSize,displayWidth);
						mPtr->yPos = AutoPosition(pWindow->yPos,pWindow->ySize,mPtr->ySize,displayHeight);
						DoMenu(mPtr,oPtr);
						break;
					case CMD_CLOSEWINDOW:
//						CloseWindow(pWindow);
						delete pWindow;
						delete (_listRectDesc *)oPtr->dataPtr3;
//						DeleteObject(oPtr);
						delete oPtr;
						oPtr = NULL;
						inputUsed = boolean::TRUE;
						break;
					case CMD_SEARCHLIST:
						break;
					default:
						inputUsed = boolean::FALSE;
	 	 	 	}
				break;
			case INP_MOUSEMOVE:
				break;
			case INP_MOUSE_LEFTBUTTON_UP:
				break;

			case INP_MOUSE_LEFTBUTTON_DOWN:
				if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
			 	{
					switch(gPtr->gNum)
				 	{
						case GGAD_CLOSE:
//							CloseWindow(pWindow);
							delete pWindow;
							delete (_listRectDesc *)oPtr->dataPtr3;
							DeleteObject(oPtr);
							oPtr = NULL;
							inputUsed = boolean::TRUE;
							break;
						default:
							inputUsed = boolean::FALSE;
							break;
				 	}
			 	}
				break;
			case INP_MOUSE_RIGHTBUTTON_DOWN:
				mPtr = CreateMenuWithItems(searchMenuItems,searchRoutines,"Search");
//				mPtr->xPos = AutoPosition(pWindow->xPos,pWindow->xSize,mPtr->xSize,displayWidth);
//				mPtr->yPos = AutoPosition(pWindow->yPos,pWindow->ySize,mPtr->ySize,displayHeight);
				DoLocalMenu(mPtr,oPtr,in);
				break;

			case INP_RESIZE:
				delete (_listRectDesc *)oPtr->dataPtr3;
				AddSearchGadgets(pWindow,oPtr);
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
#endif  // results-window scaffold (future work)

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
	// wider range value if you need the explicit width.)  Hits are reported on the
	// message bar (first cut; a scrollable results window is future work — TODO).
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

	const unsigned int CHUNK = 4096;
	unsigned char buf[CHUNK];
	unsigned long matches[64];
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
				if (nmatch < 64) matches[nmatch++] = addr + i;
				else             capped = 1;
			 }
		 }
		if (want < CHUNK) break;                       // final (short) window done
		addr += CHUNK - (unsigned long)(w - 1);        // overlap w-1 for span hits
	 }

	char msg[256];
	if (nmatch == 0)
	 {
		snprintf(msg, sizeof(msg),
		         "Search: no match for %lX in %lu bytes @ %06lX",
		         (unsigned long)value, len, start);
	 }
	else
	 {
		int shown = nmatch < 8 ? nmatch : 8;
		int pos = snprintf(msg, sizeof(msg), "Search: %d%s hit%s @",
		                   nmatch, capped ? "+" : "", nmatch == 1 ? "" : "s");
		for (int i = 0; i < shown && pos < (int)sizeof(msg) - 8; i++)
			pos += snprintf(msg + pos, sizeof(msg) - pos, " %06lX", matches[i]);
		if (nmatch > shown && pos < (int)sizeof(msg) - 5)
			snprintf(msg + pos, sizeof(msg) - pos, " ...");
	 }
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

