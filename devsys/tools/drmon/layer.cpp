/*===========================================================================*/
/*==   layer.cpp: layer handling functions                                =*/
/*===========================================================================*/

#include <assert.h>

#include        "base.hpp"
#include        "global.hpp"
#include        "screen.hpp"
#include        "list.hpp"
#include        "layer.hpp"

#include	"config.hpp"

/*===========================================================================*/

FLAG refreshAll,refreshEnable = boolean::TRUE;

//=============================================================================

_layerBase layBase;

_layerBase::_layerBase() : _layer()
	{
	next =
	prev = NULL;
	refreshAll = boolean::TRUE;
	}


_layerBase::~_layerBase()
	{
	_layer* lPtr;

	if ( lPtr = this->next )
		while ( lPtr->next )
			delete lPtr->next;
	}


class _layer*
_layerBase::Tail()
	{
	for ( _layer* lPtr = (_layer*)&layBase; lPtr->next; lPtr=lPtr->next )
		;

	assert( lPtr );
	return( lPtr );
	}


void
_layerBase::Display( char far *buffer,int xOffset,int yOffset,char backgroundChar, char backgroundAttr,int screenWidth,int ScreenHeight )
	{
	if ( refreshEnable )
		{
		_layer* lPtr = next;

		if ( refreshAll )
			ClearScrBuff(buffer,backgroundChar,backgroundAttr,screenWidth*screenHeight*charSize);
		else    // skip first layers that have not been changed
			while( lPtr && ((lPtr->flags & layf_Changed) == 0))
				lPtr = lPtr->next;

		for ( ; lPtr; lPtr = lPtr->next )
			lPtr->Render(buffer,xOffset,yOffset,screenWidth,screenHeight);
		}
	}


void
_shadowedLayer::Render(char* buffer, int xOffset, int yOffset, int screenWidth, int screenHeight)
{

	RenderShadow(buffer, xOffset+xShadow, yOffset+yShadow, screenWidth, screenHeight);
	_layer::Render(buffer, xOffset, yOffset, screenWidth, screenHeight);
}


void
DisplayLayers(char far *buffer,int xOffset,int yOffset,char backgroundChar, char backgroundAttr,int screenWidth,int ScreenHeight)
	{
	layBase.Display( buffer, xOffset, yOffset, backgroundChar, backgroundAttr, screenWidth, screenHeight );
	}

//=============================================================================

class _layer*
_layer::ToFront()
	{
	_layer* lPtrTop = NULL;

	_layer* lPtr = this;

	_layer *l2Ptr;
									// unlink from current position
   lPtr->prev->next = lPtr->next;
	if ( lPtr->next )
	lPtr->next->prev = lPtr->prev;
									// add to end of list
	l2Ptr =  (layBase.Tail());
	l2Ptr->next = lPtr;

	lPtr->next = NULL;
	lPtr->prev = l2Ptr;
									// remember to re-draw
   refreshAll = boolean::TRUE;

	lPtr->flags |= layf_Front;

	return( lPtrTop );
	}


class _layer*
_layer::ToBack()
	{
	_layer* lPtr = this;
									// unlink from current position
    lPtr->prev->next = lPtr->next;
	if (lPtr->next)
	lPtr->next->prev = lPtr->prev;
									// add to begining of list
	lPtr->next = layBase.next;
	layBase.next = lPtr;
	lPtr->prev = &layBase;
	lPtr->next->prev = lPtr;
									// remember to re-draw
    refreshAll = boolean::TRUE;

	 (layBase.Tail())->flags |= layf_Front;
	lPtr->flags &= ~layf_Front;

	return( NULL );
	}


void
_layer::Fill( unsigned char fillChar )
	{
#if 0
	unsigned char far* localBuffer = buffer;

	asm les di,localBuffer;
	_AH = charAttr;
	_AL = fillChar;
	_CX = xSize * ySize;
		// -> es:di
	asm rep stosw;
#else
	unsigned char far* buff = buffer;
	for ( int i=xSize*ySize; i!=0; --i )
		{
		*buff++ = fillChar;
		*buff++ = charAttr;
		}
#endif
	}

//=============================================================================
// note: this puts layer at bottom of list

_layer::_layer( int xPos,int yPos,int xSize,int ySize,unsigned char charAttr,unsigned char fillChar )
	{
	_layer* lPtr = this;

	lPtr->xPos = xPos;
	lPtr->yPos = yPos;
	lPtr->xSize = xSize;
	lPtr->ySize = ySize;

	lPtr->oldXPos = xPos;
	lPtr->oldYPos = yPos;
	lPtr->oldXSize = xSize;
	lPtr->oldYSize = ySize;

	lPtr->xMax = xSize;
	lPtr->xMin = xSize;
	lPtr->yMax = ySize;
	lPtr->yMin = ySize;
	lPtr->next = 0;
	lPtr->cursorX = 0;
	lPtr->cursorY = 0;
	_layer* l2Ptr =  (layBase.Tail());
	lPtr->prev = l2Ptr;
	l2Ptr->next = lPtr;             // add to end of list
	lPtr->flags = layf_Changed|layf_Front;
	lPtr->charAttr = charAttr;
	lPtr->fillChar = fillChar;
	lPtr->activateRoutine = NULL;
	lPtr->deactivateRoutine = NULL;
	lPtr->data = 0;
	lPtr->cursorOn = boolean::FALSE;
	lPtr->cStartLine = 6;
	lPtr->cStopLine = 7;
	lPtr->gadgetHdr.prev = 0;
	lPtr->gadgetHdr.next = 0;
	lPtr->windowNum = 0;
	if ( lPtr->buffer = (unsigned char *)farmalloc(ySize*xSize*charSize) )
		lPtr->Fill( fillChar );                                 // FillLayer(lPtr,fillChar);
#if 0
	else                            // if cannot open buffer, failed
		{
		farfree( lPtr );
		lPtr = 0;
		}
#endif

	_layer* oldFrontLay =  (layBase.Tail());
	oldFrontLay->flags &= ~layf_Front;
	}


_layer::_layer()
	{
	}

_layer::~_layer()
{
	if ( buffer ) farfree( buffer );

	if(prev)
		prev->next = next;
	if ( next )
	next->prev = prev;
	refreshAll = boolean::TRUE;
}

//=============================================================================

void
_layer::Render(char* destBuffer, int xOffset, int yOffset, int screenWidth, int screenHeight)
{
    int clippedXSize = xSize;
    int clippedYSize = ySize;
	int clippedXPos = xPos+xOffset;
	int clippedYPos = yPos+yOffset;

    unsigned char* sBuff = buffer;
	assert( sBuff );
	int ysAdd = 0;
									// left clipping
	if(clippedXPos < 0)
	 {
		if((clippedXPos+clippedXSize) < 0)
			return;
		clippedXSize -= -clippedXPos;
		sBuff += (-clippedXPos)*charSize;
		ysAdd = -clippedXPos*charSize;
		clippedXPos = 0;
	 }
									// right clipping
	if(clippedXPos+clippedXSize > screenWidth)
	 {
		ysAdd  += ((clippedXPos+clippedXSize)-screenWidth)*charSize;
		clippedXSize -= (clippedXPos+clippedXSize)-screenWidth;
	 }
									// top clipping
	if(clippedYPos < 0)
	 {
		clippedYSize -= -clippedYPos;
		sBuff += (-clippedYPos)*(xSize*charSize);
		clippedYPos = 0;
	 }

									// bottom clipping
	if((clippedYSize+clippedYPos) > screenHeight)
		clippedYSize -= (clippedYSize+clippedYPos)-screenHeight;
									// get buffer pointers
	char far *dBuff;
    dBuff = destBuffer+(clippedYPos*screenWidth*charSize)+(clippedXPos*charSize);
    int yAdd = (screenWidth - clippedXSize)*2;

	if(clippedXSize > 0 && clippedYSize > 0)
		for(int y=0;y<clippedYSize;++y)
			{
			for(int x=0;x<clippedXSize;++x)
				{
				*dBuff++ = *sBuff++;
				*dBuff++ = *sBuff++;
				}
			dBuff += yAdd;
			sBuff += ysAdd;
			}
}

//=============================================================================

static ubyte shadowTable[16] =
{
	0,0,0,0,0,0,0,8,
	0,1,2,3,4,5,6,7
};

void
_shadowedLayer::RenderShadow(char* destBuffer, int xOffset, int yOffset, int screenWidth, int screenHeight)
{
    int clippedXSize = xSize;
    int clippedYSize = ySize;
	int clippedXPos = xPos+xOffset;
	int clippedYPos = yPos+yOffset;

	int ysAdd = 0;
									// left clipping
	if(clippedXPos < 0)
	 {
		if((clippedXPos+clippedXSize) < 0)
			return;
		clippedXSize -= -clippedXPos;
		ysAdd = -clippedXPos*charSize;
		clippedXPos = 0;
	 }
									// right clipping
	if(clippedXPos+clippedXSize > screenWidth)
	 {
		ysAdd  += ((clippedXPos+clippedXSize)-screenWidth)*charSize;
		clippedXSize -= (clippedXPos+clippedXSize)-screenWidth;
	 }
									// top clipping
	if(clippedYPos < 0)
	 {
		clippedYSize -= -clippedYPos;
		clippedYPos = 0;
	 }

									// bottom clipping
	if((clippedYSize+clippedYPos) > screenHeight)
		clippedYSize -= (clippedYSize+clippedYPos)-screenHeight;
									// get buffer pointers
	char far *dBuff;
    dBuff = destBuffer+(clippedYPos*screenWidth*charSize)+(clippedXPos*charSize);
    int yAdd = (screenWidth - clippedXSize)*2;

	if(clippedXSize > 0 && clippedYSize > 0)
		for(int y=0;y<clippedYSize;++y)
			{
			for(int x=0;x<clippedXSize;++x)
			 {
				dBuff++;							// skip character
				ubyte attr = *dBuff;
				*dBuff++ = shadowTable[attr & 0xf] | (shadowTable[attr>>4]<<4);		// look up new attribute
			 }
			dBuff += yAdd;
			}
}

//=============================================================================

void
PrintLayAttrClip( class _layer *lPtr,char*text,int xPos,int yPos,unsigned char charAttr,int xClip)
	{
	assert( lPtr );

	int currXPos = xPos;

	unsigned char far* buff = lPtr->buffer +((xPos+(yPos*lPtr->xSize))*charSize);
	assert( buff );

	unsigned char far* endBuff = lPtr->buffer + ( ((lPtr->ySize * lPtr->xSize)-1) * charSize );

	unsigned char far* oldBuff = buff;

    lPtr->flags |= layf_Changed;
    while(*text && ( buff <= endBuff ) )
     {
		if(*text == '\n')
		 {
			buff = oldBuff + (lPtr->xSize*2);
			oldBuff = buff;
			text++;
			currXPos = xPos;
		 }
		else
		 {
			if(currXPos < xClip)
			 {
				*buff++ = *text;
				*buff++ = charAttr;
			 }
			text++;
			currXPos++;
		 }
     }
}

//=============================================================================

void
PrintLayAttr( class _layer *lPtr,char*text, int xPos, int yPos,unsigned char charAttr)
{
	PrintLayAttrClip(lPtr,text,xPos,yPos,charAttr,lPtr->xSize);
}

void
PrintLay( class _layer* lPtr, char* text, int xPos, int yPos)
{
	PrintLayAttr(lPtr,text,xPos,yPos,lPtr->charAttr);
}

//=============================================================================

// for windowing functions, see window.c
//=============================================================================
// note: does no clipping, assumes is passed reasonable numbers

void cdecl
FillRectLayer( class _layer* lPtr,
	int xStart, int yStart, int xSize, int ySize,
	unsigned char fillChar, unsigned char charAttr )
{
	assert( lPtr );

	int xCount;
	unsigned char far *buffer,far *b2;

	buffer = lPtr->buffer + (xStart*charSize) + (yStart*charSize*lPtr->xSize);
	while(ySize--)
	 {
		b2 = buffer;
		xCount = xSize;
		while(xCount--)
		 {
			*buffer++ = fillChar;
			*buffer++ = charAttr;
		 }
		buffer = b2 + (lPtr->xSize*charSize);
	 }
}

//=============================================================================

