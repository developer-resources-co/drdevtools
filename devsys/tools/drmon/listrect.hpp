//============================================================================
//	listreq.hpp: list requestor header file
//============================================================================

#ifndef _LISTRECT_HPP_
#define _LISTRECT_HPP_

#include	"window.hpp"

//============================================================================

class _stringList
{
private:
	_list link;
	char* name;			// ASCII to get printed by listreq handler

public:
	_stringList* Prev() { return((_stringList*)link.Prev()); }
	_stringList* Prev(_stringList* newPrev) { return((_stringList*)link.Prev((_list*)newPrev)); }

	_stringList* Next() { return((_stringList*)link.Next()); }
	_stringList* Next(_stringList* newNext) { return((_stringList*)link.Next((_list*)newNext)); }

	_stringList() { name = NULL; link.Next(NULL); link.Prev(NULL); }
	~_stringList() { RemoveListNode((_list *)this); }

	char *Name() {return(name); }
	char *Name(char *newName) { return( name = newName ); }
};

//============================================================================

enum
	{
	LGAD_UP,
	LGAD_DOWN,
	LGAD_SCROLL,
	LGAD_SBOX,
	LGAD_LIST,

	LGAD_COUNT
	};

class _listRectDesc
{
public:
	int xPos,yPos,xSize,ySize;
	int topItem,selItem,itemCount;
	int lineCount;							// # of lines needed to render all list items
	_stringList *listPtr;
	unsigned char fillAttr,fillChar;
	char *(*drawListEntry)
	 	(_stringList *stPtr, unsigned char *attr, unsigned* nLines);

public:
	void Render( _window* pWindow);
	_listRectDesc(_window* pWindow,int xSize,int ySize,_stringList* listBase,
				  int gadgOffset,int xPos=1,int yPos=1,int scrollXOffset=0);
	_listRectDesc() {}
};

//============================================================================

#define LRIB_INPUTUSED 0
#define LRIF_INPUTUSED 1<<LRIB_INPUTUSED
#define LRIB_NEWSEL 1
#define LRIF_NEWSEL 1<<LRIB_NEWSEL

FLAG ListRectInput(_input* in,_object* oPtr,_listRectDesc* lrPtr,int gadgOffset);

void UpdateListRect(_window* pWindow,_listRectDesc* lrPtr,int gadgOffset);

_listRectDesc* AddListRect(_window* pWindow, int xSize, int ySize, _stringList* listBase, int gadgOffset, int xPos=1, int yPos=1, int scrollOffset=0);

void SortListRect(_stringList* liPtr);

void ChangeListRect(_window* pWindow, _listRectDesc* lrPtr,_stringList* listBase);

//============================================================================

#endif

//============================================================================
