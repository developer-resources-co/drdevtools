/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Description:	Class Code Support for MapAnim

File:			MAPANIM.C

See Also:		MAPANIM.H (class definition for MapAnim)

-------------------------------------------------------------------------- */

/* compiler dependant */

#include <stdlib.h>
#include <limits.h>
#include <assert.h>


/* module dependant */

#include "debug.h"
#include "funcdef.h"
#include "alexdef.h"
#include "spanim.h"
#include "vmalloc.h"
#include "camera.h"
#include "vdp.h"

#include "mapanim.h"


#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b) )
#define MIN(a,b) ((a) < (b) ? (a) : (b) )
#endif

#ifndef P2C
#define P2C(a) ((a)>>3)
#endif


/* --------------------------------------------------------------------------

Function:		MapAnimAdd

Scope:			PUBLIC

Usage:			this = MapAnimAdd(pAnimBase);
				MapAnim	*this;			- pointer to instance of MapAnim
				MapAnim	*pAnimBase;		- pointer to first MapAnim object in chain

Returns:		A pointer to new instance of MapAnim

Description:	Creates a new instance of MapAnim and places it at the
				end of the Anim/MapAnim chain of objects.

-------------------------------------------------------------------------- */

PUBLIC MapAnim *
MapAnimAdd(Anim *pAnimBase)
{
	Anim		*pAnim = pAnimBase;
	MapAnim	*pNewMapAnim = MapAnimNew();

	assert(pNewMapAnim);

	if (pAnimBase) {								/* if base pointer not NULL */
		while (pAnim->pNext)
			pAnim = pAnim->pNext;					/* loop till end of chain found */

		pAnim->pNext = (Anim *) pNewMapAnim;		/* point to new object now at end of chain */
	}

	return( pNewMapAnim );
}





/* --------------------------------------------------------------------------

Function:		MapAnimNew

Scope:			PUBLIC

Usage:			this = MapAnimNew();
				MapAnim	*this;			- pointer to instance of MapAnim

Returns:		A pointer to MapAnim passed to the routine

Description:	Allocates space on the heap for an instance of MapAnim and
				sets all class variables and method pointers to default
				values which may be overridden.

-------------------------------------------------------------------------- */

PUBLIC MapAnim *
MapAnimNew()
{
	MapAnim	*this = malloc(sizeof(MapAnim));

	assert(this);

	if (this) {
		MapAnimConstruct(this);
		this->fHeapAllocated = TRUE;
	}

	return( this );
}





/* --------------------------------------------------------------------------

Function:		MapAnimConstruct

Scope:			PUBLIC

Usage:			thisDup = MapAnimConstruct(this);
				MapAnim	*thisDup;	- returned pointer to instance of MapAnim
				MapAnim	*this;		- pointer to instance of MapAnim

Returns:		A pointer to MapAnim passed to the routine

Description:	Sets all class variables and method pointers to default
				values which may be overridden

Note:			If and methods are overridden, make sure you have a full
				understanding of what the original methods did. Chances
				are, they did a lot of the code that you wanted to do
				anyway. If you need to do more, think about calling
				these standard methods within your own method code.

-------------------------------------------------------------------------- */

PUBLIC MapAnim *
MapAnimConstruct( MapAnim* this )
{
	assert(this);

	AnimConstruct((Anim *) this);
	this->fPermSprite = TRUE;
	this->pScroll = NULL;
	this->mRenderSprite = MapAnimRenderMap;
	this->mSpriteMethod = NULL;

	return this;
}





/* --------------------------------------------------------------------------

Function:		MapAnimTick

Scope:			PUBLIC

Usage:			MapAnimTick(pMapAnim);
				MapAnim	*pMapAnim;			- pointer to instance of MapAnim

Returns:		Nothing

Description:	Main routine usually called previous to render routine

-------------------------------------------------------------------------- */

PUBLIC void
MapAnimTick( MapAnim* this )
{
	AnimTick((Anim *) this);
}





/* --------------------------------------------------------------------------

Function:		MapAnimRenderMap

Scope:			PUBLIC

Usage:			MapAnimRenderMap(pRender, pMapAnim);
				Render	*pRender;			- pointer to instance of Render
				MapAnim	*pMapAnim;			- pointer to instance of MapAnim

Returns:		Nothing

Description:	Main routine usually called previous to render routine

-------------------------------------------------------------------------- */

PUBLIC void
MapAnimRenderMap( WORD xPosition, WORD yPosition, Anim* thisMapAnim )
{
const TMapAnim	 	*pSequence;
const TMapAnimFrame *pFrame;
	short			nNumFrames;
	short			nFrame;
	unsigned short	desc;
	MapAnim		*this = (MapAnim *) thisMapAnim;


	short			camxPos,camyPos;
	short			camxSize,camySize;
	short			xPosLocal,yPosLocal;
	short			xStart,yStart;
	short			xStop,yStop;

	MapAnimTick(this);

	nFrame = this->nFrameNumber;

	pSequence = (const TMapAnim *)this->oAnims[this->nAnimNumber];
	nNumFrames = pSequence->nNumFrames;

	if ( nFrame >= nNumFrames )
		nFrame = nFrame % nNumFrames;

	pFrame = pSequence->frame + nFrame;
	assert(pFrame);

	camxPos = pSpriteCamera->xPos;
	camyPos = pSpriteCamera->yPos;

	camxSize = pSpriteCamera->xSize;	/* 320 */
	camySize = pSpriteCamera->ySize;	/* 224 */

	xPosLocal = xPosition;
	yPosLocal = yPosition;

	if (camxPos > (xPosLocal + this->xAnimSize))  		/* if camera is to my right, and I'm visible, */
		camxPos -= (this->pScroll->wMapWidth*16);		/* it must be wrapped */

	xPosLocal = MAX(camxPos,xPosLocal);
	xStart = MAX(0,camxPos - xPosition);
	xStop = MIN((this->xAnimSize-1),camxPos + camxSize - xPosition);

	yPosLocal = MAX(camyPos,yPosLocal);
	yStart = MAX(0,camyPos - yPosition);
	yStop = MIN((this->yAnimSize -1),camyPos + camySize - yPosition);

	if ( xStart < 0 )
		return;

	if (xStop <= xStart)
		return;

	if ( xStop >= this->xAnimSize)
		return;

	if ( yStart < 0 )
		return;

	if (yStop <= yStart)
		return;

	if ( yStop >= this->yAnimSize)
		return;

	if (this->nAnimNumber != this->nPrevAnimNumber)
	{
		if (!this->fPermSprite)
			if(this->wCharBase)
				vfree(this->wCharBase);
		this->wCharBase = LoadChars(pSequence->pCharSet);
	}
	desc = this->wMap._Word;

	PlotSubMap
		(P2C(xPosLocal)&63,P2C(yPosLocal)&31,
		P2C(xStart),P2C(yStart),
		P2C(xStop),P2C(yStop),
		desc,
		(TPlayField *) this->pScroll,
		pFrame->map);

	this->nPrevFrameNumber = this->nFrameNumber;
	this->nPrevAnimNumber = this->nAnimNumber;

	if (this->pNext)
		this->pNext->mRenderSprite(xPosition, yPosition, this->pNext);
}





/* --------------------------------------------------------------------------

Function:		MapAnimOnScreen

Scope:			PUBLIC

Usage:			MapAnimOnScreen(pMapAnim);
				MapAnim	*pMapAnim;			- pointer to instance of MapAnim

Returns:		Nothing

Description:	Calls the mOnScreen method for both for itself and
				the next object in the link.

-------------------------------------------------------------------------- */

PUBLIC void
MapAnimOnScreen( MapAnim* this )
{
	/* call same routine for next MapAnim object in chain if present */

	if (this->pNext)
		MapAnimOnScreen((MapAnim *)this->pNext);
}





/* --------------------------------------------------------------------------

Function:		MapAnimOffScreen

Scope:			PUBLIC

Usage:			MapAnimOffScreen(pMapAnim);
				MapAnim	*pMapAnim;			- pointer to instance of MapAnim

Returns:		Nothing

Description:	Calls the mOffScreen method for both for itself and
				the next object in the link. Frees VRAM used to render
				object.

-------------------------------------------------------------------------- */

PUBLIC void
MapAnimOffScreen( MapAnim* this )
{
	/* call same routine for next MapAnim object in chain if present */

	FreeObjectVRAM((Anim *) this);

	if (this->pNext)
		MapAnimOffScreen((MapAnim *)this->pNext);
}



PUBLIC void
MapAnimComputeRectangle(Render *p,TRect *rrr)
{
	rrr->x1 = p->xPos;
	rrr->x2 = p->xPos + p->xSize;
	rrr->y1 = p->yPos;
	rrr->y2 = p->yPos + p->ySize;

}



/* CMAPANIM.C -- EOF */



