//=============================================================================
//  menu.h: menu headers
//=============================================================================

#ifndef DRMON_MENU_H
#define DRMON_MENU_H

//=============================================================================

#include "object.hpp"
#include "window.hpp"

//=============================================================================

struct _menuItem;

class _menu
{
public:
	_menuItem* child;
	_menu* next;
	int xPos;
	int yPos;
	char* text;
	ubyte hotKey;
	ubyte hotKeyPos;
	unsigned int attr;

	unsigned int xSize;
	unsigned int ySize;
	int choice;
	unsigned int itemCount;

	_object *oPtr;		// spawning object
	_window *pWindow;
	FLAG oldMenuUp;

	_menu( _menuItem* firstItem, char* title="" );
	_menu()	{ }
	virtual ~_menu();

	void Render(_object *);

//private:
	_menuItem* GetMenuItem( int );
	void UnHighlightMenuOption();
	void HighlightMenuOption();
	unsigned int CountMenuItems();
	unsigned int GetWidestItem();
};


class _menuItem
	{
public:
	_menu* parent;
	_menu* child;						// Sub-menu
	_menuItem *next;
	unsigned int xPos;
	unsigned int yPos;
	char* text;
	ubyte hotKey;
	ubyte hotKeyPos;
	unsigned int attr;
	int userData;

    FLAG (*routine)(_menuItem *,_object *,int choice);		// routine to execute when menu option is selected
	void *data;									// user-assigned

//private:
	void PrintMenuText( unsigned char attr );
	void PrintMenuText()					{ PrintMenuText( attr ); }
	};

//=============================================================================

extern FLAG menuUp;

//=============================================================================

struct menuItems
{
	char *text;
	FLAG ((*routine)(_menuItem *iPtr,_object *oPtr,int choice));
	unsigned int userData;					// probably message to send to parent window
};

//=============================================================================

_menuItem*
CreateItems(menuItems* items);

void DeleteItems(_menuItem *item);

_menu* CreateMenuWithItems(menuItems* items,char* title);

FLAG MenuInput(_input *in,_object *oPtr);

//=============================================================================

void DoMenuBarMenu(_menu *mPtr,_object *oPtr);
void DoLocalMenu( _menu* mPtr, _object* oPtr, _input* in );
void DoMenu(_menu *mPtr,_object *oPtr);
void DoSubMenu(_menuItem *iPtr,_object *oPtr,menuItems* items,char* title);

FLAG
SendWindowMessage(_menuItem *iPtr,_object *oPtr,int choice);

//=============================================================================

#endif

//=============================================================================

