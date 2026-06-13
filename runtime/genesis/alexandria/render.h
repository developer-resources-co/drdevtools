/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Title:		Class Definition for Render

File:		RENDER.H

See Also:	RENDER.C (support routines for class CRENDER)

-------------------------------------------------------------------------- */

#ifndef _RENDER_H_
#define _RENDER_H_

#include <alexdef.h>
#include <object.h>
#include <anim.h>


/* structure definition for class Render */


#define _CRENDER \
	_COBJECT \
\
	/* <<<<<<<<<<<  class variables  >>>>>>>>>> */\
\
	TFixedPoint xLoc, yLoc;\
\
	WORD	xSize, ySize;					/* size of bounding rectangle */\
\
	Anim	*pAnim;							/* pointer to instance on Anim object */\
\
	/* <<<<<<<<<<  class methods  >>>>>>>>>> */\
\
	void	(*mRender)(struct _Render *);	/* animation rendering method */



/*===========================================================================*/

/* defines to access position, both in fixed and integer format */
#define xPos		xLoc._FixPos._Int
#define xLongPos	xLoc._Whole
#define xInt		xLoc._FixPos._Int
#define xFrac		xLoc._FixPos._Frac

#define yPos		yLoc._FixPos._Int
#define yLongPos	yLoc._Whole
#define yInt		yLoc._FixPos._Int
#define yFrac		yLoc._FixPos._Frac

/* actual typedef structure definition */

typedef struct _Render {

	_CRENDER

} Render;






/* ---------- class method prototype definitions  ---------- */

PUBLIC	Render	*RenderNew(void);						/* create new instance of object Render */

PUBLIC	Render	*RenderConstruct( Render *this );		/* assign default values to variables and */
														/* method pointers                        */

Render	*RenderConstructMapAnim (Render *this);

PUBLIC  void	RenderDestruct( Render *this );		/* destroy this instance of Render */

PUBLIC	void	RenderDestructHeap( Render *this );	/* destroy this instance of Render from heap */

PUBLIC	void	RenderTick( Render *this );			/* default tick routine */

PUBLIC	void	RenderRoutine( Render *this );		/* default animation rendering routine */

PUBLIC	void	RenderOnScreen( Render *this );		/* default on screen routine */

PUBLIC	void	RenderOffScreen( Render *this );		/* default off screen routine */

PUBLIC	void	RenderList(UWORD nList);				/* traverse list and render displayable sprites */


#endif




/* CRENDER.H -- EOF */

