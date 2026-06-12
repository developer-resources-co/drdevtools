//===========================================================================
//  menubar.cpp: menubar simulating pull-down type menus
//===========================================================================

#include <assert.h>
#include <time.h>

#include "base.hpp"
#include "global.hpp"

#include "config.hpp"
#include "screen.hpp"		// screenWidth (right-align the menu-bar clock)
#include "list.hpp"
#include "keys.hpp"
#include "monkeys.hpp"
#include "input.hpp"
#include "gadget.hpp"
#include "object.hpp"
#include "layer.hpp"
#include "menu.hpp"
#include "drmon.hpp"
#include "menubar.hpp"

//===========================================================================

_menu *
GetMenu(_menu *mPtr,int choice)
	{
	for ( int i=0; i<choice; ++i )
		mPtr = mPtr->next;
	return mPtr;
	}

//===========================================================================

void
UnHighlightMenu(_menu *mPtr,int currentMenu,_layer *lPtr)
{
	mPtr = GetMenu(mPtr,currentMenu);
    PrintLay(lPtr,mPtr->text,mPtr->xPos,mPtr->yPos);
}

//===========================================================================

void
HighlightMenu(_menu *mPtr,int currentMenu,_layer *lPtr)
{
	mPtr = GetMenu(mPtr,currentMenu);
    PrintLayAttr(lPtr,mPtr->text,mPtr->xPos,mPtr->yPos,menuBarHighlightAttr);
}

//===========================================================================

void
MenuBarActivate(_object *oPtr)
{
	_menu *mPtr;
	mPtr = (_menu *)oPtr->dataPtr;
//	oPtr->data1 = 0;								// force to restart at first menu
	HighlightMenu(mPtr,oPtr->data1,oPtr->layer);
}

//===========================================================================

void
MenuBarDeactivate(_object *oPtr)
{
	_menu *mPtr;
	mPtr = (_menu *)oPtr->dataPtr;
	UnHighlightMenu(mPtr,oPtr->data1,oPtr->layer);
}

//===========================================================================

void
RenderMenuTitles(_layer *lPtr,_menu *mPtr,int currentMenu)
{
	_menu *mPtr2;
	mPtr2 = mPtr;
	while(mPtr != 0)
	 {
    	PrintLay(lPtr,mPtr->text,mPtr->xPos,mPtr->yPos);
		mPtr = mPtr->next;
	 }
	if(frontObj->layer == lPtr)
		HighlightMenu(mPtr2,currentMenu,lPtr);
}

//===========================================================================

void
MenuRoutine(_object *oPtr)
{
	tm *tPtr;
	time_t lt;
	char *cPtr;

	lt = time(NULL);
	tPtr=localtime(&lt);
	cPtr = asctime(tPtr);
	strcpy(textBuffer,cPtr);
	{ char *nl = strchr(textBuffer,'\n'); if(nl) *nl = 0; }   // asctime trailing newline
	// Right-align to the menu bar's width (lands at col 55 on an 80-wide screen, as
	// before; moves to the top-right when the screen fills a wider terminal).
	{ int x = (int)screenWidth - (int)strlen(textBuffer) - 1; if(x < 0) x = 0;
	  PrintLay(oPtr->layer,textBuffer,x,0); }
}

//===========================================================================

int
CountMenus(_menu *mPtr)
{
	int count = 0;
	while(mPtr)
	 {
		mPtr = mPtr->next;
		count++;
	 }
	return(count);
}

//===========================================================================

_menu *
FindLastMenu(_menu *mPtr)
{
	while(mPtr->next != 0)
		mPtr = mPtr->next;
	return(mPtr);
}

//===========================================================================

void
AddMenu(_menu *mPtr,_menu *m2Ptr)
{
	mPtr = FindLastMenu(mPtr);
	mPtr->next = m2Ptr;
	m2Ptr->yPos = mPtr->yPos;		// assumes it is a horizontal menu
	m2Ptr->xPos = mPtr->xPos+(strlen(mPtr->text)+1);
}

//===========================================================================

void
MMenuBarMenu(_menu *mPtr,_object *oPtr)
	{
	assert( mPtr ), assert( oPtr );

	_menu* mPtr1 = GetMenu(mPtr,oPtr->data1);
	assert( mPtr1 );

	mPtr1->choice = 0;							// always start at top of menu
	DoMenuBarMenu(mPtr1,oPtr);
	}

//---------------------------------------------------------------------------

FLAG
MenuBarKeys(_input *in,_object *oPtr)
{
    _layer *lPtr;
    FLAG keyUsed,moved;
    unsigned char *chr;
	_menu *mPtr,*mPtr1;
	int i,oldActiveMenu;

	oldActiveMenu = oPtr->data1;
	moved = boolean::FALSE;

	mPtr = (_menu *)oPtr->dataPtr;
    lPtr = oPtr->layer;
	keyUsed = boolean::TRUE;

	switch(in->inputType)
	 {
		case INP_KEY:
			if(in->fullKey >= 'A' && in->fullKey <= 'Z')
	 		 {
				mPtr1 = mPtr;
				i = 0;
				while(mPtr1 && i != -1)
		 		 {
					if(mPtr1->hotKey == in->fullKey)
			 		 {
						oPtr->data1 = i;
						in->fullKey = CMD_MENUBAR_SHOWMENU;
						i = -2;							// leave loop
			 		 }
					mPtr1 = mPtr1->next;
					i++;
		 		 }
	 	     }

			switch(in->fullKey)
	 		 {
				case CMD_MOVEWIN:				// eat window move commands
					break;

				case CMD_MENUBAR_NEXTMENU:
					if(oPtr->data1 < CountMenus(mPtr)-1)
			 	 	 {
						oPtr->data1++;
						moved = boolean::TRUE;
			 	 	 }
					break;

				case CMD_MENUBAR_PREVMENU:
					if(oPtr->data1 > 0)
			 	 	 {
						oPtr->data1--;
						moved = boolean::TRUE;
			 	 	 }
					break;

				case CMD_MENUBAR_SHOWMENU:
					UnHighlightMenu(mPtr,oldActiveMenu,lPtr);
					MMenuBarMenu(mPtr,oPtr);
					break;

				default:
					keyUsed = boolean::FALSE;
					break;
	     	 }
			break;

		case INP_MOUSE_LEFTBUTTON_DOWN:
//		case INP_MOUSE_RIGHTBUTTON_DOWN:
			mPtr1 = GetMenu(mPtr,0);
			i = 0;
			cont = boolean::TRUE;
			while(mPtr1)
			 {
			  	if(in->mouseX >= mPtr1->xPos & in->mouseX < mPtr1->xPos+strlen(mPtr1->text))
				 {
				  	oPtr->data1 = i;
					UnHighlightMenu(mPtr,oldActiveMenu,lPtr);
					MMenuBarMenu(mPtr,oPtr);

					_input fakeIn;
					fakeIn.inputType = INP_MOUSEMOVE;
					frontObj->inputRoutine(&fakeIn,frontObj);
					break;
				 }
				mPtr1 = mPtr1->next;
				i++;
			 }
			break;

		default:
			keyUsed = boolean::FALSE;
			break;
	 }

	if(moved)
	 {
		UnHighlightMenu(mPtr,oldActiveMenu,lPtr);
		HighlightMenu(mPtr,oPtr->data1,lPtr);
	 }
    return(keyUsed);
}

//===========================================================================
