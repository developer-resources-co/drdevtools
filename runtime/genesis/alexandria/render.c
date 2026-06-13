/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Description:	Class Code Support for Render (root class)

File:			RENDER.C

See Also:		RENDER.H (class definition for Render)

-------------------------------------------------------------------------- */

/* compiler dependant */

#include <stdlib.h>
#include <limits.h>
#include <assert.h>


/* module dependant */

#include "debug.h"
#include "funcdef.h"
#include "alexdef.h"
#include "list.h"

#include "render.h"
#include "mapanim.h"



/* --------------------------------------------------------------------------

Function:		RenderNew

Scope:			PUBLIC

Usage:			this = RenderNew();
				Render	*this;			- pointer to instance of Render

Returns:		A pointer to Render passed to the routine

Description:	Allocates space on the heap for an instance of Render and
				sets all class variables and method pointers to default
				values which may be overridden.

-------------------------------------------------------------------------- */

PUBLIC Render *
RenderNew()
{
	Render	*this = malloc(sizeof(Render));

	assert(this);

	if (this) {
		RenderConstruct(this);
		this->mDestruct = (Func_v) RenderDestructHeap;			/* override default destructor */
	}

	return( this );
}





/* --------------------------------------------------------------------------

Function:		ConstructRender

Scope:			PUBLIC

Usage:			thisderDup = ConstructRender(this);
				Render	*thisDup;		- returned pointer to instance of Render
				Render	*this;			- pointer to instance of Render

Returns:		A pointer to Render passed to the routine

Description:	Sets all class variables and method pointers to default
				values which may be overridden

Note:			If and methods are overridden, make sure you have a full
				understanding of what the original methods did. Chances
				are, they did a lot of the code that you wanted to do
				anyway. If you need to do more, think about calling
				these standard methods within your own method code.

-------------------------------------------------------------------------- */

PUBLIC Render *
RenderConstruct( Render* this )
{
	assert(this);

	ObjectConstruct((Object *) this);

#ifndef xLongPos
	this->xPos = 0;
	this->yPos = 0;
#else
	this->xLongPos =  0;
	this->yLongPos =  0;
#endif
	this->xSize = 0;
	this->ySize = 0;
	this->mTick = (Func_v) RenderTick;
	this->mRender = RenderRoutine;
	this->mDestruct = (Func_v) RenderDestruct;
	this->fVisible = TRUE;
	this->pAnim = AnimNew();					/* create an instance of Anim */
	this->mOffScreen = (Func_v) RenderOffScreen;
	this->mOnScreen  = (Func_v) RenderOnScreen;
	assert(this->pAnim);

	return( this );
}



/*==========================================================================*/
Render *
RenderConstructMapAnim (Render* this)
{
	assert(this);

	ObjectConstruct((Object *) this);

#ifndef xLongPos
	this->xPos = 0;
	this->yPos = 0;
#else
	this->xLongPos =  0;
	this->yLongPos =  0;
#endif
	this->xSize = 0;
	this->ySize = 0;
	this->mTick = (Func_v) RenderTick;
	this->mRender = RenderRoutine;
	this->mDestruct = (Func_v) RenderDestruct;
	this->fVisible = TRUE;
	this->pAnim = MapAnimNew();
	this->mOffScreen = (Func_v) RenderOffScreen;
	this->mOnScreen  = (Func_v) RenderOnScreen;
	assert(this->pAnim);

	return( this );
}





/* --------------------------------------------------------------------------

Function:		RenderDestruct

Scope:			PUBLIC

Usage:			RenderDestruct(this);
				Render	*this;			- pointer to instance of Render

Returns:		Nothing

Description:	Removes this instance of Render by eliminating all pointers
				to it from all lists. It will also free any allocated instances
				of Anim objects.

Note:			This routine is not aware of whether this instance of Render
				was allocated from the heap or the stack. Removal from the
				head is done via the routine RenderDestructHeap. This is
				the default destructor used is an instance of Render was
				created by NOT using the RenderNew() function.

-------------------------------------------------------------------------- */

PUBLIC void
RenderDestruct( Render* this )
{
	assert(this);

	/* call Anim object's own Destruct method */

	AnimDestruct(this->pAnim);

	ObjectDestruct((Object *) this);
}





/* --------------------------------------------------------------------------

Function:		RenderDestructHeap

Scope:			PUBLIC

Usage:			RenderDestructHeap(this);
				Render	*this;			- pointer to instance of Render

Returns:		Nothing

Description:	Removes this instance of Render by eliminating all pointers
				to it from all lists.

Note:			This routine is not aware of whether this instance of Render
				was allocated from the heap or the stack. Removal from the
				head is done via the routine RenderDestructHeap. This is
				the default destructor used is an instance of Render was
				created by NOT using the RenderNew() function.

-------------------------------------------------------------------------- */

PUBLIC void
RenderDestructHeap( Render* this )
{
	assert(this);

	RenderDestruct(this);

	if (this)
		free(this);
}





/* --------------------------------------------------------------------------

Function:		RenderTick

Scope:			PUBLIC

Usage:			RenderTick(this);
				Render	*this;			- pointer to instance of Render

Returns:		Nothing

Description:	Causes all animations to be rendered

-------------------------------------------------------------------------- */

PUBLIC void
RenderTick(Render *this)
{
	assert(this);

	ObjectTick((Object *) this);
}




/* --------------------------------------------------------------------------

Function:		RenderRoutine

Scope:			PUBLIC

Usage:			RenderRoutine(this);
				Render	*this;			- pointer to instance of Render

Returns:		Nothing

Description:	Causes all animations to be rendered

-------------------------------------------------------------------------- */

PUBLIC void
RenderRoutine(Render *this)
{
	assert(this);

	/* call tick routine of Anim object to render sprite */

	if (this->fOnScreen)
		this->pAnim->mRenderSprite(this->xPos, this->yPos, this->pAnim);
}



/* --------------------------------------------------------------------------

Function:		RenderOnScreen

Scope:			PUBLIC

Usage:			RenderOnScreen(this);
				Render	*this;			- pointer to instance of Render

Returns:		Nothing

Description:	Performs the required default operation when an object is
				told that it is now on screen

-------------------------------------------------------------------------- */

PUBLIC void
RenderOnScreen( Render* this )
{
	assert(this);

	ObjectOnScreen((Object *) this);

	AnimOnScreen(this->pAnim);
}





/* --------------------------------------------------------------------------

Function:		RenderOffScreen

Scope:			PUBLIC

Usage:			RenderOffScreen(this);
				Render	*this;			- pointer to instance of Render

Returns:		Nothing

Description:	Performs the required default operation when an object is
				told that is it now off screen

-------------------------------------------------------------------------- */

PUBLIC void
RenderOffScreen( Render* this )
{
	assert(this);

	ObjectOffScreen((Object *) this);

	AnimOffScreen(this->pAnim);
}




/* --------------------------------------------------------------------------

Function:		RenderList

Scope:			PUBLIC

Usage:			RenderList(nList);
				UWORD	nList;			- list number to traverse

Returns:		Nothing

Description:	The specified list will be traversed and if the criterial
				is met, the sprite will be rendered.


-------------------------------------------------------------------------- */

#define SetColorZero(xx) *(ULONG*)0xc00004=0xc0000000;*(UWORD*)0xc00000=xx;

PUBLIC void
RenderList(UWORD nList)
{
	void	*ptr;
	Render	*pRender;
#ifdef SPEED
    UWORD color = 0x00e;
#endif

	ptr = ListStart(nList);

	while (ptr) {
        #ifdef SPEED
            SetColorZero(color);color+=0x40;
        #endif
		pRender = (Render *) ListObject(ptr);

		assert(pRender);

        if (IntersectCamera(pSpriteCamera, pRender)) {

		  	/* if in field of view */

            pRender->fOnScreen = TRUE;
#if 0
            if (!pRender->fOnScreen)
                pRender->mOnScreen((Object *) pRender);       /* send OnScreen message */
#endif

            if (pRender->fActive && pRender->fVisible && pRender->mRender)
                pRender->mRender(pRender);
		}

		else {

			/* object not in field of view */

            pRender->fOnScreen = FALSE;
#if 0
            if (pRender->fOnScreen)
                pRender->mOffScreen((Object *) pRender);      /* send OffScreen message */
#endif
		}

        AnimTick(pRender->pAnim);

		ptr = ListNext(ptr);
	}
}

/* CRENDER.C -- EOF */


