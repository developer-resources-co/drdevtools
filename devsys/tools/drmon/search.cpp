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

//=============================================================================

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

//=============================================================================

void
MemSearchGUI(ULONG value,void *dataPtr)
{
	//SlaveCompMem(globTemp,globTemp2,value);
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

