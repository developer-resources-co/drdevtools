/*                      Alexandria Genesis Library
 *
 * Title:		Zone Activator
 * File:		ZACTIVAT.C
 * See Also:	ZACTIVAT.H
 *
 * Created:		22 July 1993
 * Blame:		Dave Gross
 *
 * Changed:     3 August 1993 -- Tag positions in pixels, not characters
 * Placed in Library:  5 August 1993
 * "InZone" added:  16 August 1993
 * "InZone" corrected:  23 August 1993
 * Many untested attempts at efficiency added:  25 August 1993
 */

#ifndef FALSE
#define FALSE 0
#endif
#ifndef ZACTIVATOR_TYPE
#define ZACTIVATOR_TYPE 0x80000000
#endif
#include "stdlib.h"
#include "assert.h"
#include "render.h"
#include "text.h"
#include "cdebug.h"
#include "object.h"
#include "zactivat.h"
#include "tags.h"
#include "subzone.h"

char InZone( unsigned int X, unsigned int Y, Zactivator *activator )
{
  int Width, Height, LX, UY, RX, DY;
#ifndef ZFAST
  char i;
  ZOC GridZone;
#endif

  Width   = activator->nLevelXSize;
  Height  = activator->nLevelYSize;
  LX     = (activator->pFocus->xPos+activator->nFocalXOffset
            - activator->nXExtent/2);
  UY     = (activator->pFocus->yPos+activator->nFocalYOffset
            - activator->nYExtent/2);
  RX     = (LX + activator->nXExtent);
  DY     = (UY + activator->nYExtent);

  while (LX < 0) LX+=Width;
  while (UY < 0) UY+=Height;
  while (RX < 0) RX+=Width;
  while (DY < 0) DY+=Height;
  while (LX >= Width) LX-=Width;
  while (UY >= Height) UY-=Height;
  while (RX >= Width)  RX-=Width;
  while (DY >= Height) DY-=Height;

  if( (LX < RX) && (UY < DY) )
  {
#ifdef ZFAST
	if((X>=LX) && (Y>=UY) && (X<=RX) && (Y<=DY))
		return(1);
#else
	/* The ZOC is in one piece */
	GridZone[0].number=1;
	GridZone[0].ULC.xPosition = LX;
	GridZone[0].ULC.yPosition = UY;
	GridZone[0].LRC.xPosition = RX;
	GridZone[0].LRC.yPosition = DY;
#endif
  }
  else if( (LX > RX) && (UY < DY) )
  {
#ifdef ZFAST
	if(   ((Y>=UY) && (Y<=DY) && (X<=RX))
	   || ((X>=LX) && (X<=Width-1) && (Y>=UY) && (Y<=DY)) )
		return(1);
#else
	/* The ZOC is in two pieces, left and right */
	GridZone[0].number=2;
	GridZone[0].ULC.xPosition = 0;
	GridZone[0].ULC.yPosition = UY;
	GridZone[0].LRC.xPosition = RX;
	GridZone[0].LRC.yPosition = DY;
	GridZone[1].ULC.xPosition = LX;
	GridZone[1].ULC.yPosition = UY;
	GridZone[1].LRC.xPosition = Width-1;
	GridZone[1].LRC.yPosition = DY;
#endif
  }
    else if( (LX < RX) && (UY > DY) )
  {
#ifdef ZFAST
	if(   ((X>=LX) && (X<=RX) && (Y<=DY))
	   || ((X>=LX) && (X<=RX) && (Y>=UY) && (Y<=Height-1)) )
		return(1);
#else
	/* The ZOC is in two pieces, top and bottom */
	GridZone[0].number=2;
	GridZone[0].ULC.xPosition = LX;
	GridZone[0].ULC.yPosition = 0;
	GridZone[0].LRC.xPosition = RX;
	GridZone[0].LRC.yPosition = DY;
	GridZone[1].ULC.xPosition = LX;
	GridZone[1].ULC.yPosition = UY;
	GridZone[1].LRC.xPosition = RX;
	GridZone[1].LRC.yPosition = Height-1;
#endif
  }
  else if( (LX > RX) && (UY > DY) )
  {
#ifdef ZFAST
	if(   ((X<=RX) && (Y<=DY))
	   || ((X>=LX) && (X<=Width-1) && (Y<=DY))
	   || ((Y>=UY) && (X<=RX) && (Y<=Height-1))
	   || ((X>=LX) && (Y>=UY) && (X<=Width-1) && (Y<=Height-1)) )
	    return(1);
#else
	/* The ZOC is in four pieces */
	GridZone[0].number=4;
	GridZone[0].ULC.xPosition = 0;
	GridZone[0].ULC.yPosition = 0;
	GridZone[0].LRC.xPosition = RX;
	GridZone[0].LRC.yPosition = DY;
	GridZone[1].ULC.xPosition = LX;
	GridZone[1].ULC.yPosition = 0;
	GridZone[1].LRC.xPosition = Width-1;
	GridZone[1].LRC.yPosition = DY;
	GridZone[2].ULC.xPosition = 0;
	GridZone[2].ULC.yPosition = UY;
	GridZone[2].LRC.xPosition = RX;
	GridZone[2].LRC.yPosition = Height-1;
	GridZone[3].ULC.xPosition = LX;
	GridZone[3].ULC.yPosition = UY;
	GridZone[3].LRC.xPosition = Width-1;
	GridZone[3].LRC.yPosition = Height-1;
#endif
  }

#ifndef ZFAST
  for(i=0;i<GridZone[0].number;i++)
  {
    if(   (X >= GridZone[i].ULC.xPosition)
	   && (X <= GridZone[i].LRC.xPosition)
	   && (Y >= GridZone[i].ULC.yPosition)
	   && (Y <= GridZone[i].LRC.yPosition)
	   ) return(1);
  }
#endif
  return(0);
}
/****************************************************************************
 * 		int GridX(Zactivator *Zactivator, unsigned int x)
 * 		int GridY(Zactivator *Zactivator, unsigned int y)
 *
 * Purpose:  Given a pixel coordinate, returns the associated grid coordinate
 ****************************************************************************
 * int GridX(Zactivator *Zactivator, unsigned int x)
 * {
 *  assert(Zactivator);
 *  assert(x < Zactivator->nLevelXSize);
 *  if(   (!Zactivator)
 * 	 || (x >= Zactivator->nLevelXSize) ) return 0;
 *  return(x / Zactivator->nGridXSize);
 * }
 *
 * int GridY(Zactivator *Zactivator, unsigned int y)
 * {
 *   assert(Zactivator);
 *   assert(y < Zactivator->nLevelYSize);
 *   if(   (!Zactivator)
 * 	    || (y >= Zactivator->nLevelYSize) ) return 0;
 *  return(y / Zactivator->nGridYSize);
 * }
 ***************************************************************************/

#define GridX( a, b ) ( (b) / ((a)->nGridXSize) )
#define GridY( a, b ) ( (b) / ((a)->nGridYSize) )

/****************************************************************************
 *	   void CheckGrid(Zactivator *Zactivator, unsigned int x, unsigned int y,
 *                    ZOC oldZOC, ZOC newZOC)
 *
 * Purpose:  Given an x/y grid (not pixel) location in the level, scan
 *           through that grid block -- for each tag in the block, ask
 *           if it is in the new ZOC and out of the ZOC.  If this is true,
 *           and the tag's status allows it to be activated, activate the
 *           tag.
 *
 * Notes:  Because the ZOC can potentially wrap-around the map both
 *         vertically and horizontally, the ZOC can be split into 1, 2, or 4
 *         sections, each of which might be the section containing the grid
 *         block specified (if the ZOC is wide and/or tall enough, more than
 *         one section of it may contain a portion of the specified grid
 *         block within it as well).
 *
 *         The ZOC arrays contain these four sections, although often less
 *         than four are actually active.
 *
 *         Each tag in the block is then checked to see if it is in any
 *         section of the new ZOC.  If it is, then it is checked to see
 *         if it was in any section of the old ZOC.  If it wasn't, the
 *         tag is activated if it's TagStatus is zero (whereupon the
 *         TagStatus is changed to one.)
 ***************************************************************************/

void CheckGrid(Zactivator *Zactivator, unsigned int x, unsigned int y,
			   ZOC oldZOC, ZOC newZOC)
{
  unsigned int blockNumber, i, j;
  char frag;
  int next, prev;
  int tagX, tagY;

#ifndef ZFAST
  assert(Zactivator);
  assert(Zactivator->pnMetaMapIndex);
  assert(Zactivator->pnTagList);
  assert(Zactivator->pnTagStatus);
  assert( x < Zactivator->nGridX );
  assert( y < Zactivator->nGridY );

  if(   (!Zactivator)
	 || (!(Zactivator->pnMetaMapIndex))
	 || (!(Zactivator->pnTagList))
	 || (!(Zactivator->pnTagStatus))
	 || (x >= Zactivator->nGridX)
	 || (y >= Zactivator->nGridY) )
    return;
#endif

  blockNumber = x + (y * Zactivator->nGridX);

#ifndef ZFAST
  assert(blockNumber < (Zactivator->nGridX * Zactivator->nGridY));
  if(blockNumber >= (Zactivator->nGridX * Zactivator->nGridY)) return;
#endif

  prev = Zactivator->pnMetaMapIndex[blockNumber];
  next = Zactivator->pnMetaMapIndex[blockNumber+1];

#ifndef ZFAST
  assert((prev <= next));
  if (prev >= next) return;
#endif

  for( i=prev; i < next; i++ )
  {
    if(Zactivator->pnTagStatus[i+1]==0)
    {
	   tagX = Zactivator->pnTagList[i].xPosition;
	   tagY = Zactivator->pnTagList[i].yPosition;
/* Take out of here and check all tags in the init routine
 *	   assert(tagX <= Zactivator->nLevelXSize);
 *	   assert(tagY <= Zactivator->nLevelYSize);
 *	   if(   (tagX > Zactivator->nLevelXSize)
 *		  || (tagY > Zactivator->nLevelYSize) ) return;
 */
	   for(j=0; j<newZOC[0].number; j++)
	   {
		 frag = oldZOC[0].number;
	     if ( (   (tagX>=newZOC[j].ULC.xPosition) && (tagX<=newZOC[j].LRC.xPosition)
		       && (tagY>=newZOC[j].ULC.yPosition) && (tagY<=newZOC[j].LRC.yPosition) )
		    &&
			  (   (tagX<oldZOC[0].ULC.xPosition) || (tagX>oldZOC[0].LRC.xPosition)
		       || (tagY<oldZOC[0].ULC.yPosition) || (tagY>oldZOC[0].LRC.yPosition) )
		    &&
		      (   (frag < 2)
			   || (tagX<oldZOC[1].ULC.xPosition) || (tagX>oldZOC[1].LRC.xPosition)
		       || (tagY<oldZOC[1].ULC.yPosition) || (tagY>oldZOC[1].LRC.yPosition) )
		    &&
		      (   (frag < 4)
			   || (tagX>oldZOC[2].LRC.xPosition) || (tagY<oldZOC[2].ULC.yPosition) )
		    &&
		      (   (frag < 4)
		   	   || (tagX<oldZOC[3].ULC.xPosition) || (tagY<oldZOC[3].ULC.yPosition) )
	        )
	     {
		  Zactivator->mTagObjectCreate(Zactivator,
		                               Zactivator->pnTagList[i], i+1);
	      Zactivator->pnTagStatus[i+1] = 1;
	     }
	   }
    }
  }
}

/****************************************************************************
 *		void BlockFind(Zactivator *Zactivator)
 *
 * Purpose:  Each activator has a focus and an X and Y extent.  BlockFind
 *           determines which level-blocks are within the rectangle with
 *           the focal object at the center and with length and height equal
 *           to the X and Y extents respectively.
 *
 * Notes:  Because the ZOC can potentially wrap around the level map both
 *         vertically and horizontally, the ZOC can be split into 1, 2, or 4
 *         sections, each of which will contain grid blocks.
 *
 *         The ZOC arrays contain these four sections, although often less
 *         than four are actually active.
 *
 *         Any block which is partially in any ZOC section may potentially
 *         have tags which must be activated, so each of these blocks is
 *         checked using another functions.  If the ZOC is wide and/or tall
 *         enough, the same block may fall into more than one ZOC section
 *         and will therefore be checked twice -- a waste of time, but an
 *         action that should not cause bugs in the game, as a tag's
 *         TagStatus is flagged when it is first activated so that the
 *         second pass will not activate it again.
 *
 ***************************************************************************/

void BlockFind(Zactivator *Zactivator)
{
	int OLX, ORX, OUY, ODY;
	int NLX, NRX, NUY, NDY;
	int halfX, halfY;
	int X1, X2, X3, Y1, Y2, Y3;
	int i, j, x, y;
	ZOC newZOC, oldZOC;
	ZOC newGridZone, oldGridZone;
	int Width, Height;
	char frag;

/*** (O)ld (L)eft (X) (R)ight (Y) (U)p (D)own (N)ew ***/
#ifndef ZFAST
  assert(Zactivator);
  if(!Zactivator) return;
#endif

  Width   = Zactivator->nLevelXSize;
  Height  = Zactivator->nLevelYSize;

#ifndef ZFAST
  assert(Width > Zactivator->nXExtent);
  assert(Height > Zactivator->nYExtent);
  if(   (Width <= Zactivator->nXExtent)
	 || (Height <= Zactivator->nYExtent) ) return;
#endif

  halfX   = (Zactivator->nXExtent /2);
  halfY   = (Zactivator->nYExtent /2);
  OLX     = (Zactivator->nLastFocX+Zactivator->nFocalXOffset - halfX);
  OUY     = (Zactivator->nLastFocY+Zactivator->nFocalYOffset - halfY);
  ORX     = (OLX + Zactivator->nXExtent);
  ODY     = (OUY + Zactivator->nYExtent);
  NLX     = (Zactivator->pFocus->xPos+Zactivator->nFocalXOffset - halfX);
  NUY     = (Zactivator->pFocus->yPos+Zactivator->nFocalYOffset - halfY);
  NRX     = (NLX + Zactivator->nXExtent);
  NDY     = (NUY + Zactivator->nYExtent);

  while (OLX < 0) OLX+=Width;
  while (OUY < 0) OUY+=Height;
  while (ORX < 0) ORX+=Width;
  while (ODY < 0) ODY+=Height;
  while (NLX < 0) NLX+=Width;
  while (NUY < 0) NUY+=Height;
  while (NRX < 0) NRX+=Width;
  while (NDY < 0) NDY+=Height;

  while (OLX >= Width)  OLX-=Width;
  while (OUY >= Height) OUY-=Height;
  while (ORX >= Width)  ORX-=Width;
  while (ODY >= Height) ODY-=Height;
  while (NLX >= Width)  NLX-=Width;
  while (NUY >= Height) NUY-=Height;
  while (NRX >= Width)  NRX-=Width;
  while (NDY >= Height) NDY-=Height;

  if( (OLX < ORX) && (OUY < ODY) )
  {
	/* The ZOC is in one piece */
	oldZOC[0].number=1;
	oldZOC[0].ULC.xPosition = OLX;
	oldZOC[0].ULC.yPosition = OUY;
	oldZOC[0].LRC.xPosition = ORX;
	oldZOC[0].LRC.yPosition = ODY;
  }
  else if( (OLX > ORX) && (OUY < ODY) )
  {
	/* The ZOC is in two pieces, left and right */
	oldZOC[0].number=2;
	oldZOC[0].ULC.xPosition = 0;
	oldZOC[0].ULC.yPosition = OUY;
	oldZOC[0].LRC.xPosition = ORX;
	oldZOC[0].LRC.yPosition = ODY;
	oldZOC[1].ULC.xPosition = OLX;
	oldZOC[1].ULC.yPosition = OUY;
	oldZOC[1].LRC.xPosition = Width-1;
	oldZOC[1].LRC.yPosition = ODY;
  }
  else if( (OLX < ORX) && (OUY > ODY) )
  {
	/* The ZOC is in two pieces, top and bottom */
	oldZOC[0].number=2;
	oldZOC[0].ULC.xPosition = OLX;
	oldZOC[0].ULC.yPosition = 0;
	oldZOC[0].LRC.xPosition = ORX;
	oldZOC[0].LRC.yPosition = ODY;
	oldZOC[1].ULC.xPosition = OLX;
	oldZOC[1].ULC.yPosition = OUY;
	oldZOC[1].LRC.xPosition = ORX;
	oldZOC[1].LRC.yPosition = Height-1;
  }
  else if( (OLX > ORX) && (OUY > NDY) )
  {
	/* The ZOC is in four pieces */
	oldZOC[0].number=4;
	oldZOC[0].ULC.xPosition = 0;
	oldZOC[0].ULC.yPosition = 0;
	oldZOC[0].LRC.xPosition = ORX;
	oldZOC[0].LRC.yPosition = ODY;
	oldZOC[1].ULC.xPosition = OLX;
	oldZOC[1].ULC.yPosition = 0;
	oldZOC[1].LRC.xPosition = Width-1;
	oldZOC[1].LRC.yPosition = ODY;
	oldZOC[2].ULC.xPosition = 0;
	oldZOC[2].ULC.yPosition = OUY;
	oldZOC[2].LRC.xPosition = ORX;
	oldZOC[2].LRC.yPosition = Height-1;
	oldZOC[3].ULC.xPosition = OLX;
	oldZOC[3].ULC.yPosition = OUY;
	oldZOC[3].LRC.xPosition = Width-1;
	oldZOC[3].LRC.yPosition = Height-1;
  }

  if( (NLX < NRX) && (NUY < NDY) )
  {
	/* The ZOC is in one piece */
	newZOC[0].number=1;
	newZOC[0].ULC.xPosition = NLX;
	newZOC[0].ULC.yPosition = NUY;
	newZOC[0].LRC.xPosition = NRX;
	newZOC[0].LRC.yPosition = NDY;
  }
  else if( (NLX > NRX) && (NUY < NDY) )
  {
	/* The ZOC is in two pieces, left and right */
	newZOC[0].number=2;
	newZOC[0].ULC.xPosition = 0;
	newZOC[0].ULC.yPosition = NUY;
	newZOC[0].LRC.xPosition = NRX;
	newZOC[0].LRC.yPosition = NDY;
	newZOC[1].ULC.xPosition = NLX;
	newZOC[1].ULC.yPosition = NUY;
	newZOC[1].LRC.xPosition = Width-1;
	newZOC[1].LRC.yPosition = NDY;
  }
  else if( (NLX < NRX) && (NUY > NDY) )
  {
	/* The ZOC is in two pieces, top and bottom */
	newZOC[0].number=2;
	newZOC[0].ULC.xPosition = NLX;
	newZOC[0].ULC.yPosition = 0;
	newZOC[0].LRC.xPosition = NRX;
	newZOC[0].LRC.yPosition = NDY;
	newZOC[1].ULC.xPosition = NLX;
	newZOC[1].ULC.yPosition = NUY;
	newZOC[1].LRC.xPosition = NRX;
	newZOC[1].LRC.yPosition = Height-1;
  }
  else if( (NLX > NRX) && (NUY > NDY) )
  {
	/* The ZOC is in four pieces */
	newZOC[0].number=4;
	newZOC[0].ULC.xPosition = 0;
	newZOC[0].ULC.yPosition = 0;
	newZOC[0].LRC.xPosition = NRX;
	newZOC[0].LRC.yPosition = NDY;
	newZOC[1].ULC.xPosition = NLX;
	newZOC[1].ULC.yPosition = 0;
	newZOC[1].LRC.xPosition = Width-1;
	newZOC[1].LRC.yPosition = NDY;
	newZOC[2].ULC.xPosition = 0;
	newZOC[2].ULC.yPosition = NUY;
	newZOC[2].LRC.xPosition = NRX;
	newZOC[2].LRC.yPosition = Height-1;
	newZOC[3].ULC.xPosition = NLX;
	newZOC[3].ULC.yPosition = NUY;
	newZOC[3].LRC.xPosition = Width-1;
	newZOC[3].LRC.yPosition = Height-1;
  }
  newGridZone[0].number = newZOC[0].number;
  oldGridZone[0].number = oldZOC[0].number;

  for(i=0;i<newGridZone[0].number;i++)
  {
	newGridZone[i].ULC.xPosition = GridX(Zactivator,newZOC[i].ULC.xPosition);
	newGridZone[i].ULC.yPosition = GridY(Zactivator,newZOC[i].ULC.yPosition);
	newGridZone[i].LRC.xPosition = GridX(Zactivator,newZOC[i].LRC.xPosition);
	newGridZone[i].LRC.yPosition = GridY(Zactivator,newZOC[i].LRC.yPosition);
  }
  for(i=0;i<oldGridZone[0].number;i++)
  {
	oldGridZone[i].ULC.xPosition = GridX(Zactivator,oldZOC[i].ULC.xPosition);
	oldGridZone[i].ULC.yPosition = GridY(Zactivator,oldZOC[i].ULC.yPosition);
	oldGridZone[i].LRC.xPosition = GridX(Zactivator,oldZOC[i].LRC.xPosition);
	oldGridZone[i].LRC.yPosition = GridY(Zactivator,oldZOC[i].LRC.yPosition);
  }
  frag = oldGridZone[0].number;
  for(i=0;i<(newGridZone[0].number);i++)
  {
	for(x=newGridZone[i].ULC.xPosition; x<=newGridZone[i].LRC.xPosition; x++)
	{
	  for(y=newGridZone[i].ULC.yPosition; y<=newGridZone[i].LRC.yPosition; y++)
	  {
	    if /* (x,y) is not completely in the old GridZone */
	      (
		     (   (x<=oldGridZone[0].ULC.xPosition) || (x>=oldGridZone[0].LRC.xPosition)
		      || (y<=oldGridZone[0].ULC.yPosition) || (y>=oldGridZone[0].LRC.yPosition) )
		   &&
		     (   (frag < 2)
		      || (x<=oldGridZone[1].ULC.xPosition) || (x>=oldGridZone[1].LRC.xPosition)
		      || (y<=oldGridZone[1].ULC.yPosition) || (y>=oldGridZone[1].LRC.yPosition) )
	       &&
		     (   (frag < 4)
		      || (x<=oldGridZone[2].ULC.xPosition) || (x>=oldGridZone[2].LRC.xPosition)
		      || (y<=oldGridZone[2].ULC.yPosition) || (y>=oldGridZone[2].LRC.yPosition) )
		   &&
		     (   (frag < 4)
		      || (x<=oldGridZone[3].ULC.xPosition) || (x>=oldGridZone[3].LRC.xPosition)
		      || (y<=oldGridZone[3].ULC.yPosition) || (y>=oldGridZone[3].LRC.yPosition) )
		  )
		  CheckGrid(Zactivator,x,y,oldZOC,newZOC);
	  }
	}
  }
}

/****************************************************************************
 *		void ZactivatorTick(Zactivator *this)
 *
 * Purpose:  The tick routine for an activator.  This routine calls BlockFind
 *           to search the ZOC for blocks and thereby for tags, then updates
 *           the LastFoc values
 ***************************************************************************/

void ZactivatorTick(Zactivator *this)
{
#ifndef ZFAST
  assert(this);
  assert(this->pFocus);
  if( (!this) || !(this->pFocus) ) return;
#endif
    if (((this->pFocus->xLoc.Int&0xfff0) != (this->nLastFocX&0xfff0)) ||
        ((this->pFocus->yLoc.Int&0xfff0) != (this->nLastFocY&0xfff0)))
        BlockFind(this);
  ObjectTick((Object *) this);
  this->nLastFocX = this->pFocus->xPos;
  this->nLastFocY = this->pFocus->yPos;
}

/****************************************************************************
 *		void ResetStatus(int ident, Zactivator *this)
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
 *           mReset function (ResetStatus; this function) with these two
 *           values, it forces a reset of the TagStatus array value for that
 *           tag to zero, thus allowing further objects to be spawned from
 *           that tag.
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
 *         In short, calling ResetStatus on a tag will cause that tag to
 *         spawn a new object ONLY when that tag re-enters the ZOC from
 *         outside of the ZOC.
 *
 ***************************************************************************/

void ResetStatus(int ident, Zactivator *this)
{
#ifndef ZFAST
  assert(this);
  assert(this->pnTagStatus);
  assert(ident <= this->nTagListSize);
  assert(ident);                       /* ident != 0 as of 13 August 1993 */
  if(   !(this)
	 || !(this->pnTagStatus)
	 || (ident > this->nTagListSize)
	 || (ident == 0) ) return;
#endif
  this->pnTagStatus[ident] = 0;
}

/****************************************************************************
 *		void ZactivatorDestruct(Zactivator *this)
 *
 * Purpose: Destroy an instance of a Zone Activator
 *
 ***************************************************************************/

void ZactivatorDestruct(Zactivator *this)
{
   assert(this);
   free(this->pnTagStatus);
   ObjectDestruct((Object *) this);
}

/****************************************************************************
 *		void ZactivatorDestructHeap(Zactivator *this)
 *
 * Purpose: Destroy an instance of a Zone Activator, free it's memory
 *
 ***************************************************************************/

void ZactivatorDestructHeap(Zactivator *this)
{
	assert(this);
	ZactivatorDestruct(this);
	if (this)
	  free (this);
}

/****************************************************************************
 *		Zactivator *ZactivatorNew(void)
 *
 * Purpose:  Allocate space for a Zone Activator, set mDestruct
 *
 ***************************************************************************/

Zactivator *ZactivatorNew(void)
{
	Zactivator *this = malloc (sizeof(Zactivator));
	assert (this);
	if (this) {
	  ZactivatorConstruct(this);
	  this->mDestruct = ZactivatorDestructHeap;
	}
	return (this);
}

/****************************************************************************
 *		Zactivator *ZactivatorConstruct(Zactivator *this)
 *
 * Purpose:  Assigns values to parts of an already allocated activator
 *           structure.
 *
 ***************************************************************************/

Zactivator *ZactivatorConstruct(Zactivator *this)
{
  assert(this);
  this = ObjectConstruct((Object *) this);
  this->fActive          = 1;               /* I am an active object       */
  this->fPermanent       = 0;               /* No persistance after level  */
  this->fDelete          = 0;               /* Don't delete me             */
  this->fVisible         = 0;               /* I'm not a visible object    */
  this->fMusic           = 0;               /* I don't play music          */
  this->nTickDelay       = 0;               /* Start  immediately          */
  this->nObjectType      = ZACTIVATOR_TYPE; /* ???                         */
  this->mTick            = ZactivatorTick;  /* tick function               */
  return(this);
}

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
			                              int         tagListSize,
				                          Render     *focus,
		                         unsigned int         Xextent,
				                 unsigned int         Yextent,
				                 unsigned int         gridX,
					             unsigned int         gridY,
					             unsigned int         levelXsize,
					             unsigned int         levelYsize,
						   void (*TagObjectCreate)(Zactivator *, tagID, int))
{
  int i, j, prev, next, X, Y;
  Zactivator *this = ZactivatorNew();
  assert(this);
  assert(metaMapIndex);
  assert(tagList);
  assert(focus);
  assert(TagObjectCreate);
  assert(metaMapIndex[gridX*gridY] <= tagListSize);
#if 0
  for(i=0;i<(gridX*gridY);i++)
  {
	 prev = metaMapIndex[i];
	 if(i >= (gridX * gridY -1)) next = tagListSize;
	 else next = metaMapIndex[i+1];
	 if(prev>next) Crash("The metaMapIndex is not correct\n");
	 for(j=prev;j<next;j++)
	 {
		X = (i % gridX) * (levelXsize / gridX);
		Y = (i / gridX) * (levelYsize / gridY);
		if(   (tagList[j].xPosition < X)
		   || (tagList[j].xPosition > (X + levelXsize/gridX))
		   || (tagList[j].yPosition < Y)
		   || (tagList[j].yPosition > (Y + levelYsize/gridY))  )
		   Crash("The tagList does not match the metaMapIndex\n");
	 }
  }
#else
  for(i=0;i<tagListSize;i++)
  {
	assert(tagList[i].xPosition < levelXsize);
	assert(tagList[i].xPosition >= 0);
	assert(tagList[i].yPosition < levelYsize);
	assert(tagList[i].yPosition >= 0);
  }
#endif
  assert(Xextent < levelXsize);
  assert(Yextent < levelYsize);
  assert(gridX < levelXsize);
  assert(gridY < levelYsize);

  if(   !(this)
	 || !(metaMapIndex)
	 || !(tagList)
	 || !(focus)
	 || !(TagObjectCreate)
	 || (metaMapIndex[gridX*gridY] > tagListSize)
	 || (Xextent >= levelXsize)
	 || (Yextent >= levelYsize)
	 || (gridX >= levelXsize)
	 || (gridY >= levelYsize) )
    return((Zactivator *) 0);

  this->pnMetaMapIndex = metaMapIndex;
  this->nTagListSize   = tagListSize;
  this->pnTagList      = tagList;
  this->pnTagStatus    = (char *) malloc((tagListSize+1) * sizeof(char));
  for(i=0;i<=tagListSize;i++) this->pnTagStatus[i]=0;
  this->pFocus         = (Render *) focus;
  this->nFocalXOffset  = 0;
  this->nFocalYOffset  = 0;
  this->nXExtent       = Xextent;
  this->nYExtent       = Yextent;
  this->nLastFocX      = focus->xPos + Xextent;
  this->nLastFocY      = focus->yPos + Yextent;
  this->nGridX         = gridX;
  this->nGridY         = gridY;
  this->nGridXSize     = levelXsize / gridX;
  this->nGridYSize     = levelYsize / gridY;
  this->nLevelXSize    = levelXsize;
  this->nLevelYSize    = levelYsize;
  this->mReset         = ResetStatus;
  this->mTagObjectCreate = TagObjectCreate;
  this->mInZone        = InZone;
	return(this);
}

/****************************************************************************
 *		Zactivator *InitZactivatorTwin
 *
 * Purpose:  If you want to have more than one activator working on the same
 *           tags at the same time (perhaps one activator with a ZOC around
 *           the main character and one with a ZOC around the camera manager,
 *           or perhaps an activator for each player in a two-player game),
 *           they need to share some data, but have some differences.
 *
 *           InitZactivatorTwin allows one to hook a sibling activator to an
 *           already existing one so that they share tag list and tag status
 *           data but have different focal points and ZOC sizes.
 *
 * Note:  If you do not use InitZactivatorTwin to construct a second activator,
 *        and the activators use the same tag list and metaMapIndex, they
 *        will be prone to duplicating each other's efforts -- activating
 *        objects which have already been activated by the other activator.
 *
 *        Note that if you Destruct the oldest sibling, the younger siblings
 *        will potentially lose access to the TagStatus array which will be
 *        freed in the process of destructing the original Zactivator.  The
 *        best bet is not to Destruct an activator until it's younger twins
 *        have also been Destructed.
 ***************************************************************************/

Zactivator *InitZactivatorTwin( Zactivator *sibling,
						        Render     *focus,
						        int		    Xextent,
						        int		    Yextent,
								void *TagObjectCreate(Zactivator*, tagID, int))
{
  Zactivator *this;
  this = ZactivatorConstruct(ZactivatorNew());
  assert(this);
  assert(sibling);
  assert(sibling->pnMetaMapIndex);
  assert(sibling->pnTagList);
  assert(sibling->pnTagStatus);
  assert(focus);
  assert(TagObjectCreate);
  assert(Xextent < sibling->nLevelXSize);
  assert(Yextent < sibling->nLevelYSize);

  if(   !(this)
	 || !(sibling)
	 || !(sibling->pnMetaMapIndex)
	 || !(sibling->pnTagList)
	 || !(sibling->pnTagStatus)
	 || !(focus)
	 || !(TagObjectCreate)
	 || (Xextent >= sibling->nLevelXSize)
	 || (Yextent >= sibling->nLevelYSize)  )
    return((Zactivator *) 0);

  this->pnMetaMapIndex = sibling->pnMetaMapIndex;
  this->nTagListSize   = sibling->nTagListSize;
  this->pnTagList      = sibling->pnTagList;
  this->pnTagStatus    = sibling->pnTagStatus;
  this->nGridXSize     = sibling->nGridXSize;
  this->nGridYSize     = sibling->nGridYSize;
  this->nGridX		   = sibling->nGridX;
  this->nGridY		   = sibling->nGridY;
  this->nLevelXSize    = sibling->nLevelXSize;
  this->nLevelYSize    = sibling->nLevelYSize;
  this->pFocus         = focus;
  this->nFocalXOffset  = 0;
  this->nFocalYOffset  = 0;
  this->nXExtent       = Xextent;
  this->nYExtent       = Yextent;
  this->nLastFocX      = focus->xPos + Xextent;
  this->nLastFocY      = focus->yPos + Yextent;
  this->mReset         = ResetStatus;
  this->mTagObjectCreate = TagObjectCreate;
  this->mInZone        = InZone;
  return(this);
}

void OffsetFocus( Zactivator *this, int X, int Y )
{
   /* To be called after InitZactivator, but before the first mTick */
   assert(this);
   if(!this) return;
   this->nFocalXOffset = X;
   this->nFocalYOffset = Y;
   this->nLastFocX += X;
   this->nLastFocY += Y;
}

/* EOF */
