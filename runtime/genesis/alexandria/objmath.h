/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Title:		Header File for Object Math Routines

File:		OBJMATH.H

See Also:	OBJMATH.C

-------------------------------------------------------------------------- */


#ifndef _OBJMATH_H_
#define _OBJMATH_H_


#include <gentypes.h>
#include <alexdef.h>
#include <render.h>



	BOOL	ObjInObj(Render *, Render *);

	BOOL	PointInObj(short, short, Render *);

	BOOL	ObjInObjNW(Render *, Render *);

	BOOL	PointInObjNW(short, short, Render *);

	short 	ComputeDistance(Render *p1, Render *p2);

	short 	ComputeXDelta(Render *p1, Render *p2);

	short 	ComputeYDelta(Render *p1, Render *p2);

	BOOL   ObjInRelativeRect(	Render *p1,
								short left,
								short top,
								short right,
								short bottom,
								Render *p2);

	short   ComputeRectDistance(Render *p1, Render *p2);

	short	ComputeXPointDelta(short xPos1, short xPos2);

	short   ComputeYPointDelta(short yPos1, short yPos2);

	short   ComputePointDistance(   short xPos1,
									short yPos1,
									short xPos2,
									short yPos2);

	BOOL	RectInRectNW(TRect *Obj1Rect, TRect *Obj2Rect);


#endif



/* OBJMATH.H -- EOF */

