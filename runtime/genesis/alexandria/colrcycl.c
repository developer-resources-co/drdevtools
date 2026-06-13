/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Title:			Class Code Support for ColorCycle

File:			COLRCYCL.C

See Also:		COLRCYCL.H (class definition for ColorCycle)

-------------------------------------------------------------------------- */

/* compiler dependant */

#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <assert.h>


/* module dependant */

#include "debug.h"
#include "funcdef.h"
#include "alexdef.h"
#include "scroll.h"
#include "mapmngr.h"
#include "genesis.h"
#include "text.h"
#include "vdp.h"
#include <spanim.h>
#include <list.h>

#include "colrcycl.h"





/* --------------------------------------------------------------------------

Function:		InitColorCycle

Scope:			PUBLIC

Usage:			pColorCycle = InitColorCycle(list, nStartColor, nColors, pPalette)
				ColorCycle	*pColorCycle	- pointer to instance of ColorCycle created
				UWORD		list;			- list where ColorCycle should live
				UWORD		nStartColor;	- starting color number
				UWORD		nColors;		- number of colors
				TPaletteData *pPalette;		- palette information

Returns:		pointer to instance if successful, NULL if not

Description:	Initialize a ColorCycle object and places it in the
				specified list.

-------------------------------------------------------------------------- */

PUBLIC ColorCycleRange *
InitColorCycle(UWORD nList, UWORD nStartColor, UWORD nColors, TPaletteData *pPalette)
{
    ColorCycleRange  *this = ColorCycleNew(nColors);
	short	stat;

	assert(this);

	if (this) {
		stat = ListAddObject((Object *) this, nList);

		assert(stat == LIST_NoError);

		if (stat == LIST_NoError) {
			this->nStartColor = nStartColor;

			if ( nColors < 0 )
			{
				this->nColors		= -nColors;
				this->nDirection	= -1;
			}
			else
			{
				this->nColors		= nColors;
				this->nDirection	= +1;
			}

			memcpy(	this->oColors,
                    &(pPalette->wColor[nStartColor]),
/*                     + (this->nStartColor * sizeof(TColor)),*/
                    (nColors * sizeof(TColor)) );
		}

		else {
			this->mDestruct((Object *) this);
			this = NULL;
		}
	}
	return this;
}

/* --------------------------------------------------------------------------

Function:		ColorCycleNew

Scope:			PUBLIC

Usage:			this = ColorCycleNew();
				ColorCycle	*this;			- pointer to instance of ColorCycle

Returns:		A pointer to ColorCycle passed to the routine

Description:	Allocates space on the heap for an instance of ColorCycle and
				sets all class variables and method pointers to default
				values which may be overridden.

-------------------------------------------------------------------------- */

PUBLIC ColorCycleRange *
ColorCycleNew(UWORD nColors)
{
    ColorCycleRange  *this = malloc(sizeof(ColorCycleRange) +
                                sizeof(TColor)*(nColors-1));

	assert(this);

	if (this) {
		ColorCycleConstruct(this);
		this->mDestruct = (Func_v) ColorCycleDestructHeap;			/* override default destructor */
	}

	return( this );
}





/* --------------------------------------------------------------------------

Function:		ConstructColorCycle

Scope:			PUBLIC

Usage:			thisderDup = ConstructColorCycle(this);
				ColorCycle	*thisDup;		- returned pointer to instance of ColorCycle
				ColorCycle	*this;			- pointer to instance of ColorCycle

Returns:		A pointer to ColorCycle passed to the routine

Description:	Sets all class variables and method pointers to default
				values which may be overridden

Note:			If and methods are overridden, make sure you have a full
				understanding of what the original methods did. Chances
				are, they did a lot of the code that you wanted to do
				anyway. If you need to do more, think about calling
				these standard methods within your own method code.

-------------------------------------------------------------------------- */

PUBLIC ColorCycleRange *
ColorCycleConstruct( ColorCycleRange* this )
{
	assert(this);

	ObjectConstruct((Object *) this);

	this->mTick = (Func_v) ColorCycleTick;
	this->mDestruct = (Func_v) ColorCycleDestruct;

	this->nDelay = 0;
	this->nCounter = 0;

	return( this );
}





/* --------------------------------------------------------------------------

Function:		ColorCycleDestruct

Scope:			PUBLIC

Usage:			ColorCycleDestruct(this);
				ColorCycle	*this;			- pointer to instance of ColorCycle

Returns:		Nothing

Description:	Removes this instance of ColorCycle by eliminating all pointers
				to it from all lists. It will also free any allocated instances
				of Anim objects.

Note:			This routine is not aware of whether this instance of ColorCycle
				was allocated from the heap or the stack. Removal from the
				head is done via the routine ColorCycleDestructHeap. This is
				the default destructor used is an instance of ColorCycle was
				created by NOT using the ColorCycleNew() function.

-------------------------------------------------------------------------- */

PUBLIC void
ColorCycleDestruct( ColorCycleRange* this )
{
	assert(this);

	ObjectDestruct((Object *) this);
}





/* --------------------------------------------------------------------------

Function:		ColorCycleDestructHeap

Scope:			PUBLIC

Usage:			ColorCycleDestructHeap(this);
				ColorCycle	*this;			- pointer to instance of ColorCycle

Returns:		Nothing

Description:	Removes this instance of ColorCycle by eliminating all pointers
				to it from all lists.

Note:			This routine is not aware of whether this instance of ColorCycle
				was allocated from the heap or the stack. Removal from the
				head is done via the routine ColorCycleDestructHeap. This is
				the default destructor used is an instance of ColorCycle was
				created by NOT using the ColorCycleNew() function.

-------------------------------------------------------------------------- */

PUBLIC void
ColorCycleDestructHeap( ColorCycleRange* this )
{
	assert(this);

    ColorCycleDestruct(this);

	if (this)
		free(this);
}





/* --------------------------------------------------------------------------

Function:		ColorCycleTick

Scope:			PUBLIC

Usage:			ColorCycleTick(this);
				ColorCycle	*this;			- pointer to instance of ColorCycle

Returns:		Nothing

Description:	Causes all animations to be rendered

-------------------------------------------------------------------------- */

PUBLIC void
ColorCycleTick(ColorCycleRange *this)
{
	assert(this);

	ObjectTick((Object *) this);			/* always call parent tick routine first */

	if ( this->nCounter > 0 )
		this->nCounter -= this->nDeltaWallClock;

	else {
		if ( this->nDirection < 0 ) {		/* Reverse */
			TColor firstColor;

			firstColor = this->oColors[0];
            memmove( this->oColors, &(this->oColors[1]), sizeof(TColor) * (this->nColors - 1) );
			this->oColors[ this->nColors-1 ] = firstColor;
		}

		else {								/* Forward */
			TColor lastColor;

			lastColor = this->oColors[ this->nColors - 1 ];
            memmove( &(this->oColors[1]), this->oColors, sizeof(TColor) * (this->nColors - 1) );
			this->oColors[ 0 ] = lastColor;
		}

		SetPalette( this->nStartColor, this->nColors, this->oColors );

		this->nCounter = this->nDelay;
	}
}
/* EOF */
