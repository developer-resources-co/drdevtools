/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Title:		Class Definition for ColorCycle

File:		CCOLORCYCLE.H

See Also:	CCOLORCYCLE.C (support routines for class CCOLORCYCLE)

-------------------------------------------------------------------------- */

#ifndef _CCOLORCYCLE_H_
#define _CCOLORCYCLE_H_

#include <alexdef.h>
#include <render.h>
#include <gentypes.h>





/* structure definition for class ColorCycle */


#define _CCOLORCYCLE \
	_COBJECT \
\
	/* <<<<<<<<<<<  class variables  >>>>>>>>>> */\
\
	short	nStartColor;\
	short	nColors;\
	short	nDirection;\
	short	nDelay;\
	short	nCounter;\
    TColor  oColors[1];



/*===========================================================================*/

/* actual typedef structure definition */

typedef struct _ColorCycle {

	_CCOLORCYCLE

} ColorCycleRange;






/* ---------- class method prototype definitions  ---------- */

PUBLIC  ColorCycleRange  *InitColorCycle(UWORD nList, UWORD nStartColor, UWORD nColors, TPaletteData *pPalette);

PUBLIC  ColorCycleRange  *ColorCycleNew(UWORD nColors);                       /* create new instance of object ColorCycle */

PUBLIC  ColorCycleRange  *ColorCycleConstruct( ColorCycleRange *this );       /* assign default values to variables and */
																	/* method pointers                        */

PUBLIC  void    ColorCycleDestruct( ColorCycleRange *this );         /* destroy this instance of ColorCycle */

PUBLIC  void    ColorCycleDestructHeap( ColorCycleRange *this );     /* destroy this instance of ColorCycle from heap */

PUBLIC  void    ColorCycleTick( ColorCycleRange *this );             /* default tick routine */



#endif




/* COLRCYCL.H -- EOF */

