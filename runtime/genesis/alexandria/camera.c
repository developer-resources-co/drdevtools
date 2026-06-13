/* --------------------------------------------------------------------------

                          Alexandria Genesis Library
                          --------------------------

Title:		Camera Object

File:		CAMERA.C

Overview:

			The camera object has the responsibility of controlling the
			on-screen scrolling to keep the screen centered around a
			specified object. Any on-screen object can be used as long
			it has a valid x,y position. The camera object will also
			take into account the location of null meta tiles preventing
			the screen from scrolling into those areas. At a null meta tile
			boundary, the camera will lock into position and the object the
			camera is following can continue to move right up until the meta
			tile boundary.

-------------------------------------------------------------------------- */

/* compiler specific */

#include <stdlib.h>
#include <assert.h>


/* module specific */

#include "debug.h"
#include "alexdef.h"
#include "gentypes.h"
#include "mapmngr.h"
#include "scroll.h"
#include "objmath.h"
#include "list.h"
#include "camera.h"


/* private functions */

PRIVATE	void	ComputeSpriteRectangle(register Render *p, register TRect *rrr);






/* --------------------------------------------------------------------------

Function:		InitCamera

Scope:			PUBLIC

Usage:			pCamera = InitCamera(pObj, list, scrollStruct)
				Camera			*pCamera		- returned pointer to instance
												  of camera object
				Object			*pObj			- pointer to object to follow
				TScrollField	*scrollStruct	- pointer to scroll structure of
												  background plane to be controlled

Returns:		pointer to instace if successful, NULL if not

Description:	Initialize the camera object with the information necessary
				to track the specified object.

Note:			InitMetaScroll must have been called first.

-------------------------------------------------------------------------- */

PUBLIC Camera *
InitCamera(short nList, Render *pObj, TScrollField *pScrollStruct)
{
	Camera *this = CameraNew();
	short	stat;

	assert(this);
	assert(pObj);
	assert(pScrollStruct);

	if (this) {

		stat = ListAddObject((Object *) this, nList);

		assert(stat == LIST_NoError);

		if (stat == LIST_NoError) {
			this->pTrackObj = pObj;
			this->pScroll = pScrollStruct;
			this->xPos = pScrollStruct->xPosition;
			this->yPos = pScrollStruct->yPosition;
		}

		else {
			this->mDestruct((Object *) this);
			this = NULL;
		}
	}

	return this;
}




/* --------------------------------------------------------------------------

Function:		CameraNew

Scope:			PUBLIC

Usage:			this = CameraNew();
				Camera	*this;			- pointer to instance of Camera

Returns:		A pointer to Camera passed to the routine

Description:	Allocates space on the heap for an instance of Camera and
				sets all class variables and method pointers to default
				values which may be overridden.

-------------------------------------------------------------------------- */

PUBLIC Camera *
CameraNew()
{
	Camera	*this = malloc(sizeof(Camera));

	assert(this);

	if (this) {
		CameraConstruct(this);
		this->mDestruct = (Func_v) CameraDestructHeap;			/* override default destructor */
	}

	return( this );
}





/* --------------------------------------------------------------------------

Function:		ConstructCamera

Scope:			PUBLIC

Usage:			thisDup = ConstructCamera(this);
				Camera	*thisDup;		- returned pointer to instance of Camera
				Camera	*this;			- pointer to instance of Camera

Returns:		A pointer to Camera passed to the routine

Description:	Sets all class variables and method pointers to default
				values which may be overridden

Note:			If and methods are overridden, make sure you have a full
				understanding of what the original methods did. Chances
				are, they did a lot of the code that you wanted to do
				anyway. If you need to do more, think about calling
				these standard methods within your own method code.

-------------------------------------------------------------------------- */

PUBLIC Camera *
CameraConstruct( Camera* this )
{
	assert(this);

	RenderConstruct((Render *) this);

	this->mTick = (Func_v) CameraTick;
	this->mDestruct = (Func_v) CameraDestruct;
	this->xScale = Camera_xScale;
	this->yScale = Camera_yScale;
	this->xSize = Camera_xPixelSize;
	this->ySize = Camera_yPixelSize;
	this->camXOffset = Camera_xOffset;
	this->camYOffset = Camera_yOffset;
	this->fVisible = FALSE;

	return( this );
}





/* --------------------------------------------------------------------------

Function:		CameraDestruct

Scope:			PUBLIC

Usage:			CameraDestruct(this);
				Camera	*this;			- pointer to instance of Camera

Returns:		Nothing

Description:	Removes this instance of Camera by eliminating all pointers
				to it from all lists. It will also free any allocated instances
				of Anim objects.

Note:			This routine is not aware of whether this instance of Camera
				was allocated from the heap or the stack. Removal from the
				heap is done via the routine CameraDestructHeap. This is
				the default destructor used is an instance of Camera was
				created by NOT using the CameraNew() function.

-------------------------------------------------------------------------- */

PUBLIC void
CameraDestruct( Camera* this )
{
	assert(this);

	RenderDestruct((Render *) this);
}





/* --------------------------------------------------------------------------

Function:		CameraDestructHeap

Scope:			PUBLIC

Usage:			CameraDestructHeap(this);
				Camera	*this;			- pointer to instance of Camera

Returns:		Nothing

Description:	Removes this instance of Camera by eliminating all pointers
				to it from all lists.

Note:			This routine is not aware of whether this instance of Camera
				was allocated from the heap or the stack. Removal from the
				head is done via the routine CameraDestructHeap. This is
				the default destructor used is an instance of Camera was
				created by NOT using the CameraNew() function.

-------------------------------------------------------------------------- */

PUBLIC void
CameraDestructHeap( Camera* this )
{
	assert(this);

	CameraDestruct(this);

	if (this)
		free(this);
}





/* --------------------------------------------------------------------------

Function:		CameraTick

Scope:			PUBLIC

Usage:			CameraTick(this);
				Camera	*this;			- pointer to instance of Camera

Returns:		Nothing

Description:	Causes all animations to be rendered

-------------------------------------------------------------------------- */

PUBLIC void
CameraTick(Camera *this)
{
	short	xxObj 	= this->pTrackObj->xPos - this->camXOffset,	/* position of object to follow */
			yyObj 	= this->pTrackObj->yPos - this->camYOffset,
			camXPos = this->xPos,							/* position of UL corner */
			camYPos = this->yPos,							/* of camera (constrained to map */
			mapXPixelSize = this->pScroll->wMapWidth * 8,
			mapYPixelSize = this->pScroll->wMapHeight * 8;

	assert(this);

	RenderTick((Render *) this);

	if (this->xScale != 0x10000)
		xxObj = (xxObj * this->xScale) >> 16;

	if (this->yScale != 0x10000)
		yyObj = (yyObj * this->yScale) >> 16;

/* determine shortest distance to scroll towards */

	if ((xxObj - camXPos) > mapXPixelSize / 2)
		xxObj -= mapXPixelSize;

	if ((camXPos - xxObj) > mapXPixelSize / 2)
		xxObj += mapXPixelSize;

	if ((yyObj - camYPos) > mapYPixelSize / 2)
		yyObj -= mapYPixelSize;

	if ((camYPos - yyObj) > mapYPixelSize / 2)
		yyObj += mapYPixelSize;

/* check for x axis meta tile limits */

	if (xxObj < camXPos) {					/* if scrolling left */

		/* if destination x coordinate in a null meta tile  */

		if (GetMetaTileNumber((xxObj & (mapXPixelSize - 1)) >> MTile_xPixSizeP2,
							 ((camYPos) & (mapYPixelSize - 1)) >> MTile_yPixSizeP2) == NULL_TILE
			|| GetMetaTileNumber((xxObj & (mapXPixelSize - 1)) >> MTile_xPixSizeP2,
							 ((camYPos + Camera_yPixelSize - 1) & (mapYPixelSize - 1)) >> MTile_yPixSizeP2) == NULL_TILE) {
			xxObj = (xxObj + MTile_xPixSize) & ~(MTile_xPixSize - 1);
		}
	}

	else if (xxObj > camXPos) {				/* if scrolling right */
		if (GetMetaTileNumber(((xxObj + Camera_xPixelSize - 1) & (mapXPixelSize - 1)) >> MTile_xPixSizeP2,
							 (camYPos & (mapYPixelSize - 1)) >> MTile_yPixSizeP2) == NULL_TILE
			|| GetMetaTileNumber(((xxObj + Camera_xPixelSize - 1) & (mapXPixelSize - 1)) >> MTile_xPixSizeP2,
							 ((camYPos + Camera_yPixelSize - 1) & (mapYPixelSize - 1)) >> MTile_yPixSizeP2) == NULL_TILE) {
			xxObj = ((xxObj + Camera_xPixelSize - 1) & ~(MTile_xPixSize - 1)) - Camera_xPixelSize;
		}
	}
#ifdef xxx

/* check for y axis map boundary limits */

	if (yyObj < 0)										/* upper screen limit */
		yyObj = 0;

	else if ((yyObj + Camera_yPixelSize) > mapYPixelSize)	/* lower screen limit */
		yyObj = mapYPixelSize - Camera_yPixelSize;

#endif


/* check for y axis meta tile limits */

	if (yyObj < camYPos) {					/* if scrolling up */
		/* if destination y coordinate in a null meta tile  */

		if (GetMetaTileNumber((camXPos & (mapXPixelSize - 1)) >> MTile_xPixSizeP2,
							 (yyObj & (mapYPixelSize - 1)) >> MTile_yPixSizeP2) == 0
			|| GetMetaTileNumber(((camXPos + Camera_xPixelSize - 1) & (mapXPixelSize - 1)) >> MTile_xPixSizeP2,
							 (yyObj & (mapYPixelSize - 1)) >> MTile_yPixSizeP2) == 0) {

			yyObj = (yyObj + MTile_yPixSize) & ~(MTile_yPixSize - 1);
		}
	}

	else if (yyObj > camYPos) {				/* if scrolling right */
		if (GetMetaTileNumber((camXPos & (mapXPixelSize - 1)) >> MTile_xPixSizeP2,
							 ((yyObj + Camera_yPixelSize - 1) & (mapYPixelSize - 1)) >> MTile_yPixSizeP2) == 0
			|| GetMetaTileNumber(((camXPos + Camera_xPixelSize - 1) & (mapXPixelSize - 1)) >> MTile_xPixSizeP2,
							 ((yyObj + Camera_yPixelSize - 1) & (mapYPixelSize - 1)) >> MTile_yPixSizeP2) == 0) {

			yyObj = ((yyObj + Camera_yPixelSize - 1) & ~(MTile_yPixSize - 1)) - Camera_yPixelSize;
		}
	}



/* perform scrolling */

	if (xxObj > camXPos)
		ScrollMetaRight(this->pScroll,xxObj - camXPos);

	else if (xxObj < camXPos)
		ScrollMetaLeft(this->pScroll,camXPos - xxObj);

	if (yyObj > camYPos)
		ScrollMetaDown(this->pScroll,yyObj - camYPos);

	else if (yyObj < camYPos)
		ScrollMetaUp(this->pScroll,camYPos - yyObj);

	this->xPos = xxObj & (mapXPixelSize - 1);
	this->yPos = yyObj & (mapYPixelSize - 1);

}





/*--------------------------------------------------------------------------*/
/*
 * Function:	InteresectCamera
 * Scope:		Public
 * Author: 		SLS
 * Purpose:		Boolean test, is object (p1) within the field of view of
 *				the camera?
 */

PUBLIC BOOL
IntersectCamera(Camera *pCamera, Render *pObj)
{
register Camera	*pCam;
		 TRect		ObjRect;
		 TRect		CamRect;
		 BOOL 		stat;

	pCam = pCamera;		/* make it local to force optimization */

	ComputeSpriteRectangle(pObj,&ObjRect);

	CamRect.x1 = pCam->xPos;
	CamRect.x2 = pCam->xPos+pCam->xSize-1;

	CamRect.y1 = pCam->yPos;
	CamRect.y2 = pCam->yPos+pCam->ySize-1;

	stat = RectInRectNW(&ObjRect,&CamRect);				/* try as they are */

	/* if no collision, then check if object pObj past x and y axis map boundary */

	if (!stat && (ObjRect.x2 >= mapXPixelSize) && (ObjRect.y2 >= mapYPixelSize))
	{
		TRect ObjRect2 = ObjRect;

		ObjRect2.x2 -= (mapXPixelSize-1);
		ObjRect2.x1 = 0;
		ObjRect2.y2 -= (mapYPixelSize-1);
		ObjRect2.y1 = 0;

		stat = RectInRectNW(&ObjRect2,&CamRect);
	}

	/* if no collision, then check if object pObj past x axis map boundary */

	if (!stat && (ObjRect.x2 >= mapXPixelSize))
	{
		TRect ObjRect2 = ObjRect;

		ObjRect2.x2 -= (mapXPixelSize-1);
		ObjRect2.x1 = 0;

		stat = RectInRectNW(&ObjRect2,&CamRect);
	}

	/* if no collision, then check if object pObj past y axis map boundary */

	if (!stat && (ObjRect.y2 >= mapYPixelSize))
	{
		TRect ObjRect2 = ObjRect;

		ObjRect2.y2 -= (mapYPixelSize-1);
		ObjRect2.y1 = 0;

		stat = RectInRectNW(&ObjRect2,&CamRect);
	}

	/* if no collision, then check if camera past x and y axis map boundary */

	if (!stat && (CamRect.x2 >= mapXPixelSize) && (CamRect.y2 >= mapYPixelSize))
	{
		TRect ObjRect2 = ObjRect;

		ObjRect2.x1 += mapXPixelSize;
		ObjRect2.x2 += mapXPixelSize;
		ObjRect2.y1 += mapYPixelSize;
		ObjRect2.y2 += mapYPixelSize;

		stat = RectInRectNW(&ObjRect2,&CamRect);
	}

	/* if no collision, then check if camera past x axis map boundary */

	if (!stat && (CamRect.x2 >= mapXPixelSize))
	{
		TRect ObjRect2 = ObjRect;

		ObjRect2.x1 += mapXPixelSize;
		ObjRect2.x2 += mapXPixelSize;

		stat = RectInRectNW(&ObjRect2,&CamRect);
	}

	/* if no collision, then check if camera past y axis map boundary */

	if (!stat && (CamRect.y2 >= mapYPixelSize))
	{
		TRect ObjRect2 = ObjRect;

		ObjRect2.y1 += mapYPixelSize;
		ObjRect2.y2 += mapYPixelSize;

		stat = RectInRectNW(&ObjRect2,&CamRect);
	}

	return stat;
}



/*--------------------------------------------------------------------------*/
/*
 * Function:	ComputeSpriteRectangle
 * Scope:		Public
 * Author: 		SLS
 * Purpose:		set a rect structure to the bounding rectangle of a
 *				Render Object
 */

PRIVATE void
ComputeSpriteRectangle(register Render *p,register TRect *rrr)
{
	short	xSize2;
	short	ySize;

	xSize2 = p->xSize / 2;
	ySize = p->ySize;

	rrr->x1 = p->xPos - xSize2;
	rrr->x2 = p->xPos + xSize2 - 1;
	rrr->y1 = p->yPos - ySize;
	rrr->y2 = p->yPos;
}

/* EOF */

