//=============================================================================
//	window.cpp: windowing functions using layers
//=============================================================================

#include <assert.h>

#include	"base.hpp"
#include	"global.hpp"
#include	"list.hpp"
#include	"gadget.hpp"
#include	"input.hpp"
#include	"object.hpp"
#include	"drmon.hpp"
#include	"config.hpp"
#include	"screen.hpp"
#include	"layer.hpp"
#include	"window.hpp"
#include	"message.hpp"
#include	"error.hpp"
#include	"display.hpp"
#include	"command.hpp"

//=============================================================================
// extended layer open code

int
GetAvailWindowNum(void)
{
	_layer *lPtr;
	FLAG arr[12];
	int i;
	for(i =0;i<12;i++)
		arr[i] = boolean::FALSE;

    lPtr = layBase.Head();
    while(lPtr)
	 {
		if(lPtr->windowNum)
			arr[lPtr->windowNum] = boolean::TRUE;
		lPtr = lPtr->next;
	 }

	for(i=1;i<11;i++)
	 {
		if(arr[i] == boolean::FALSE)
			return(i);
	 }

	return(0);					// if none available, print none
}

//==============================================================================

_layer *
FindNumberedWindow(int num)
{
	_layer *lPtr;

    lPtr = layBase.Head();
    while(lPtr)
	 {
		if(lPtr->windowNum == num)
			return(lPtr);
		lPtr = lPtr->next;
	 }
	return(NULL);					// not found
}

//==============================================================================

_window::_window( int newXPos,int newYPos,unsigned int xSize,unsigned int ySize, char *newTitle, unsigned char charAttr, char fillChar, char newBorder[8], int newWindowNum) :
	_shadowedLayer(newXPos, newYPos, xSize, ySize, charAttr, fillChar)
{
	if ( newXPos == -1 )
		xPos = virtualXOffset + AutoCenter( xSize, displayWidth );
	if ( newYPos == -1 )
		yPos = virtualYOffset + AutoCenter( ySize, displayHeight );

	CopyBorder(border,newBorder);
	title = NULL;
	data = NULL;
	if(newTitle)
		title = strdup( newTitle );
#if 1
#else
	 {
		int charCount;
		charCount = strlen(newTitle);
		char *tPtr;
		tPtr = (char *)malloc(charCount+1);
		if(tPtr)
		 {
			strcpy(tPtr,newTitle);
	    	title = tPtr;
		 }
	 }
#endif
	DrawBorder(this,border,xSize,ySize);
	// -1 => auto-assign; otherwise honour the caller (0 = unnumbered menu). Must be
	// set before PrintTitle, which paints the number into the title bar right here —
	// a later override (e.g. menu.cpp) is too late to stop the digit being drawn.
	windowNum = (newWindowNum < 0) ? GetAvailWindowNum() : newWindowNum;
	PrintTitle(this,windowTitleAttr);			// won't work since oPtr is not yet valid
	activateRoutine = HighLightTitle;
	deactivateRoutine = UnHighLightTitle;

	activeGadget = NULL;
	InitGadgetList( &gadgBase );
}

//==============================================================================

_window::~_window()
{
	_layer *lPtr2;

	ToBack();

	DeleteAllGadgets( &gadgBase );

	while( (layBase.Tail())->data == 0)  			// ensure input window is found
		 (layBase.Tail())->ToBack();

	lPtr2 =  (layBase.Tail());
	assert( lPtr2 );
	frontObj = (_object *)lPtr2->data;
	assert( frontObj );

	if(frontObj->layer->activateRoutine != 0)
		frontObj->layer->activateRoutine(frontObj);
	UpdateCursor();
}

//==============================================================================

void
ZoomWindow(_object *oPtr)
{
	_window *pWindow;
	pWindow = (_window*)oPtr->layer;

	if(pWindow->xSize != pWindow->xMax || pWindow->ySize != pWindow->yMax)
	 {
		pWindow->oldXPos = pWindow->xPos;
		pWindow->oldYPos = pWindow->yPos;
		pWindow->oldXSize = pWindow->xSize;
		pWindow->oldYSize = pWindow->ySize;

		pWindow->xSize = pWindow->xMax;
		pWindow->ySize = pWindow->yMax;
		if(pWindow->xPos + pWindow->xSize > displayWidth)
			pWindow->xPos = displayWidth - pWindow->xSize;
		if(pWindow->yPos + pWindow->ySize > displayHeight)
	  		pWindow->yPos = displayHeight - pWindow->ySize;
	 }
	else
	 {
		pWindow->xPos = pWindow->oldXPos;
		pWindow->yPos = pWindow->oldYPos;
		pWindow->xSize = pWindow->oldXSize;
		pWindow->ySize = pWindow->oldYSize;
	 }
	ForceWindowResize(pWindow,oPtr,oPtr->inputRoutine);
	PrintTitle(pWindow,windowTitleHighlightAttr);
}

//=============================================================================

_object *
FindWindow( char* wName )
{
//	printf( "FindWindow(%s)\n", wName );

	_object *oPtr = pObjBase->Next();

	while ( oPtr )
		{
		if ( oPtr->layer && oPtr->layer->title )
			if (!strcmp(oPtr->layer->title,wName))
				return oPtr;
		oPtr = oPtr->Next();
		}
	return NULL;
	}


void
CopyBorder(char destB[8],char srcB[8])
{
    int i;
    for(i=0;i<8;++i)
	destB[i] = srcB[i];
}

//=============================================================================
// border order ul,um,ur,l,r,dl,dm,dr	(u=up,l=left,m=middle,r=right,d=down)
//  example: '╔═╗║║╚═╝'

void
DrawBorder(_layer *lPtr,char border[8],int xSize,int ySize)
{
    unsigned char *buff;
    int i;
    buff = lPtr->buffer;

    *buff++ = border[BOR_UL];		    // draw upper left
    *buff++ = windowBorderAttr;
    for(i=0;i<(xSize-2);i++)
     {
  		*buff++ = border[BOR_UM];	    // draw upper middle
  		*buff++ = windowBorderAttr;
     }
    *buff++ = border[BOR_UR];		    // draw upper right
    *buff++ = windowBorderAttr;

    for(i=0;i<(ySize-2);i++)
     {
		*buff = border[BOR_ML]; 	    // draw left edge
		*(buff+1) = windowBorderAttr;
		buff += xSize*charSize;
     }

    *buff++ = border[BOR_DL];		    // draw lower left
    *buff++ = windowBorderAttr;
    for(i=0;i<(xSize-2);i++)
     {
		*buff++ = border[BOR_DM];	    // draw lower middle
		*buff++ = windowBorderAttr;
     }
    *buff++ = border[BOR_DR];		    // draw lower right
    *buff++ = windowBorderAttr;

    buff = lPtr->buffer + (((xSize*2)-1)*charSize);
    for(i=0;i<(ySize-2);i++)
     {
		*buff = border[BOR_ML]; 	    // draw right edge
		*(buff+1) = windowBorderAttr;
		buff += xSize*charSize;
     }
    lPtr->flags |= layf_Changed;
}

//==============================================================================

void
DrawBottomBorder(_layer *lPtr, char border[8],int xSize,int ySize)
{
    unsigned char *buff;
    int i;
	buff = lPtr->buffer + ((xSize*charSize)*(ySize-1)) + charSize;
    for(i=0;i<(xSize-2);i++)
     {
		*buff++ = border[BOR_DM];	    // draw lower middle
		*buff++ = windowBorderAttr;
     }
}

//=============================================================================
// note: change this to printwindowstuff or something

void
PrintTitle(_window *pWindow,unsigned char attr)
{
	_gadget *gClose;
	assert( pWindow );
	_object *oPtr = ((_object *)pWindow->data);

	if(oPtr)							// in case oPtr is not valid
		gClose = FindGadget( &(pWindow->gadgBase), GGAD_CLOSE );
	else
		gClose = 0;

	if(pWindow->title)
		PrintLayAttrClip(pWindow,pWindow->title,(gClose?gClose->xPos+(gClose->xSize-1)+2:2),0,attr,pWindow->xSize-2);
	if(pWindow->windowNum)
	 {
		textBuffer[0] = pWindow->windowNum + '0';
		if(textBuffer[0] == ':')
			textBuffer[0] = '0';
		textBuffer[1] = 0;
		PrintLayAttr(pWindow,textBuffer,pWindow->xSize-(2+strlen(textBuffer)),0,attr);
	 }
}

//=============================================================================

void
FillWindow(_layer *lPtr)
{
	assert( lPtr );

    int i,j;
	unsigned char fillChar,fillAttr;
    unsigned char *buff;
	fillChar = lPtr->fillChar;
	fillAttr = lPtr->charAttr;
    buff = lPtr->buffer+(lPtr->xSize*charSize)+charSize;		// skip first row & first char of second row
	assert( buff );
    for(i=lPtr->ySize-2;i!=0;--i)
     {
		for(j=lPtr->xSize-2;j!=0;--j)
		 {
			*buff++ = fillChar;
			*buff++ = fillAttr;
		 }
		buff += charSize*2;				// skip right edge & left edge
     }
}


void
FillWindowSize(_layer *lPtr,int ySize2)
{
	assert( lPtr );

    int i,j;
	unsigned char fillChar,fillAttr;
    unsigned char *buff;
	fillChar = lPtr->fillChar;
	fillAttr = lPtr->charAttr;
    buff = lPtr->buffer+(lPtr->xSize*charSize)+charSize;		// skip first row & first char of second row
	assert( buff );
    for(i=ySize2;i;--i)
     {
		for(j=lPtr->xSize-2;j!=0;--j)
		 {
			*buff++ = fillChar;
			*buff++ = fillAttr;
		 }
		buff += charSize*2;				// skip right edge & left edge
     }
}

//=============================================================================

void
ResizeWindow(_window* pWindow,int newX,int newY)
{
    unsigned char* buff;
	buff = (unsigned char*)farmalloc(newX*newY*charSize);

	if(buff)
	 {
		farfree(pWindow->buffer);
		pWindow->xSize = newX;
		pWindow->ySize = newY;
	    pWindow->buffer = buff;
		pWindow->Fill( fillChar );					// FillLayer(pWindow,fillChar);
	    DrawBorder(pWindow,pWindow->border,pWindow->xSize,pWindow->ySize);
	    PrintTitle(pWindow,windowTitleAttr);
	 }
	else				// if cannot open buffer, failed, dont change window
	 {
		PrintError(ERROR_NOMEM);
	 }
}

//=============================================================================
// place in center of current window, if possible

unsigned int
AutoPosition(unsigned int backPos,unsigned int backSize,unsigned int frontSize,unsigned int edge)
{
	int pos;
	pos = (backPos+(backSize/2))-(frontSize/2);

	if(pos < 0)
		pos = 0;

	if(pos+frontSize > edge)
		pos = edge-frontSize;
	return(pos);
}

//=============================================================================
// place at specified coordinates, if posible

// pos = left edge of parent window
// frontSize = width of object to place
// width = width of display

unsigned int
AutoPlace(int pos,unsigned int frontSize,unsigned int edge)
{
	if(pos < 0)
		pos = 0;

	if(pos+frontSize > edge)
		pos = edge-frontSize;
	return(pos);
}

//=============================================================================
// place window in center of screen
unsigned int
AutoCenter(unsigned int size,unsigned int edge)
{
	return((edge/2)-(size/2));
}

//=============================================================================

void
UnHighLightTitle(_object *oPtr)
{
	assert( oPtr );
	_window *pWindow = (_window*)oPtr->layer;
	PrintTitle(pWindow,windowTitleAttr);
}

//=============================================================================

void
HighLightTitle(_object *oPtr)
{
	assert( oPtr );
	_window *pWindow = (_window*)oPtr->layer;
	PrintTitle(pWindow,windowTitleHighlightAttr);
}

//=============================================================================

void
ForceWindowResize ( _window *pWindow, _object *oPtr,FLAG(*inputRoutine)(_input *,_object *) )
{
	_input fakeInBase;

	assert( pWindow );
	pWindow->xSize = ( pWindow->xSize > pWindow->xMax ? pWindow->xMax : pWindow->xSize );
	pWindow->xSize = ( pWindow->xSize < pWindow->xMin ? pWindow->xMin : pWindow->xSize );
	pWindow->ySize = ( pWindow->ySize > pWindow->yMax ? pWindow->yMax : pWindow->ySize );
	pWindow->ySize = ( pWindow->ySize < pWindow->yMin ? pWindow->yMin : pWindow->ySize );

	if (( pWindow->xMax != pWindow->xMin) && (pWindow->yMax != pWindow->yMin))
	{
		ResizeWindow(pWindow,pWindow->xSize,pWindow->ySize);
		DeleteAllGadgets(&(pWindow->gadgBase));
		AddSysGadgets(pWindow);
		PrintTitle(pWindow,windowTitleMoveAttr);

		fakeInBase = inBase;			// structure to structure copy!
		fakeInBase.inputType = INP_RESIZE;
		(*inputRoutine)(&fakeInBase,oPtr);		// call every time resize occurs
		DrawGadgets(pWindow);
		refreshAll = boolean::TRUE;
	}
}

//=============================================================================

FLAG
MouseInsideWindow(_input *in,_layer *lPtr)
{
	assert( in );
	assert( lPtr );

	if(in->mouseX >= lPtr->xPos && in->mouseX < lPtr->xPos+lPtr->xSize &&
	   in->mouseY >= lPtr->yPos && in->mouseY < lPtr->yPos+lPtr->ySize)
		return(boolean::TRUE);
	return(boolean::FALSE);
}

//=============================================================================

_layer *
GetLayerMouseOver(_input *in)
{
	_layer *lPtr =  (layBase.Tail());

	while(lPtr != &layBase)
	 {
		if(MouseInsideWindow(in,lPtr))
			return(lPtr);
		lPtr = lPtr->prev;
	 }
	return(NULL);
}

//=============================================================================

void
ActivateFrontWindow(void)
{
	_layer *lPtr;
	_object *oPtr;

	while( (layBase.Tail())->data == 0)  			// insure input window is found
		 (layBase.Tail())->ToBack();

	if(frontObj->layer->deactivateRoutine != 0)
		frontObj->layer->deactivateRoutine(frontObj);

	lPtr =  (layBase.Tail());
	frontObj = (_object *)lPtr->data;


	while (!(lPtr->data) || (_object *)lPtr->data == cmdObjPtr)
		lPtr = lPtr->prev;

	if(frontObj->layer->activateRoutine != 0)
		frontObj->layer->activateRoutine(frontObj);

	UpdateCursor();
}

//=============================================================================

