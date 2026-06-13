/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Description:	Screen Manager Class Definition

File:			SCRNMNGR.H

See Also:		SCRNMNGR.C (class code for ScreenManager)

-------------------------------------------------------------------------- */


#ifndef _CSCRNMGR_H_
#define	_CSCRNMGR_H_


/* module dependant */

#include <alexdef.h>
#include <object.h>
#include <camera.h>





/* structure definition for class Render */


#define _CSCREENMANAGER \
	_COBJECT \
\
	/* <<<<<<<<<<<  class variables  >>>>>>>>>> */\
\
	UWORD	nListNumber;					/* list number to traverse */



/*===========================================================================*/

/* actual typedef structure definition */

typedef struct _ScreenManager {

	_CSCREENMANAGER

} ScreenManager;





/* ---------- class method prototype definitions  ---------- */

PUBLIC	ScreenManager	*InitScreenManager(UWORD nScrnMngrList, UWORD nScanList);

PUBLIC	ScreenManager	*ScreenManagerNew(void);

PUBLIC	ScreenManager	*ScreenManagerConstruct( ScreenManager *this );

PUBLIC	void	ScreenManagerDestruct( ScreenManager *this );

PUBLIC	void	ScreenManagerDestructHeap( ScreenManager *this );

PUBLIC	void	ScreenManagerTick( ScreenManager *this );



#endif


/* CSCRNMGR.H -- EOF */

