/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Title:		Math and Distance Routines for Objects Derrived from Render

File:		CRENDER.C

See Also:	CRENDER.H

-------------------------------------------------------------------------- */

/* compiler depandant */

#include <stdlib.h>
#include <assert.h>


/* module dependant */

#include "render.h"
#include "alexdef.h"
#include "mapmngr.h"
#include "objmath.h"





/* --------------------------------------------------------------------------

Function:		ObjInObj

Scope:			PUBLIC

Usage:			status = ObjInObj(pObj1, pObj2)
				BOOL		status			- TRUE if objects intersect
				Render		*pObj1, pObj2	- pointers to objects to check

Returns:		TRUE if object rectangles intercept, FALSE if not

Description:	A check is done to determine if the bounding rectangles of
				the specified objects intersect at any point. This routine
				will also take into account a wrap-around universe by checking
				if either rectangle over-steps the right side of the map. If
				so, the rectangle will be split into two such that the starting
				location of the first half will be the same as the original
				rectangle but the width will be adjust to end at the map
				boundary. The second rectangle will start at x location 0 and
				the width will be adjust to reflect how far the original
				rectangle overstepped the map boundary. The splitting will
				only occur if a collision check of the original rectangles
				returned a FALSE.

Note:			MapManagerInit must have been called. xSize and ySize must
				best set to a valid value in the object structures.

-------------------------------------------------------------------------- */

PUBLIC BOOL ObjInObj(Render *p1, Render *p2)
{
	short	p1XEnd = p1->xPos + (p1->xSize - 1) / 2,	/* determine ending x coordinates */
			p2XEnd = p2->xPos + (p2->xSize - 1) / 2;

	BOOL	stat = ObjInObjNW(p1, p2);				/* try as they are */

	Render	tmp;

	/* if no collision, then check if object p1 past x and y map boundary */

	if (stat == FALSE && (p1XEnd >= mapXPixelSize) && (p1->yPos >= mapYPixelSize)) {
		tmp.xSize = p2->xSize;
		tmp.ySize = p2->ySize;
		tmp.xPos = p2->xPos + mapXPixelSize;
		tmp.yPos = p2->yPos + mapYPixelSize;

		stat = ObjInObjNW(&tmp,p1);
	}

	/* if no collision, then check if object p1 past x map boundary */

	if (stat == FALSE && (p1XEnd >= mapXPixelSize)) {
		tmp.xSize = p2->xSize;
		tmp.ySize = p2->ySize;
		tmp.xPos = p2->xPos + mapXPixelSize;
		tmp.yPos = p2->yPos;

		stat = ObjInObjNW(&tmp,p1);
	}

	/* if no collision, then check if object p1 past y map boundary */

	if (stat == FALSE && (p1->yPos >= mapYPixelSize)) {
		tmp.xSize = p2->xSize;
		tmp.ySize = p2->ySize;
		tmp.xPos = p2->xPos;
		tmp.yPos = p2->yPos + mapYPixelSize;

		stat = ObjInObjNW(&tmp,p1);
	}

	/* if no collision, then check if object p2 past x and y map boundary */

	if (stat == FALSE && (p2XEnd >= mapXPixelSize) && (p2->yPos >= mapYPixelSize)) {
		tmp.xSize = p1->xSize;
		tmp.ySize = p1->ySize;
		tmp.xPos = p1->xPos + mapXPixelSize;
		tmp.yPos = p2->yPos + mapYPixelSize;

		stat = ObjInObjNW(p2,&tmp);
	}

	/* if no collision, then check if object p2 past x map boundary */

	if (stat == FALSE && (p2XEnd >= mapXPixelSize)) {
		tmp.xSize = p1->xSize;
		tmp.ySize = p1->ySize;
		tmp.xPos = p1->xPos + mapXPixelSize;
		tmp.yPos = p1->yPos;

		stat = ObjInObjNW(p2,&tmp);
	}

	/* if no collision, then check if object p2 past y map boundary */

	if (stat == FALSE && (p2->yPos >= mapYPixelSize)) {
		tmp.xSize = p1->xSize;
		tmp.ySize = p1->ySize;
		tmp.xPos = p1->xPos;
		tmp.yPos = p2->yPos + mapYPixelSize;

		stat = ObjInObjNW(p2,&tmp);
	}

	return stat;
}










/* --------------------------------------------------------------------------

Function:		PointInObj

Scope:			PUBLIC

Usage:			status = PointInObj(x, y, pObj)
				BOOL		status			- TRUE if point within object
				short		x, y			- point to check against
				Render		*pObj,			- pointer to object to check

Returns:		TRUE if object rectangles intercept, FALSE if not

Description:	A check is done to determine if the specified point is within
				the bounding rectangle of the specified object. This routine
				will also take into account a wrap-around universe by checking
				if the rectangle or point over-steps the right side of the
				map. If so, the rectangle will be split into two such that
				the starting location of the first half will be the same as
				the original rectangle but the width will be adjust to end
				at the map boundary. The second rectangle will start at x
				location 0 and the width will be adjust to reflect how far
				the original rectangle overstepped the map boundary. The
				splitting will only occur if a collision check of the
				original rectangle returned a FALSE.

Note:			MapManagerInit must have been called. xSize and ySize must
				best set to a valid value in the object structures.

-------------------------------------------------------------------------- */

PUBLIC BOOL PointInObj(short x, short y, Render *p)
{
	short	xEnd = p->xPos + (p->xSize - 1) / 2,
			xc = x & (mapXPixelSize - 1),
			yc = y & (mapYPixelSize - 1);

	BOOL	stat = PointInObjNW(xc, yc, p);		/* try as they are */

	/* if no collision, split rectangle */

	if ((stat == FALSE) && (xEnd >= mapXPixelSize) && (p->yPos >= mapYPixelSize))
		stat = PointInObjNW(xc + mapXPixelSize, yc + mapYPixelSize, p);

	if ((stat == FALSE) && (xEnd >= mapXPixelSize))
		stat = PointInObjNW(xc + mapXPixelSize, yc, p);

	if ((stat == FALSE) && (p->yPos >= mapYPixelSize))
		stat = PointInObjNW(xc, yc + mapYPixelSize, p);

	return stat;
}










/* --------------------------------------------------------------------------

Function:		ObjInObjNW

Scope:			PUBLIC

Usage:			status = ObjInObjNW(pObj1, pObj2)
				BOOL		status			- TRUE if objects intersect
				Render		*pObj1, pObj2	- pointers to objects to check

Returns:		TRUE if object rectangles intercept, FALSE if not

Description:	A check is done to determine if the bounding rectangles of
				the specified objects intersect at any point. This routine
				will NOT take into account a wrap-around universe.

-------------------------------------------------------------------------- */

PUBLIC BOOL ObjInObjNW(Render *p1, Render *p2)
{
	BOOL	stat = TRUE;

	short	p1x1 = p1->xPos - (p1->xSize - 1) / 2,
			p1x2 = p1->xPos + (p1->xSize - 1) / 2,
			p1y1 = p1->yPos - (p1->ySize - 1),
			p1y2 = p1->yPos;

	short	p2x1 = p2->xPos - (p2->xSize - 1) / 2,
			p2x2 = p2->xPos + (p2->xSize - 1) / 2,
			p2y1 = p2->yPos - (p2->ySize - 1),
			p2y2 = p2->yPos;

	if (p2x1 > p1x2)
		stat = FALSE;

	else if (p2x2 < p1x1)
		stat = FALSE;

	else if (p2y1 > p1y2)
		stat = FALSE;

	else if (p2y2 < p1y1)
		stat = FALSE;

	return stat;
}










/* --------------------------------------------------------------------------

Function:		PointInObjNW

Scope:			PUBLIC

Usage:			status = PointInObjNW(x, y, pObj)
				BOOL		status			- TRUE if point within object
				short		x, y			- point to check against
				Render		*pObj,			- pointer to object to check

Returns:		TRUE if object rectangles intercept, FALSE if not

Description:	A check is done to determine if the specified point is within
				the bounding rectangle of the specified object. This routine
				will NOT take into account a wrap-around universe.

-------------------------------------------------------------------------- */

PUBLIC BOOL PointInObjNW(short x, short y, Render *p)
{
	BOOL	stat = TRUE;

	short	px1 = p->xPos - (p->xSize - 1) / 2,
			px2 = p->xPos + (p->xSize - 1) / 2,
			py1 = p->yPos - (p->ySize - 1),
			py2 = p->yPos;

	if (px1 > x)
		stat = FALSE;

	else if (px2 < x)
		stat = FALSE;

	else if (py1 > y)
		stat = FALSE;

	else if (py2 < y)
		stat = FALSE;

	return stat;
}










/*
 * ComputeDistance -- Find direct distance between two objects
 */

PUBLIC short
ComputeDistance(Render *p1, Render *p2)
{
	int xDelta, yDelta;

	register long n;
	short r;

	xDelta = ComputeXDelta(p1,p2);
	yDelta = ComputeYDelta(p1,p2);

	xDelta = abs(xDelta);  yDelta = abs(yDelta);

	n = ( (xDelta * xDelta ) + (yDelta * yDelta ) );

	for ( r = 1 ; n> 0 ; r+= 2 , n -= r );

	return r>>1;
}















/*-----------------------------------------------------------------------*/
/*
 * Compute[X|Y]Delta -- Find shortest path from object 1 to object 2
 */
PUBLIC short
ComputeXDelta(Render *p1, Render *p2)
{
	short xDelta;

	xDelta = p2->xPos - p1->xPos;

	if ( xDelta > (mapXPixelSize/2) ) xDelta -= mapXPixelSize;
	if ( xDelta < -(mapXPixelSize/2) ) xDelta += mapXPixelSize;

	return xDelta;
}











PUBLIC short
ComputeYDelta(Render *p1, Render *p2)
{
	short yDelta = p2->yPos - p1->yPos;

	if ( yDelta > (mapYPixelSize/2) ) yDelta -= mapYPixelSize;
	if ( yDelta < -(mapYPixelSize/2) ) yDelta += mapYPixelSize;

	return yDelta;
}




/*-----------------------------------------------------------------------*/
/*
 * Compute[X|Y]PointDelta -- Find distance from point 1 to point 2
 */
PUBLIC short
ComputeXPointDelta(short xPos1, short xPos2)
{
	short xDelta;

	xDelta = xPos2 - xPos1;

	if ( xDelta > (mapXPixelSize/2) ) xDelta -= mapXPixelSize;
	if ( xDelta < -(mapXPixelSize/2) ) xDelta += mapXPixelSize;

	return xDelta;
}




PUBLIC short
ComputeYPointDelta(short yPos1, short yPos2)
{
	short yDelta = yPos2 - yPos1;

	if ( yDelta > (mapYPixelSize/2) ) yDelta -= mapYPixelSize;
	if ( yDelta < -(mapYPixelSize/2) ) yDelta += mapYPixelSize;

	return yDelta;
}




/*
 * ComputePointDistance -- Find direct distance between two points
 */
PUBLIC short
ComputePointDistance(short xPos1, short yPos1, short xPos2, short yPos2)
{
register	long	n;
			short	xDelta, yDelta;
			short	r;

	xDelta = ComputeXPointDelta(xPos1, xPos2);
	yDelta = ComputeYPointDelta(yPos1, yPos2);

	xDelta = abs(xDelta);  yDelta = abs(yDelta);

	n = ( (xDelta * xDelta ) + (yDelta * yDelta ) );

	for ( r = 1 ; n> 0 ; r+= 2 , n -= r );

	return r>>1;
}




/* --------------------------------------------------------------------------

Function:		ObjInRelativeRect

Scope:			PUBLIC

Usage:			status = ObjInRelativeRect(Render *p1, short left, short top,
											short right, short bottom,
											Render *p2)
					Render *p1	  - object collision rect is relative to
					short left    - deltaX to left side of collis rect
					short top     - deltaY to top of collis rect
					short right   - deltaX to right side of collis rect
					short bottom  - deltaY to bottom of collis rect
					Render *p2   - object to detect in rect or not

Returns:		TRUE if p2 is in rectangle specified relative to p1

Description:

-------------------------------------------------------------------------- */
PUBLIC BOOL
ObjInRelativeRect(	Render *p1,
						short left,
						short top,
						short right,
						short bottom,
						Render *p2)
{
	short yDelta;
	short xDelta = ComputeXDelta(p1,p2);

	if ( (xDelta > left) && (xDelta < right) )
	{
		yDelta = ComputeYDelta(p1,p2);

		if ( (yDelta >= top) && (yDelta <= bottom) )
			return TRUE;
	}

	return FALSE;
}



/* --------------------------------------------------------------------------

Function:		ComputeRectDistance

Scope:			PUBLIC

Usage:			dist = ComputeRectDistance(Render *p1, Render *p2)
					short   dist  - rectangular distance from p1 to p2
					Render *p1	  - object to find rect dist. from
					Render *p2	  - object to find rect dist. to

Returns:		returns longer side length of smallest rectangle containing
				both p1 and p2

Description:

-------------------------------------------------------------------------- */

PUBLIC short
ComputeRectDistance(Render *p1, Render *p2)
{
	short xDelta = ComputeXDelta(p1,p2);
	short yDelta = ComputeYDelta(p1,p2);

	xDelta = abs(xDelta);
	yDelta = abs(yDelta);

	return (xDelta > yDelta) ? xDelta : yDelta;
}



/*--------------------------------------------------------------------------*/
/*
 * Function:   Intersect CamNW
 * Scope:		Private
 * Author:		SLS
 * Purpose:		Check for object intersection with the camera discounting
 *				wrap.
 */

PUBLIC BOOL
RectInRectNW(TRect *Obj1Rect, TRect *Obj2Rect)
{
	BOOL	stat = TRUE;

	if (Obj2Rect->x1 > Obj1Rect->x2)
		stat = FALSE;

	else if (Obj2Rect->x2 < Obj1Rect->x1)
		stat = FALSE;

	else if (Obj2Rect->y1 > Obj1Rect->y2)
		stat = FALSE;

	else if (Obj2Rect->y2 < Obj1Rect->y1)
		stat = FALSE;

	return stat;
}

