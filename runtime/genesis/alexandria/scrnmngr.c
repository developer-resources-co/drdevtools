/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Description:	The Screen Manager notifies objects when they have
				either moved on or off screen by calling their
				mOnScreen and mOffScreen methods.

File:			SCRNMNGR.C


-------------------------------------------------------------------------- */


/* compiler dependant */


#include <stdlib.h>
#include <assert.h>


/* module dependant */

#include "debug.h"
#include "gentypes.h"
#include "alexdef.h"
#include "camera.h"
#include "scrnmngr.h"
#include "list.h"
#include "render.h"

#define SetColorZero(xx) *(ULONG*)0xc00004=0xc0000000;*(UWORD*)0xc00000=xx;


/* --------------------------------------------------------------------------

Function:		InitScreenManager

Scope:			PUBLIC

Usage:			pScrnMngr = InitScreenManager(nScrnMngrList, nScanList)
				ScreenManager	*pScrnMngr		- returned pointer to instance
												  of screen manager object
				UWORD			nScrnMngrList	- list number for ScrnMngr object to live
				UWORD			nScanList		- list number for ScrnMngr to scan

Returns:		pointer to instace if successful, NULL if not

Description:	Creates an instance of the ScreenManager object and
				initialize it to scan the specified list.


-------------------------------------------------------------------------- */

PUBLIC ScreenManager *
InitScreenManager(UWORD nScrnMngrList, UWORD nScanList)
{
	ScreenManager	*this = ScreenManagerNew();
	short			stat;

	assert(this);

	if (this) {
		stat = ListAddObject((Object *) this, nScrnMngrList);

		assert(stat == LIST_NoError);

		if (stat == LIST_NoError)
			this->nListNumber = nScanList;

		else {
			this->mDestruct((Object *) this);
			this = NULL;
		}
	}

	return this;
}




/* --------------------------------------------------------------------------

Function:		ScreenManagerNew

Scope:			PUBLIC

Usage:			this = ScreenManagerNew();
				ScreenManager	*this;			- pointer to instance of ScreenManager

Returns:		A pointer to ScreenManager passed to the routine

Description:	Allocates space on the heap for an instance of ScreenManager and
				sets all class variables and method pointers to default
				values which may be overridden.

-------------------------------------------------------------------------- */

PUBLIC ScreenManager *
ScreenManagerNew(void)
{
	ScreenManager	*this = malloc(sizeof(ScreenManager));

	assert(this);

	if (this) {
		ScreenManagerConstruct(this);
		this->mDestruct = (Func_v) ScreenManagerDestructHeap;			/* override default destructor */
	}

	return( this );
}





/* --------------------------------------------------------------------------

Function:		ScreenManagerConstruct

Scope:			PUBLIC

Usage:			thisderDup = ScreenManagerConstruct(this);
				ScreenManager	*thisDup;		- returned pointer to instance of ScreenManager
				ScreenManager	*this;			- pointer to instance of ScreenManager

Returns:		A pointer to ScreenManager passed to the routine

Description:	Sets all class variables and method pointers to default
				values which may be overridden

Note:			If and methods are overridden, make sure you have a full
				understanding of what the original methods did. Chances
				are, they did a lot of the code that you wanted to do
				anyway. If you need to do more, think about calling
				these standard methods within your own method code.

-------------------------------------------------------------------------- */

PUBLIC ScreenManager *
ScreenManagerConstruct( ScreenManager* this )
{
	assert(this);

	ObjectConstruct((Object *) this);

	this->mTick = (Func_v) ScreenManagerTick;
	this->nListNumber = 0;
	
	return( this );
}





/* --------------------------------------------------------------------------

Function:		ScreenManagerDestruct

Scope:			PUBLIC

Usage:			ScreenManagerDestruct(this);
				ScreenManager	*this;			- pointer to instance of ScreenManager

Returns:		Nothing

Description:	Removes this instance of ScreenManager by eliminating all pointers
				to it from all lists.

Note:			This routine is not aware of whether this instance of ScreenManager
				was allocated from the heap or the stack. Removal from the
				head is done via the routine ScreenManagerDestructHeap. This is
				the default destructor used is an instance of ScreenManager was
				created by NOT using the ScreenManagerNew() function.

-------------------------------------------------------------------------- */

PUBLIC void
ScreenManagerDestruct( ScreenManager* this )
{
	assert(this);

	ObjectDestruct((Object *) this);
}





/* --------------------------------------------------------------------------

Function:		ScreenManagerDestructHeap

Scope:			PUBLIC

Usage:			ScreenManagerDestructHeap(this);
				ScreenManager	*this;			- pointer to instance of ScreenManager

Returns:		Nothing

Description:	Removes this instance of ScreenManager by eliminating all pointers
				to it from all lists.

Note:			This routine is not aware of whether this instance of ScreenManager
				was allocated from the heap or the stack. Removal from the
				head is done via the routine ScreenManagerDestructHeap. This is
				the default destructor used is an instance of ScreenManager was
				created by NOT using the ScreenManagerNew() function.

-------------------------------------------------------------------------- */

PUBLIC void
ScreenManagerDestructHeap( ScreenManager* this )
{
	assert(this);

	ScreenManagerDestruct(this);

	if (this)
		free(this);
}





/* --------------------------------------------------------------------------

Function:		ScreenManagerTick

Scope:			PUBLIC

Usage:			ScreenManagerTick(this);
				ScreenManager	*this;			- pointer to instance of ScreenManager

Returns:		Nothing

Description:	Causes the list specified in nListNumber to be traversed
				and each object will be checked to see whether it should be
				sent on mOnScreen message or mOffScreen.

Note:			It is up to the objects in the list to set the status of
				it's own fOnScreen bit.

-------------------------------------------------------------------------- */

PUBLIC void
ScreenManagerTick(ScreenManager *this)
{
	Render	*pObj;
	void	*ptr;
#ifdef SPEED
    UWORD   color=0x600;
#endif
    assert(this);

	ObjectTick((Object *) this);

	ptr = ListStart(this->nListNumber);

	while (ptr) {
        #ifdef SPEED
            SetColorZero(color); color+=4;
        #endif

		pObj = (Render *) ListObject(ptr);

		if (IntersectCamera(pSpriteCamera, pObj)) {

		  	/* if in field of view */

			if (!pObj->fOnScreen)
				pObj->mOnScreen((Object *) pObj);		/* send OnScreen message */
		}

		else {

			/* object not in field of view */

			if (pObj->fOnScreen)
				pObj->mOffScreen((Object *) pObj);		/* send OffScreen message */
		}

		ptr = ListNext(ptr);
	}
}


/* CSCRNMGR.C -- EOF */

