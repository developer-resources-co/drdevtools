/* 						Alexandria Genesis Library
 *
 * Title:		Zone Activator
 * File:		ZACTIVAT.H
 * See Also:	ZACTIVAT.C
 *
 * Created:		21 July 1993
 * Blame:		Dave Gross
 *
 */

#ifndef _ZACTIVAT_H_
#define _ZACTIVAT_H_

#include <object.h>
#include "tags.h"
#include <render.h>

/***************************************************************************/

#define _ZACTIVATOR \
   _COBJECT \
   const unsigned short  *pnMetaMapIndex;                      \
                  int     nTagListSize;                        \
   const          tagID  *pnTagList;                           \
                  char   *pnTagStatus;                         \
                  Render *pFocus;                              \
		          int     nFocalXOffset;                       \
		          int     nFocalYOffset;                       \
		 unsigned int     nXExtent;                            \
         unsigned int     nYExtent;                            \
         unsigned int     nLastFocX;                           \
         unsigned int     nLastFocY;                           \
         unsigned int     nGridXSize;                          \
         unsigned int     nGridYSize;                          \
         unsigned int     nGridX;                              \
         unsigned int     nGridY;                              \
         unsigned int     nLevelXSize;                         \
         unsigned int     nLevelYSize;                         \
  void  (*mTagObjectCreate) (struct _zactivator *, tagID, int); \
  void  (*mReset)  (int, struct _zactivator *); \
  char  (*mInZone) (unsigned int, unsigned int, struct _zactivator *);

/***************************************************************************
 * pnMetaMapIndex = An array of indices into pnTagList, points to a constant
 *					integer array.
 * nTagListSize   = The length of pnTagList and pnTagStatus
 * pnTagList	  = An array of tag ID information, points to a constant tag
 *					ID array.
 * pnTagStatus	  = An array ot tag status flags, points to a level-specific
 *					array that may be modified by other ZACTIVATOR objects
 * pFocus		  = A pointer to the focal object (often the camera or the
 *				    protagonist).
 * nFocalXOffset  = How much to offset the pFocus.xPos in the x direction to
 *                  find the true center of the pFocus object.
 * nFocalYOffset  = How much to offset the pFocus.yPos in the y direction to
 *                  find the true center of the pFocus object.
 * nXExtent		  = x-extent (length) of the ZOC
 * nYExtent		  = y-extent (height) of the ZOC
 * nLastFocX	  = previous focal X position
 * nLastFocY	  = previous focal Y position
 * nGridXSize	  = X-length of each grid box in pixels
 * nGridYSize     = Y-length of each grid box in pixels
 * nGridX		  = number of grid blocks along x-axis
 * nGridY		  = number of grid blocks along y-axis
 * nLevelXSize    = X-length of the level in pixels
 * nLevelYSize    = Y-length of the level in pixels
 * mReset		  = a function to reset the TagStatus of a tag (see below)
 * mTagObjectCreate = a function to create a tagged object
 * mInZone        = a function to determine whether a point is in the ZOC
 ***************************************************************************
 *		void Activator->mReset(int, ZACTIVATOR *)
 *
 * Purpose:  When an object which has been activated by an activator wants
 *           to allow other objects to be spawned from its tag, it must
 *           override the activator's default behavior of setting the
 *           TagStatus for that tag to 1 (which signals that the activator
 *           should ignore that tag).
 *
 *           An object which has been activated by an activator is passed a
 *           pointer to it's activator, and also an index into that
 *           activator's TagStatus array.  By calling it's activator's
 *           mReset function with these two values, it forces a reset of the
 *           TagStatus array value for that tag to zero, thus allowing
 *           further objects to be spawned from that tag.
 *
 *			 Some objects will want to call this function as soon as they
 *           are created, others will want to call this function just before
 *           they are destroyed.
 *
 * Notes:  While this function sets the TagStatus of a tag back to zero,
 *         other conditions must be met before that tag actually spawns a
 *         new object.
 *
 *         Just as with any other tag, that tag must enter the ZOC for the
 *         activator to take notice of it.  If the tag is out of the ZOC
 *         entirely, it will be ignored; similarly, if the tag is in the
 *         ZOC, but was also in the previous ZOC, it will be ignored.
 *
 *         In short, calling mReset on a tag will cause that tag to
 *         spawn a new object ONLY when that tag re-enters the ZOC from
 *         outside of the ZOC.
 *
 ***************************************************************************/

typedef struct _zactivator {
   _ZACTIVATOR
} Zactivator;

/****************************************************************************
 *		Zactivator *InitZactivator()
 *
 * Purpose:  Aside from being a textbook case of a function with too many
 *           parameters, this function allocates room in memory for an
 *           activator and assigns its parameters to activator structure
 *			 parts.
 ***************************************************************************/

Zactivator *InitZactivator(const unsigned short       metaMapIndex[],
		                   const          tagID       tagList[],
			                              int	      tagListSize,
				                          Render     *focus,
		                         unsigned int         Xextent,
				                 unsigned int	      Yextent,
				                 unsigned int	      gridX,
				                 unsigned int	      gridY,
						         unsigned int	      levelXsize,
						         unsigned int	      levelYsize,
								 void (*TagObjectCreate)(Zactivator *, tagID, int));

/****************************************************************************
 *		Zactivator *InitZactivatorTwin
 *
 * Purpose:  If you want to have more than one activator working on the same
 *           tags at the same time (perhaps one activator with a ZOC around
 *           the main character and one with a ZOC around the camera manager,
 *           or perhaps an activator for each player in a two-player game),
 *           they need to share some data, but have some differences.
 *
 *           InitZactivatorTwin allows one to hook a sibling activator
 *           to an already existing one so that they share tag list and tag
 *           status data but have different focal points and ZOC sizes.
 *
 * Note:  If you do not use InitZactivatorTwin to construct a second
 *        activator, and the activators use the same tag list and
 *        metaMapIndex, they will be prone to duplicating each other's
 *        efforts -- activating objects which have already been activated
 *        by the other activator.
 *
 *        Note that if you Destruct the oldest sibling, the younger siblings
 *        will potentially lose access to the TagStatus array which will be
 *        freed in the process of destructing the original Zactivator.  The
 *        best bet is not to Destruct an activator until it's younger twins
 *        have also been Destructed.
 ***************************************************************************/

Zactivator *InitZactivatorTwin( Zactivator *sibling,
					            Render     *focus,
					            int	        Xextent,
					            int	        Yextent,
								void *TagObjectCreate(Zactivator *, tagID, int)  );

Zactivator *ZactivatorNew( void );
Zactivator *ZactivatorConstruct( Zactivator *this );
void		OffsetFocus( Zactivator *this, int X, int Y );
void        ZactivatorDestruct( Zactivator *this );
void        ZactivatorDestructHeap( Zactivator *this );
void        ZactivatorTick( Zactivator *this );
#endif
/* EOF */
