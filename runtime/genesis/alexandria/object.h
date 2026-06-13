/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Title:		Class Definition for Object (root class)

File:		OBJECT.H

See Also:	OBJECT.C (support routines for class Object)


Craig 6/26/93 : added typedef for TMethod for easy casting

-------------------------------------------------------------------------- */

#ifndef _COBJECT_H_
#define _COBJECT_H_

#include <alexdef.h>




/* structure definition for class Object */


#define _COBJECT \
\
/* <<<<<<<<<<<  class variables  >>>>>>>>>> */\
\
	struct						/* bit field definitions - describes when bit is TRUE */\
	{\
		UWORD	_fActive			: 1;	/* object's mTickRoutine() method can be called   */\
											/* by UpdateObjects() (an object manager funtion) */\
\
		UWORD	_fPermanent			: 1;	/* object should exist throughout entire    */\
											/* game and not be destroyed between levels */\
\
		UWORD	_fDelete			: 1;	/* indicates this object wants to be deleted by    */\
											/* having its mDestruct method called - usually by */\
											/* UpdateObjects().                                */\
\
		UWORD	_fVisible	 		: 1;	/* object contains displayable animations */\
\
		UWORD	_fOnScreen			: 1;	/* object is currently on screen */\
\
		UWORD	_fMusic				: 1;	/* music/sound effect currently playing */\
	} wObjectFlags;\
\
	UWORD	nTickDelay;			/* gets decremented by UpdateObjects() each tick until reaches 0. */\
		 						/* A non-zero value prevents mTickRoutine from being called       */\
\
	ULONG	nObjectType;		/* set to a game specific value indicating both what the */\
								/* object is specifically and what general type it is    */\
\
	ULONG	nPrevWallClock;		/* contains the previous wall clock value when mTick() was called */\
\
	UWORD	nDeltaWallClock;	/* contains the number of ticks passed from previous call to mTick() */\
\
\
/* <<<<<<<<<<  class methods  >>>>>>>>>> */\
\
/* Note: class constructors will initially set these method pointers either      */\
/*       to standard routines or to NULL. The user can override any of these     */\
/*       methods in their own constructor routine. However, check to see whether */\
/*       the original methods still need to be called. For example, almost       */\
/*       certainly, you will want to write your own mTick() method. However,     */\
/*       in most cases, you will still want to call the routine originally       */\
/*       pointed to by mTick() since a lot of standard housekeeping code is      */\
/*       already written for you there.                                          */\
\
	void	(*mTick)(struct _Object *);		/* called every tick by UpdateObjects() routine */\
	void	(*mDestruct)(struct _Object *);	/* destroys this instance of Object */\
	void	(*mActivate)(struct _Object *);	/* enables object to receive mTick() messages */\
	void	(*mDeactivate)(struct _Object *);	/* disables object from receiving mTick() messages */\
\
	void	(*mOnScreen)(struct _Object *);	/* gets called by notification manager to let the  */\
												/* object know that is has moved on screen. The    */\
												/* actual conditions are fOnScreen must be FALSE,  */\
												/* fNotifyOnScreen is TRUE, and the object is      */\
												/* within the bounding rect of the camera object.  */\
\
	void	(*mOffScreen)(struct _Object *);	/* gets called by notification manager to let the  */\
												/* object know that is has moved off screen. The   */\
												/* actual conditions are fOnScreen must be TRUE,   */\
												/* fNotifyOffScreen is TRUE, and the object is not */\
												/* within the bounding rect of the camera object.  */

/*===========================================================================*/

/* actual typedef structure definition */

typedef struct _Object {

	_COBJECT

} Object;


/*===========================================================================*/

/* #defines used to eliminate the need to prefix */
/* all references to bit fields with wObjectFlags.  */

#define fActive				wObjectFlags._fActive
#define fPermanent			wObjectFlags._fPermanent
#define	fDelete				wObjectFlags._fDelete
#define fVisible			wObjectFlags._fVisible
#define fOnScreen			wObjectFlags._fOnScreen
#define	fMusic				wObjectFlags._fMusic

typedef void (*TMethod) (struct _Object*);



/* ---------- class method prototype definitions  ---------- */

PUBLIC	Object	*ObjectConstruct( Object *this );		/* assign default values to variables and */
														/* method pointers                        */

PUBLIC	void	ObjectDestruct( Object *this );		/* destroy this instance of Object */

PUBLIC	void	ObjectTick( Object *this );			/* default tick routine */

PUBLIC	void	ObjectActivate( Object *this ),		/* standard object activate and */
				ObjectDeactivate( Object *this );		/* deactivate routines          */

PUBLIC	void	ObjectOnScreen( Object *this ),		/* standard object moved on screen and */
				ObjectOffScreen( Object *this );		/* off screen routines                 */

PUBLIC	void	DoNothing(Object *this);				/* does nothing */


#endif




/* COBJECT.H -- EOF */

