/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Description:	Class Code Support for Object (root class)

File:			OBJECT.C

See Also:		OBJECT.H (class definition for Object)

-------------------------------------------------------------------------- */

/* compiler dependant */

#include <stdlib.h>
#include <limits.h>
#include <assert.h>


/* module dependant */

#include "debug.h"
#include "funcdef.h"
#include "timing.h"
#include "alexdef.h"
#include "list.h"

#include "object.h"





/* -----  constructor data for class Object  ----- */

static const Object defaultObject =
	{
		{				/*	struct {                         */
			TRUE,		/*			UWORD _fActive		: 1; */
			FALSE,		/*			UWORD _fPermanent	: 1; */
			FALSE,		/*			UWORD _fDelete		: 1; */
			FALSE,		/*			UWORD _fVisible		: 1; */
			FALSE,		/*			UWORD _fOnScreen	: 1; */
			FALSE		/*			UWORD _fMusic		: 1; */
		},				/*	} wObjectFlags;                                	 */

	0U,					/*	UWORD	nTickDelay;			*/
	0UL,				/*	ULONG	nObjectType;		*/
	0UL,				/*	ULONG	nPrevWallClock;		*/
	0U,					/*	UWORD	nDeltaWallClock;	*/

	ObjectTick,		/*	void	(*mTick)(struct _Object *);		*/
	ObjectDestruct,	/*	void	(*mDestruct)(struct _Object *);	*/
	ObjectActivate,	/*	void	(*mActivate)(struct _Object *);	*/
	ObjectDeactivate,	/*	void	(*mDeactivate)(struct _Object *);	*/
	ObjectOnScreen,	/*	void	(*mOnScreen)(struct _Object *);	*/
	ObjectOffScreen	/*	void	(*mOffScreen)(struct _Object *);	*/
};





/* --------------------------------------------------------------------------

Function:		ObjectConstruct

Scope:			PUBLIC

Usage:			thisDup = ObjectConstruct(this);
				Object	*thisDup;		- returned pointer to instance of Object
				Object	*this;			- pointer to instance of Object

Returns:		A pointer to Object passed to the routine

Description:	Sets all class variables and method pointers to default
				values which may be overridden

Note:			If and methods are overridden, make sure you have a full
				understanding of what the original methods did. Chances
				are, they did a lot of the code that you wanted to do
				anyway. If you need to do more, think about calling
				these standard methods within your own method code. This
				is especially true for ObjectTickRoutine.

-------------------------------------------------------------------------- */

Object *
ObjectConstruct( Object* this )
{
	assert(this);

	*this = defaultObject;
	this->nPrevWallClock = nWallClock;
	return( this );
}





/* --------------------------------------------------------------------------

Function:		ObjectDestruct

Scope:			PUBLIC

Usage:			ObjectDestruct(this);
				Object	*this;			- pointer to instance of Object

Returns:		Nothing

Description:	Removes this instance of Object by eliminating all pointers
				to it from all lists.

Note:			This routine is not aware of whether this instance of Object
				was allocated from the heap or the stack. Removal from the
				head is done via the routine DestructObjectHeap. This is
				the default destuctor used is an instance of Object was
				created by NOT using the NewObject() function.

-------------------------------------------------------------------------- */

void
ObjectDestruct( Object* this )
{
	assert(this);

	this->mOffScreen(this);
	this->mDeactivate(this);

	ListRemoveAllLists(this);		/* remove reference to this object from all lists */
}





/* --------------------------------------------------------------------------

Function:		ObjectTick

Scope:			PUBLIC

Usage:			ObjectTick(this);
				Object	*this;			- pointer to instance of Object

Returns:		Nothing

Description:	Calculates how many ticks have gone by since the last
				execution of ObjectTickRoutine

Note:			Almost all timing for objects and brains will rely on
				this routine being executed. If mTick() method is redirected
				to a custom tick routine, it is strongly suggest that this
				routine still be called from within the custom routine.

-------------------------------------------------------------------------- */

void
ObjectTick(Object *this)
{
	long	time = nWallClock;

	assert(this);

	this->nDeltaWallClock = time - this->nPrevWallClock;
	this->nPrevWallClock = time;
}





/* --------------------------------------------------------------------------

Function:		ObjectActivate

Scope:			PUBLIC

Usage:			ObjectActivate(this);
				Object	*this;			- pointer to instance of Object

Returns:		Nothing

Description:	Sets the fActive bit to TRUE indicating that the tick
				routine is to be called by the Object Manager.

-------------------------------------------------------------------------- */

void
ObjectActivate( Object* this )
{
	assert(this);

	this->fActive = TRUE;
}





/* --------------------------------------------------------------------------

Function:		ObjectDeactivate

Scope:			PUBLIC

Usage:			ObjectDeactivate(this);
				Object	*this;			- pointer to instance of Object

Returns:		Nothing

Description:	Sets the fActive bit to FALSE indicating that the tick
				routine is not to be called by the Object Manager.

-------------------------------------------------------------------------- */

void
ObjectDeactivate( Object* this )
{
	assert(this);

	this->fActive = FALSE;
}




/* --------------------------------------------------------------------------

Function:		ObjectOnScreen

Scope:			PUBLIC

Usage:			ObjectOnScreen(this);
				Object	*this;			- pointer to instance of Object

Returns:		Nothing

Description:	Flags that the object is now on-screen and then calls
				the object activate routine to allow the tick routine
				to be called.  This	routine is normally called by the
				Screen Manager.


-------------------------------------------------------------------------- */

void
ObjectOnScreen( Object* this )
{
	assert(this);

	this->fOnScreen = TRUE;
	this->mActivate(this);
}





/* --------------------------------------------------------------------------

Function:		ObjectOffScreen

Scope:			PUBLIC

Usage:			ObjectOffScreen(this);
				Object	*this;			- pointer to instance of Object

Returns:		Nothing

Description:	Flags that the object is now off-screen and then calls
				the object deactivate routine to disable the tick routine
				to be called.  This	routine is normally called by the
				Screen Manager.

-------------------------------------------------------------------------- */

void
ObjectOffScreen( Object* this )
{
	assert(this);

	this->fOnScreen = FALSE;
	this->mDeactivate(this);
}



void
DoNothing(Object *this)
{
}



/* OBJECT.C -- EOF */

