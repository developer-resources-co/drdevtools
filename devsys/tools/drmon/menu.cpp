//=============================================================================
//  menu.cpp: menu code
//=============================================================================
// KTS note!: none of this code handles out of memory conditions,
// however, it needs to be re-written anyway
//=============================================================================

#include <ctype.h>
#include <assert.h>

#include	"base.hpp"
#include	"global.hpp"

#include	"config.hpp"
#include	"screen.hpp"
#include	"list.hpp"
#include	"keys.hpp"
#include	"monkeys.hpp"
#include	"input.hpp"
#include	"gadget.hpp"
#include	"layer.hpp"
#include	"window.hpp"
#include	"object.hpp"

#include	"drmon.hpp"
#include	"display.hpp"
#include	"menu.hpp"

//=============================================================================

FLAG menuUp = boolean::FALSE;
static FLAG menuBarMenu = boolean::FALSE;

//=============================================================================

_menuItem*
CreateItems(menuItems* items)
{
	int yPos;
	_menuItem *item,*firstItem,*prevItem;
	yPos = 1;
	firstItem = 0;
	prevItem = 0;
	for ( int i=0; items[i].text; ++i )
	 {
		item = (_menuItem *)malloc(sizeof(_menuItem));
		if(item)
			{
	 		char* ampersandPos;

			 if(firstItem == 0)
				firstItem = item;
			if(prevItem != 0)
				prevItem->next = item;
			prevItem = item;
			item->hotKey = item->hotKeyPos = 0;
			item->text = strdup( items[i].text );
			if ( ampersandPos = strchr( item->text, '&' ) )
				{
				char hotKey;

				if ( (hotKey=*(ampersandPos+1)) != '&' )
					{
					item->hotKey = toupper( hotKey );
					item->hotKeyPos = ampersandPos - item->text;
					}

				// Move text over
				strcpy( ampersandPos, ampersandPos+1 );
				}
			item->attr = menuAttr;
			item->yPos = yPos++;
			item->xPos = 1;
			item->next = 0;
			item->routine = items[i].routine;
			item->userData = items[i].userData;
		 }
	 }
	return(firstItem);
}

//=============================================================================

void
DeleteItems(_menuItem *item)
{
	int i;
	_menuItem *nextItem;
	nextItem = item;

	while(nextItem != 0)
	 {
		nextItem = item->next;
		free(item);
	 }
}


void
_menuItem::PrintMenuText( unsigned char attr )
	{
	int _hotKey = 0;				// Used to create single character string

	PrintLayAttr( parent->pWindow, text, xPos, yPos, attr );
	if ( (hotKey) && (_hotKey = *( text + hotKeyPos) ) )
		{
		unsigned char reverseAttr = (attr>>4) | ((attr&0x0F)<<4);
		reverseAttr = attr ^ 0x08;
		PrintLayAttr( parent->pWindow, (char*)&_hotKey, (int)xPos+hotKeyPos, yPos, reverseAttr );
		}
	}

//=============================================================================
//=============================================================================
//=============================================================================

unsigned int
_menu::CountMenuItems()
	{
	_menuItem* iPtr = child;

	int i;
	for ( i=0; iPtr; ++i, iPtr=iPtr->next )
		;

	return( i );
	}

//=============================================================================

unsigned int
_menu::GetWidestItem()
	{
	unsigned int i,len;
	_menuItem *iPtr = child;
	i = 0;

	while( iPtr )
		{
		len = strlen(iPtr->text);
		if(i < len)
			i = len;

		iPtr = iPtr->next;
		}
	return(i);
	}

//=============================================================================

_menu::~_menu()
	{
	free( text );
	}


_menu::_menu( _menuItem* childParam, char* textParam )
	{
	_menuItem* iPtr;
	char* ampersandPos;

	child = childParam;
	xPos = 0;
	yPos = 0;
	text = strdup( textParam );
	hotKey = hotKeyPos = 0;
	if ( ampersandPos = strchr( text, '&' ) )
		{
		char hotKey;

		if ( (hotKey=*(ampersandPos+1)) != '&' )
			{
			hotKey = toupper( hotKey );
			hotKeyPos = ampersandPos - text;
			}

		// Move text over
		strcpy( ampersandPos, ampersandPos+1 );
		}
	next = NULL;
	choice = 0;
	attr = menuAttr;
	itemCount = CountMenuItems();
	ySize = itemCount+2;
	xSize = GetWidestItem()+2;
	if(xSize < strlen(text)+3)
		xSize = strlen(text)+3;

	for ( iPtr = child; iPtr; iPtr = iPtr->next )
		iPtr->parent = this;
	}

//=============================================================================

_menu*
CreateMenuWithItems(menuItems* items,char* text)
	{
	_menuItem *iPtr = CreateItems( items );
	return( new _menu( iPtr, text ) );
	}

//=============================================================================

_menuItem*
_menu::GetMenuItem( int choice )
{
	_menuItem* iPtr;

	iPtr = child;
	for ( int i=0; i<choice; ++i)
		iPtr = iPtr->next;

	return(iPtr);
}

//=============================================================================

// note: assumes menu currently has a layer open
void
_menu::UnHighlightMenuOption()
	{
	if ( choice >= 0 )
		GetMenuItem( choice )->PrintMenuText();
	}

//=============================================================================

// note: assumes menu currently has a layer open
void
_menu::HighlightMenuOption()
	{
	if ( choice >= 0 )
		GetMenuItem( choice )->PrintMenuText( menuHighlightAttr );
	}

//=============================================================================

FLAG
MenuInput(_input *in,_object *oPtr)
{
	_input fakeIn;
	FLAG leave = boolean::FALSE;
	_menu *mPtr;
	_menuItem *iPtr;
	mPtr = (_menu *)oPtr->dataPtr;

	switch(in->inputType)
	 {
		case INP_KEY:
			switch(in->fullKey)
	 		 {
				case CMD_MENUBAR_NEXTMENU:
				case CMD_MENUBAR_PREVMENU:
					leave = (boolean)menuBarMenu;
					break;

				case CMD_LOCALMENU_PREVITEM:
					mPtr->UnHighlightMenuOption();
					do
						{
						if ( --mPtr->choice < 0 )
							mPtr->choice = mPtr->itemCount - 1;
						iPtr = mPtr->GetMenuItem( mPtr->choice );
						}
					until ( iPtr->routine );
					mPtr->HighlightMenuOption();
					break;

				case CMD_LOCALMENU_NEXTITEM:
					mPtr->UnHighlightMenuOption();
					do
						{
						if ( ++mPtr->choice >= mPtr->itemCount )
							mPtr->choice = 0;
						iPtr = mPtr->GetMenuItem( mPtr->choice );
						}
					until ( iPtr->routine );
					mPtr->HighlightMenuOption();
					break;

				case CMD_LOCALMENU_TOP:
					mPtr->UnHighlightMenuOption();
					mPtr->choice = 0;
					mPtr->HighlightMenuOption();
					break;

				case CMD_LOCALMENU_BOTTOM:
					mPtr->UnHighlightMenuOption();
					mPtr->choice = mPtr->itemCount - 1;
					mPtr->HighlightMenuOption();
					break;

				case CMD_LOCALMENU_SELECT:
					iPtr = mPtr->GetMenuItem( mPtr->choice );
					if(iPtr->routine != 0)
						if(iPtr->routine(iPtr,((_menu *)(oPtr->dataPtr))->oPtr,iPtr->userData))
							leave = boolean::TRUE;
					break;

				case CMD_LOCALMENU_ABORT:
					leave = boolean::TRUE;
					break;

				case CMD_QUIT:
					cont = boolean::FALSE;
					break;

				default:
					{ // Look for the character in the list of items' hotkeys
					for ( int i=0; i<mPtr->itemCount; ++i )
						{
						iPtr = mPtr->GetMenuItem( i );
						if ( iPtr->hotKey == in->fullKey )
							{
							mPtr->UnHighlightMenuOption();
							mPtr->choice = i;
							mPtr->HighlightMenuOption();

							leave = ( iPtr->routine ) &&
								iPtr->routine(iPtr,((_menu *)(oPtr->dataPtr))->oPtr,mPtr->choice);
							break;
							}
						}
					}
	 		 }
			break;

		case INP_MOUSEMOVE:
			{
			FLAG found;
			int i;

			mPtr->UnHighlightMenuOption();

			for ( found=boolean::FALSE, i=0, iPtr=mPtr->GetMenuItem( 0 );
				!found && iPtr; ++i, iPtr = iPtr->next )
				{
				if( iPtr->xPos+mPtr->xPos <= in->mouseX && iPtr->xPos+mPtr->xPos + mPtr->xSize > in->mouseX &&
				   iPtr->yPos+mPtr->yPos <= in->mouseY && iPtr->yPos+mPtr->yPos + 1 > in->mouseY)
					{
					found = boolean::TRUE;
					mPtr->choice = i;
					if ( iPtr->routine )
						mPtr->HighlightMenuOption();
					}
				}
			break;
			}

		case INP_MOUSE_LEFTBUTTON_UP:
		case INP_MOUSE_RIGHTBUTTON_UP:
			{
			FLAG found;
			int i = 0;

			mPtr->UnHighlightMenuOption();

			for ( found=boolean::FALSE, i=0, iPtr=mPtr->GetMenuItem( 0 );
				!found && iPtr; ++i, iPtr = iPtr->next )
				{
				if ( iPtr->xPos+mPtr->xPos <= in->mouseX && iPtr->xPos+mPtr->xPos + mPtr->xSize > in->mouseX &&
				   iPtr->yPos+mPtr->yPos <= in->mouseY && iPtr->yPos+mPtr->yPos + 1 > in->mouseY)
					{
					found = boolean::TRUE;
					mPtr->choice = i;
					if ( iPtr->routine )
						{
						mPtr->HighlightMenuOption();
						leave = iPtr->routine(iPtr,((_menu *)(oPtr->dataPtr))->oPtr,mPtr->choice);
						}
					else
						leave = boolean::TRUE;
					}
				}

			if ( !found )
				leave = boolean::TRUE;

			break;
			}

	   case INP_MOUSE_RIGHTBUTTON_DOWN:
	   	leave = boolean::TRUE;
	   	break;
	 }

	if ( leave )
		{
		menuUp = mPtr->oldMenuUp;
		delete mPtr->pWindow;
		delete oPtr;
		if ( menuBarMenu &&
			(in->fullKey == (CMD_MENUBAR_NEXTMENU) || in->fullKey == (CMD_MENUBAR_PREVMENU) ) )
			{
			fakeIn.inputType = INP_KEY;
			fakeIn.fullKey = in->fullKey;
			frontObj->inputRoutine(&fakeIn,frontObj);
			fakeIn.fullKey = CMD_MENUBAR_SHOWMENU;
			frontObj->inputRoutine(&fakeIn,frontObj);
			}
		}

	return( boolean::TRUE );				// eat all keystroke inputs, modal
	}

//=============================================================================

void
_menu::Render(_object *pObject)
{
	// windowNum 0 => unnumbered: a dropdown isn't a hot-key-switchable window. Passed
	// to the ctor (not set after) so the digit is never painted into the title bar.
	pWindow = new _window(xPos,yPos,xSize,ySize,text,(unsigned char)menuAttr,(char)menuChar,"\xC9\xCD\xBB\xBA\xBA\xC8\xCD\xBC",0);
	pWindow->data = pObject;
	ForceWindowResize( pWindow, (_object*)pWindow->data,
		((_object*)pWindow->data)->inputRoutine );

	for ( _menuItem* iPtr = child; iPtr; iPtr = iPtr->next )
		iPtr->PrintMenuText();

	HighlightMenuOption();				// kts 06-15-93 10:42pm
}

//=============================================================================

void
DoRegularMenu(_menu *mPtr,_object *oPtr)
{
	_object *mObj;

	mObj = AddObject();
	assert(mObj);
	mPtr->Render(mObj);
	assert(mPtr);
	mPtr->oPtr = oPtr;
	mPtr->pWindow->data = mObj;
	mObj->dataPtr = (char*)mPtr;
	mPtr->oldMenuUp = menuUp;
	mObj->inputFlags = INPF_KEY|INPF_MOUSEBUTTONS|INPF_MOUSEMOVE;
	mObj->inputRoutine = MenuInput;
	mObj->layer = mPtr->pWindow;
	menuUp = boolean::TRUE;
	ActivateFrontWindow();
}

//------------------------------------------------------------------------------

void
DoMenuBarMenu(_menu *mPtr,_object *oPtr)
{
	menuBarMenu = boolean::TRUE;
	DoRegularMenu(mPtr,oPtr);
}

//------------------------------------------------------------------------------

void
DoMenu(_menu *mPtr,_object *oPtr)
{
	menuBarMenu = boolean::FALSE;
	DoRegularMenu(mPtr,oPtr);
}

//=============================================================================

void DoLocalMenu( _menu* mPtr, _object* oPtr, _input* in )
	{
	assert( mPtr ), assert( oPtr ), assert( in );
	mPtr->xPos = AutoPlace( in->mouseX - (mPtr->xSize/2), mPtr->xSize, displayWidth );
	mPtr->yPos = AutoPlace( in->mouseY, mPtr->ySize, displayHeight );
	mPtr->choice = -1;
	DoMenu( mPtr, oPtr );
	}


void
DoSubMenu(_menuItem *iPtr,_object *oPtr,menuItems *items,char *text)
{
	_menu *mPtr;
//	_layer *lPtr;
//	lPtr = oPtr->layer;

	mPtr = CreateMenuWithItems(items,text);
	mPtr->xPos = AutoPlace(iPtr->parent->xPos+2,mPtr->xSize,displayWidth);
	mPtr->yPos = AutoPlace(iPtr->parent->yPos+iPtr->parent->choice+2,mPtr->ySize,displayHeight);

	DoMenu(mPtr,oPtr);
}

//=============================================================================

FLAG
SendWindowMessage(_menuItem *iPtr,_object *oPtr,int choice)
{
	boolean result;
	_input in;

	in.inputType = INP_WINDOW_MESSAGE;
	in.message = choice;

	result = (*oPtr->inputRoutine)(&in,oPtr);


	return(result);
}

//=============================================================================

