/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Title:		Class Definition for Brain

File:		BRAIN.H

See Also:	BRAIN.C (support routines for class Brain)

-------------------------------------------------------------------------- */

#ifndef _CBRAIN_H_
#define _CBRAIN_H_

#include <alexdef.h>
#include <cobject.h>
#include <funcdef.h>



/* structure definition for class Brain */


#define _CBRAIN \
	_CRENDER \
\
	/* <<<<<<<<<<  class variables  >>>>>>>>>>  */\
\
	UWORD		nBrainState;		/* current brain state */\
\
	/* <<<<<<<<<<  class methods  >>>>>>>>>> */\
\
	UWORD		(*mBrain)(struct _Brain *);		/* points to brain function */



/*===========================================================================*/

/* actual typedef structure definition */

typedef struct _Brain {

	_CBRAIN

} Brain;






/* ---------- class method prototype definitions  ---------- */

PUBLIC	Brain	*BrainNew( void );						/* create new instance of object Brain */

PUBLIC	Brain	*BrainConstruct( Brain *this );		/* assign default values to variables and */
														/* method pointers                        */
PUBLIC	void	BrainDestruct( Brain *this );			/* destroy this instance of Brain */

PUBLIC	void	BrainDestructHeap( Brain *this );		/* destroy this instance of Brain from heap */

PUBLIC	void	BrainTick( Brain *this );				/* default tick routine */




#endif




/* CBRAINS.H -- EOF */




