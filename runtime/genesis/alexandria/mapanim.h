/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Title:		Class Definition for MapAnim

File:		MAPANIM.H

See Also:	MAPANIM.C (support routines for class CMAPANIM)

-------------------------------------------------------------------------- */

#ifndef _MAPANIM_H_
#define _MAPANIM_H_

#include "alexdef.h"
#include "funcdef.h"
#include "gentypes.h"
#include "anim.h"
#include "spanim.h"
#include "render.h"



/*============================================================================*/

/*
 * Overview:
 *	This function is used to draw animations over the playfield
 */


typedef struct _TMapAnimFrame
{
	TMapData	*map;
	void		*placeholder;
	WORD  dx, dy;
	WORD  delay;
} TMapAnimFrame;


typedef struct _TMapAnim
{
	TCharSetData  *pCharSet;
	TMapAnimFrame *frame;
	UBYTE   nNumFrames;
} TMapAnim;





/* structure definition for class MapAnim */


#define _CMAPANIM \
	_ANIM \
\
	/* <<<<<<<<<<<  class variables  >>>>>>>>>> */\
\
	TScrollField	*pScroll;			/* pointer to scroll structure */




/*===========================================================================*/

/* actual typedef structure definition */

typedef struct _MapAnim {

	_CMAPANIM

} MapAnim;





/* ---------- class method prototype definitions  ---------- */

PUBLIC	MapAnim	*MapAnimAdd(Anim *pAnimBase);		/* add another MapAnim instance to the chain */

PUBLIC	MapAnim	*MapAnimNew(void);					/* create new instance of object MapAnim */

PUBLIC	MapAnim	*MapAnimConstruct( MapAnim *this );	/* assign default values to variables and */
															/* method pointers                        */

PUBLIC	void	MapAnimTick( MapAnim *this );			/* default tick routine */

PUBLIC	void	MapAnimOnScreen( MapAnim *this );		/* default on screen routine */

PUBLIC	void	MapAnimOffScreen( MapAnim *this );	/* default off screen routine */

PUBLIC	void	MapAnimRenderMap( WORD xPosition, WORD yPosition, Anim* this );

PUBLIC	void	MapAnimComputeRectangle(Render *p, TRect *rrr);

#endif




/* MAPANIM.H -- EOF */

