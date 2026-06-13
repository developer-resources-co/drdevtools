
#ifndef _SURFMAP_H_
#define _SURFMAP_H_


/* --------------------------------------------------------------------------

			Surface Map Module ver 1.2
			--------------------------

	Written by:	Ray Ewan
				Alexandria, Inc.

	Date:		Jan 28, 1993



-------------------------------------------------------------------------- */



#include <gentypes.h>
#include <alexdef.h>


/*  ----------  defines  ----------  */


/* NOTE: only change the following defines if the size of a tile or meta tile changes */

#define	SURF_MT_WT_P2		5				/* meta tile width in tiles (product of 2) */
#define	SURF_MT_HT_P2		4				/* meta tile height in tiles (product of 2) */

#define	SURF_T_WP_P2		4				/* tile width in pixels (product of 2) */
#define	SURF_T_HP_P2		4				/* tile height in pixels (product of 2) */


/* DO NOT change these defines. Their values are entirely based on the above two defines */

#define	SURF_MT_WT			(1 << SURF_MT_WT_P2)			/* meta tile width in tiles */
#define	SURF_MT_HT			(1 << SURF_MT_HT_P2)			/* meta tile width in tiles */

#define	SURF_MT_SIZET_P2	(SURF_MT_WT_P2 + SURF_MT_HT_P2)	/* meta tile size in tiles (product of 2) */
#define	SURF_MT_SIZET		(1 << SURF_MT_SIZE_P2)			/* meta tile size in tiles */
#define	SURF_MT_SIZEB_P2	(SURF_MT_SIZET_P2 + 1)			/* meta tile map size in bytes (product of 2) */
#define	SURF_MT_SIZEB		(1 << SURF_MT_SIZEB_P2)			/* meta tile map size in bytes */

#define	SURF_TILE_WP		(1 << SURF_T_WP_P2)				/* tile width in pixels */
#define	SURF_TILE_HP		(1 << SURF_T_HP_P2)				/* tile height in pixels */

#define	SURF_COMPLETED	0
#define	SURF_INTERRUPT	1
#define	SURF_NOCONTOUR	2

#define	SURF_NOCROSS	65535








/*  ----------  type definitions  ----------  */


/* surface map structure */

typedef struct {
	const TMetaMap		*poMetaMap;			/* pointer to meta map  */
	const TMetaCharMap	*poMetaCharMap;		/* pointer to character map  */
	const TFloorTable	*poFloorTable;		/* pointer to floor table  */
	const TContourDef	*poContourDef,		/* pointer to contour tile definitions */
						*poCrossContourDef;	/* pointer to crossing contour definitions */
	UWORD				wArrayOffset,		/* array offset into floor table at last location */
						wContourOffset,		/* contour table offset at last location  */
						xSize, ySize;		/* x,y map size in pixels */
	short				xPosition, yPosition,/* coordinates of location pointer  */
						nDeltaX, nDeltaY;	/* location pointer deltas */
	UWORD				wContourID,			/* tile IDs */
						wSpecialID,
						wObjectID;
	UWORD				wCrossID,			/* object tile ID indicating crossing contours */
						wCrossTableOffset;
} TSurfMap;









/*  <<<<<<<<<<  function declarations  >>>>>>>>>>  */




extern	void MapInit(      TSurfMap		*poMapStruct,
					 const TMetaCharMap	*poMetaCharMap,
					 const TMetaMap		*poMetaMap,
					 const TFloorTable	*poFloorTable,
					 const TContourDef	*poContour,
					 const TContourDef	*poCrossContour,
					       UWORD		wCrossID);

extern	BOOL	MapAlign(TSurfMap	*poMapStruct,
						 short		xPosition,
						 short		yPosition		),

				ExtendedMapAlign(TSurfMap	*poMapStruct,
						 short		xPosition,
						 short		yPosition		);

extern	UWORD	MapMove(TSurfMap	*poMapStruct,
						short		nDeltaX		);
#endif

