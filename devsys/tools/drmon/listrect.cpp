//===========================================================================
//	listrect.cpp: list rectangle handler, for many things, including files
//===========================================================================

#include <stdlib.h>
#include <assert.h>

#include	"base.hpp"
#include	"global.hpp"

#include	"general.hpp"
#include	"config.hpp"
#include	"screen.hpp"
#include	"list.hpp"
#include	"keys.hpp"
#include	"monkeys.hpp"
#include	"input.hpp"
#include	"gadget.hpp"
#include	"object.hpp"
#include	"listrect.hpp"
#include	"filereq.hpp"
#include	"layer.hpp"
#include	"window.hpp"
#include	"message.hpp"
#include	"mouse.hpp"
#include	"error.hpp"

//===========================================================================

char*
PrintListDefault( _stringList* stPtr, unsigned char* attr, unsigned* nLines )
	{
	*attr = ATTR_NORMAL;
	*nLines = 1;
	return( stPtr->Name() );
	}

//===========================================================================

void
_listRectDesc::Render( _window* pWindow )
	{
	_stringList* reqPtr;
	int y,count,selected;
	char *buffer;
	unsigned char attr;
	unsigned nLines;

	selected = selItem;
	count = ySize;

	assert( pWindow );

	FillRectLayer( pWindow,
		xPos, yPos, xSize, ySize,
		fillChar, fillAttr );
	reqPtr = (_stringList*)listPtr;
	reqPtr = (_stringList*)reqPtr->Next();						// skip header

	lineCount = 0;

	for ( y=0; y<topItem; reqPtr = (_stringList*)reqPtr->Next() )
		{
		if ( buffer = (*drawListEntry)(reqPtr,&attr,&nLines) )
			{
			y += nLines;
			lineCount += nLines;
			}
		}

	selected -= topItem;
	y = yPos;
	while ( reqPtr && count )
		{
		if ( buffer = (*drawListEntry)(reqPtr,&attr,&nLines) )
			{
			if ( selected-- )
				PrintLayAttrClip( (_layer*)pWindow, (char*)buffer, (int)xPos, (int)y, (unsigned char)attr, (int)xSize+xPos );
			else
				PrintLayAttrClip( (_layer*)pWindow, (char*)buffer, (int)xPos, (int)y, (unsigned char)ATTR_INVERSE, (int)xSize+xPos );
			y += nLines;
			count--;
			lineCount += nLines;
			}
		reqPtr = (_stringList *)reqPtr->Next();
		}

	for ( ; reqPtr; reqPtr = (_stringList*)reqPtr->Next() )
		{
		if ( buffer = (*drawListEntry)( reqPtr, &attr, &nLines ) )
			lineCount += nLines;
		}
	}

//===========================================================================

FLAG
ListRectInput(_input *in,_object *oPtr,_listRectDesc *lrPtr,int gadgOffset)
	{
	_gadget* gPtr;
	_window *pWindow;
	FLAG inputUsed = boolean::TRUE;			// eat most keystrokes
	FLAG newSel = boolean::FALSE;

	assert(oPtr);
	assert(lrPtr);

	pWindow = (_window *)oPtr->layer;
	assert(pWindow);

#if 0
	_gadget* propGadg = FindGadget( &pWindow->gadgBase, LGAD_SBOX+gadgOffset );
	assert( propGadg );
	int yPropSize = propGadg->ySize;
#else
	int yPropSize = 1;
#endif

	switch(in->inputType)
		{
		case INP_KEY:
			switch(in->fullKey)
	 			{
				case CMD_LIST_UP:
					if(lrPtr->selItem > 0)
						{
						lrPtr->selItem--;
						}
					break;

				case CMD_LIST_DOWN:
					if(lrPtr->selItem < lrPtr->lineCount-yPropSize)
						lrPtr->selItem++;
					break;

				case CMD_LIST_TOP:
					lrPtr->selItem = 0;
					break;

				case CMD_LIST_BOTTOM:
					lrPtr->selItem = lrPtr->lineCount-yPropSize;
					break;

				case CMD_LIST_UPPAGE:
					if((lrPtr->selItem -= lrPtr->ySize) < 0)
						lrPtr->selItem = 0;
					break;

				case CMD_LIST_DOWNPAGE:
					if((lrPtr->selItem += lrPtr->ySize) > lrPtr->lineCount-lrPtr->ySize)
						lrPtr->selItem = lrPtr->lineCount-yPropSize;
					break;

				default:
					inputUsed = boolean::FALSE;
					break;
				}
			break;

		case INP_MOUSEMOVE:
			{
			int itemCount,oldTopItem = lrPtr->topItem;
			if(lrPtr->itemCount >= lrPtr->ySize)
				itemCount = lrPtr->itemCount-lrPtr->ySize;
			else
				itemCount = 0;

			if (pGadgDown && pGadgDown->gNum == gadgOffset+LGAD_SBOX)
			 {
				lrPtr->topItem = GetNormalizedPropGadgPos(pWindow,LGAD_SBOX+gadgOffset,in->mouseY - oPtr->layer->yPos,itemCount);
				lrPtr->selItem += lrPtr->topItem-oldTopItem;
			 }
			else
				inputUsed = boolean::FALSE;
			break;
			}

		case INP_MOUSE_LEFTBUTTON_UP:
			if(!PropGadgUp(oPtr,in))
				inputUsed = boolean::FALSE;
			break;

		case INP_MOUSE_LEFTBUTTON_DOWN:
			if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
			 {
				switch(gPtr->gNum-gadgOffset)
			  	 {
					case  LGAD_UP:
						if(lrPtr->topItem > 0)
						 {
							lrPtr->topItem--;
							lrPtr->selItem--;
						 }
						break;
					case  LGAD_DOWN:
					if(lrPtr->topItem+lrPtr->ySize < lrPtr->itemCount-1)
					 {
						lrPtr->topItem++;
						lrPtr->selItem++;
					 }
						break;
					case  LGAD_SBOX:
						PropGadgDown(oPtr,in,gPtr);
						break;
					case LGAD_LIST:
					 {
						int yPos;
						yPos = in->mouseY - (pWindow->yPos+gPtr->yPos);
						lrPtr->selItem = lrPtr->topItem+yPos;
						if(lrPtr->selItem >= lrPtr->itemCount)
							lrPtr->selItem = lrPtr->itemCount-1;
						newSel = boolean::TRUE;
					 }
						break;
					default:
						inputUsed = boolean::FALSE;
						break;
				 }
			 }
			break;

		default:
			inputUsed = boolean::FALSE;
			break;
		}
	return(inputUsed<<LRIB_INPUTUSED | newSel<<LRIB_NEWSEL);
	}

//===========================================================================

void
UpdateListRect(_window* pWindow,_listRectDesc *lrPtr,int gadgOffset)
	{
	assert( pWindow );
	assert( lrPtr );

	int temp;

	if ( lrPtr->selItem < 0 )
		lrPtr->selItem = 0;

	if ( lrPtr->selItem < lrPtr->topItem )
		lrPtr->topItem = lrPtr->selItem;

	if ( lrPtr->selItem >= lrPtr->topItem + lrPtr->ySize )
		lrPtr->topItem = lrPtr->selItem - ( lrPtr->ySize - 1 );

	int nPos;						// number of positions prop gadget can be set to
	if(lrPtr->itemCount >= lrPtr->ySize)
		nPos = lrPtr->itemCount-lrPtr->ySize;
	else
		nPos = 0;

	lrPtr->Render( pWindow );
	temp = nPos ? 65536/nPos : 0;
	UpdateVScrollGadg( pWindow, LGAD_SBOX+gadgOffset, lrPtr->topItem*temp,
		nPos < lrPtr->ySize ? ~0 : lrPtr->ySize * temp );
	}

//===========================================================================

static void
AddListRectGadgets(_window *pWindow,_listRectDesc *lrPtr,int gadgOffset,int scrollOffset)
{
	_gadget  *gPtr;

	AddVScrollGadgetsCoord(pWindow,LGAD_UP+gadgOffset,LGAD_DOWN+gadgOffset,LGAD_SCROLL+gadgOffset,LGAD_SBOX+gadgOffset,lrPtr->xPos+lrPtr->xSize+scrollOffset,lrPtr->yPos,(lrPtr->ySize+lrPtr->yPos)-1);

							// list gadget(so that user can click on filenames)
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = lrPtr->xSize;
	gPtr->ySize = lrPtr->ySize;
	gPtr->xPos = lrPtr->xPos;
	gPtr->yPos = lrPtr->yPos;
	gPtr->gadgText = NULL;
	gPtr->gNum = LGAD_LIST+gadgOffset;
	gPtr->gadgAttr = windowBorderAttr;
}

//===========================================================================

_listRectDesc*
AddListRect(_window* pWindow, int xSize, int ySize, _stringList* listBase, int gadgOffset, int xPos, int yPos, int scrollXOffset)
	{
	_layer* lPtr;
	_listRectDesc* lrPtr;

//	if ( lrPtr = (_listRectDesc*)malloc(sizeof(_listRectDesc)) )
	if ( lrPtr = new _listRectDesc )
		{
		lrPtr->drawListEntry = PrintListDefault;
		lrPtr->topItem = 0;
		lrPtr->listPtr = listBase;
		lrPtr->selItem = 0;								// start on item 0
		lrPtr->xPos = xPos;
		lrPtr->yPos = yPos;
		lrPtr->ySize = ySize;
		lrPtr->xSize = xSize;
		lrPtr->fillAttr = listAttr;
		lrPtr->fillChar = listChar;
		lrPtr->itemCount = CountListItems((_list*)listBase);
		lrPtr->Render( pWindow );
		AddListRectGadgets(pWindow,lrPtr,gadgOffset,scrollXOffset);
		}
	return( lrPtr );
	}


_listRectDesc::_listRectDesc(_window* pWindow, int xSize, int ySize, _stringList* listBase, int gadgOffset, int xPos, int yPos, int scrollXOffset)
{
	_layer* lPtr;
	_listRectDesc* lrPtr;

//	if ( lrPtr = (_listRectDesc*)malloc(sizeof(_listRectDesc)) )
	if ( lrPtr = new _listRectDesc )
	 {
		lrPtr->drawListEntry = PrintListDefault;
		lrPtr->topItem = 0;
		lrPtr->listPtr = listBase;
		lrPtr->selItem = 0;								// start on item 0
		lrPtr->xPos = xPos;
		lrPtr->yPos = yPos;
		lrPtr->ySize = ySize;
		lrPtr->xSize = xSize;
		lrPtr->fillAttr = listAttr;
		lrPtr->fillChar = listChar;
		lrPtr->itemCount = CountListItems((_list*)listBase);
		lrPtr->Render( pWindow );
		AddListRectGadgets(pWindow,lrPtr,gadgOffset,scrollXOffset);
	 }
}

//===========================================================================

void
ChangeListRect(_window* pWindow,_listRectDesc *lrPtr,_stringList *listBase)
	{
	assert( pWindow );

	if ( lrPtr->listPtr = listBase )
		{
		lrPtr->itemCount = CountListItems((_list *)lrPtr->listPtr);
		if(lrPtr->selItem >= lrPtr->itemCount)
			lrPtr->selItem = lrPtr->itemCount-1;
		if(lrPtr->selItem < 0)
			lrPtr->selItem = 0;
		if(lrPtr->topItem + lrPtr->ySize >= lrPtr->itemCount)
			lrPtr->topItem = lrPtr->itemCount - lrPtr->ySize;
		if(lrPtr->topItem < 0)
			lrPtr->topItem = 0;
		lrPtr->Render( pWindow );
		}
	}

//===========================================================================
// Sorts one list into another list

void
SortListRect( _stringList* liPtr )
	{
	FLAG cnt;
	int i,sc;
	_stringList *list,*list2;

	if(!(_stringList *)liPtr->Next())			// skip header
		return;							// nothing to sort

	cnt = boolean::TRUE;
	while(cnt)
		{
		list = (_stringList *)liPtr->Next();			// skip header
		cnt = boolean::FALSE;
		while((list2 = (_stringList *)list->Next()) != 0)
			{
			sc = strcmp(list->Name(),list2->Name());
			if(sc > 0)
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

//===========================================================================
