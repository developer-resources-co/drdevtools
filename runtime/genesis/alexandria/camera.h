/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Title:		Class Definition for Camera

File:		CAMERA.H

See Also:	CAMERA.C (support routines for class CCAMERA)

-------------------------------------------------------------------------- */

#ifndef _CCAMERA_H_
#define _CCAMERA_H_

#include <alexdef.h>
#include <render.h>


/* camera constants */

#define	Camera_xPixelSize		320
#define	Camera_yPixelSize		224
#define	Camera_xOffset			(Camera_xPixelSize / 2)
#define	Camera_yOffset			(Camera_yPixelSize / 2)
#define	Camera_xScale			0x10000
#define	Camera_yScale			0x10000





/* structure definition for class Camera */


#define _CCAMERA \
	_CRENDER \
\
	/* <<<<<<<<<<<  class variables  >>>>>>>>>> */\
\
	Render			*pTrackObj;					/* object that the camera will track */\
\
	ULONG			xScale, yScale;				/* speed controls */\
\
	WORD			camXOffset,	camYOffset;		/* offsets of main object from  */\
												/* upper left corner of screen  */\
\
	TScrollField 	*pScroll;					/* scroll structure */



/*===========================================================================*/

/* actual typedef structure definition */

typedef struct _Camera {

	_CCAMERA

} Camera;






/* ---------- class method prototype definitions  ---------- */

PUBLIC	Camera	*InitCamera(short, Render *, TScrollField *);	/* return pointer to initialized camera */

PUBLIC	Camera	*CameraNew(void);						/* create new instance of object Camera */

PUBLIC	Camera	*CameraConstruct( Camera *this );		/* assign default values to variables and */
														/* method pointers                        */

PUBLIC	void	CameraDestruct( Camera *this );		/* destroy this instance of Camera */

PUBLIC	void	CameraDestructHeap( Camera *this );	/* destroy this instance of Camera from heap */

PUBLIC	void	CameraTick( Camera *this );			/* default tick routine */

PUBLIC	BOOL	IntersectCamera(Camera *pCamera, 		/* determines if object is within field of view */
										Render *pObj);

#endif




/* COBJECT.H -- EOF */

