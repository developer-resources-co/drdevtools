//=============================================================================
// textview.hpp
//=============================================================================

#include "list.hpp"
#include "mem.hpp"
#include "object.hpp"

//=============================================================================

class _viewText
{
public:
	_list link;
	memory *textMemory;
	unsigned char charAttr;
	void DeleteLines();
};

//=============================================================================

class _textViewWindow : _object
{
public:
	/*
	_textViewWindow(void) : _object(TextLineRoutine)
	 {
	 }

	viewTextBase;
	char *fileName;
	int topLine;   				// top line visible in window
	int leftColumn;				// left column visible in window
	int lineCount;
	int pc;
	*/
};

//=============================================================================

void
AddTextViewGadgets(_window *pWindow);

void
TextViewRoutine(_object *oPtr);

errorcode
TextViewLoad(_object *oPtr, const char *fileName,const char *path=NULL);

FLAG
DoTextViewSearchLabel(_menuItem *iPtr,_object *oPtr,int choice);

void
TextViewGotoLine(_object *oPtr,ULONG newLine);

_object *
OpenNamedTextFile(char *name,char *title, unsigned int xSize, unsigned int ySize);

void
FVNext(_object *oPtr);

void
FVSearch(char *string,void *dataPtr);

void
FVSearchLabel(char *string,void *dataPtr);

FLAG
DoTextViewNext(_menuItem *iPtr,_object *oPtr,int choice);

FLAG
DoTextViewSearch(_menuItem *iPtr,_object *oPtr,int choice);

FLAG
DoTextViewGoto(_menuItem *iPtr,_object *oPtr,int choice);

errorcode
TextViewFindLabel(_object *oPtr,char *str);

//============================================================================
// textview commands
enum
{
	TEXTVIEW_LOAD = 1,
	TEXTVIEW_SEARCH,
	TEXTVIEW_SEARCHLABEL,
	TEXTVIEW_NEXT,
	TEXTVIEW_GOTO
};

//=============================================================================
