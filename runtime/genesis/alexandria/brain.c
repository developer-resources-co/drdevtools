/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Description:	Class Code Support for Brain (root class)

File:			CBRAIN.C

See Also:		CBRAIN.H (class definition for Brain)

-------------------------------------------------------------------------- */

/* compiler dependant */

#include <stdlib.h>
#include <limits.h>
#include <assert.h>


/* module dependant */

#include <crender.h>
#include <funcdef.h>
#include <alexdef.h>

#include "cbrain.h"





/* --------------------------------------------------------------------------

Function:		BrainNew

Scope:			PUBLIC

Usage:			this = BrainNew();
				Brain	*this;			- pointer to instance of Brain

Returns:		A pointer to Brain passed to the routine

Description:	Allocates space on the heap for an instance of Brain and
				sets all class variables and method pointers to default
				values which may be overridden.

-------------------------------------------------------------------------- */

PUBLIC Brain *
BrainNew()
{
	Brain	*this = malloc(sizeof(Brain));

	assert(this);

	if (this) {
		BrainConstruct(this);
		this->mDestruct = (Func_v) BrainDestructHeap;			/* override default destructor */
	}

	return( this );
}





/* --------------------------------------------------------------------------

Function:		BrainConstruct

Scope:			PUBLIC

Usage:			pBrainDup = BrainConstruct(pCObj);
				Brain	*thisDup;		- returned pointer to instance of Brain
				Brain	*this;			- pointer to instance of Brain

Returns:		A pointer to Brain passed to the routine

Description:	Sets all class variables and method pointers to default
				values which may be overridden

Note:			If and methods are overridden, make sure you have a full
				understanding of what the original methods did. Chances
				are, they did a lot of the code that you wanted to do
				anyway. If you need to do more, think about calling
				these standard methods within your own method code.

-------------------------------------------------------------------------- */

PUBLIC Brain *
BrainConstruct( Brain* this )
{
	short	n;

	assert(this);

	RenderConstruct((Render *) this);

	this->mBrain = NULL;
	this->nBrainState = 0;
	this->mTick = (Func_v) BrainTick;
	this->mDestruct = (Func_v) BrainDestruct;
	
	return( this );
}





/* --------------------------------------------------------------------------

Function:		BrainDestruct

Scope:			PUBLIC

Usage:			BrainDestruct(pCObj);
				Brain	*pCObj;			- pointer to instance of Brain

Returns:		Nothing

Description:	Removes this instance of Brain by eliminating all pointers
				to it from all lists. It will also free any allocated instances
				of Anim objects.

Note:			This routine is not aware of whether this instance of Brain
				was allocated from the heap or the stack. Removal from the
				head is done via the routine BrainDestructHeap. This is
				the default destructor used is an instance of Brain was
				created by NOT using the BrainNew() function.

-------------------------------------------------------------------------- */

PUBLIC void
BrainDestruct( Brain* this )
{
	short	n;

	assert(this);

	RenderDestruct((Render *) this);
}





/* --------------------------------------------------------------------------

Function:		BrainDestructHeap

Scope:			PUBLIC

Usage:			BrainDestructHeap(pCObj);
				Brain	*pCObj;			- pointer to instance of Brain

Returns:		Nothing

Description:	Removes this instance of Brain by eliminating all pointers
				to it from all lists.

Note:			This routine is not aware of whether this instance of Brain
				was allocated from the heap or the stack. Removal from the
				head is done via the routine BrainDestructHeap. This is
				the default destructor used is an instance of Brain was
				created by NOT using the BrainNew() function.

-------------------------------------------------------------------------- */

PUBLIC void
BrainDestructHeap( Brain* this )
{
	assert(this);

	BrainDestruct(this);

	if (this)
		free(this);
}





/* --------------------------------------------------------------------------

Function:		BrainRoutine

Scope:			PUBLIC

Usage:			BrainRoutine(pCObj);
				Brain	*pCObj;			- pointer to instance of Brain

Returns:		Nothing

Description:	Brain routine will be called if it exists

-------------------------------------------------------------------------- */

PUBLIC void
BrainTick(Brain *this)
{
	short	n;

	assert(this);

	RenderTick((Render *) this);

	if (this->mBrain)
		this->nBrainState = this->mBrain(this);
}




/* CBRAIN.C -- EOF */



