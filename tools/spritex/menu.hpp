//==============================================================================
// menu.hpp: first attempt at menus in graphics mode
//==============================================================================

#ifndef PCLIB_menu_H
#define PCLIB_menu_H

#include <string.h>

#include <pclib\general.h>
#include <pclib\list.h>
#include <pclib\grphport.h>
#include "text.hpp"

extern ubyte far systemFont[];

//==============================================================================
//==============================================================================
//==============================================================================

class menuItem : public nodeInterface<menuItem>
{
public:
	menuItem(char *newTitle) : nodeInterface<menuItem>()
	 {
	 	title = newTitle;
	 }

	const char *Title(void) { return(title); }

private:
	char *title;

	menuItem() {}
};

//==============================================================================

class menuBox
{
public:
	menuBox(char *newTitle) {}
private:
	menuBox() {}
};

//==============================================================================

class menuBarItem : public nodeInterface<menuBarItem>
{
public:
	menuBarItem(char *newTitle, gfxText *newMenuTextPtr) : nodeInterface<menuBarItem>(), itemList(), pos()
	 {
	 	title = newTitle;
		menuTextPtr  = newMenuTextPtr;
	 }

	const char *Title(void) { return(title); }

	menuItem *AddItem(char *title)
	 {
		menuItem *itemPtr;
		itemPtr = new menuItem(title);
		itemList.Append(itemPtr);
		return(itemPtr);
	 }

	const point &Where(void) { return(pos); }
	const point &Move(point newPos) { pos = newPos; return(pos); }

	int Width(void) { return(strlen(title)* menuTextPtr->Width()); }
	int Height(void) { return(menuTextPtr->Height()); }

private:
	char *title;
	point pos;
	list<menuItem> itemList;
	gfxText *menuTextPtr;
	menuBarItem() {}
};

//==============================================================================
// the menu bar class draws a bar across the top of the given rastport

class menuBar
{
public:
	menuBar(rastGraphPort *grp) : itemList()
	 {
	 	grPort = grp;

		menuTextPtr = new gfxText(grPort, systemFont, 64, 8,8);

		menuColor = 0xef;
		xorVal = 0x1f;

		menuBarItem *i1 = AddItem("MENU1");
		i1->AddItem("MENUITEM1");
		i1->AddItem("MENUITEM2");
		i1->AddItem("MENUITEM3");
		i1->AddItem("MENUITEM4");
		AddItem("MENU2");

		currentItem = 0;

		ReDraw();
		HighlightItem();
	 }

	void
	ReDraw(void)
	 {
	 	grPort->SetColorIndex(menuColor);
		grPort->Box(point(0,0),point(grPort->GetWidth()-1,menuTextPtr->Width()-1));

		menuBarItem *mbiPtr = itemList.GetNext();
		while(mbiPtr)
		 {
			mbiPtr->Move(menuTextPtr->Where());
			menuTextPtr->PrintLine(mbiPtr->Title());
			menuTextPtr->PrintChar(' ');
			mbiPtr = mbiPtr->GetNext();
		 }
	 }

	menuBarItem *AddItem(char *title)
	 {
		menuBarItem *itemPtr;
		itemPtr = new menuBarItem(title,menuTextPtr);
		itemList.Append(itemPtr);
		return(itemPtr);
	 }

private:
	void HighlightItem(void)
	 {
		if(itemList.Count() > 0)
		 {
			menuBarItem *mbiPtr = itemList.GetItem(currentItem);
			clippedGraphPort cgpPtr(grPort,mbiPtr->Where(),mbiPtr->Width()-1,mbiPtr->Height()-1);
			cgpPtr.Xor(xorVal);
		 }
	 }

	colorMapIndex menuColor;
	ubyte xorVal;
	rastGraphPort *grPort;
	gfxText *menuTextPtr;
	list<menuBarItem> itemList;
	uint currentItem;
};

//------------------------------------------------------------------------------

//==============================================================================
#endif
//==============================================================================

