
/* --------------------------------------------------------------------------

			               Map Manager  -- Ver 1.02
                           ------------------------

File:		MAPMNGR.C


Overview:

	The map manager performs some basic map calculations and stores the
	results in global variables. The are also some routines that can be
	called to search for specific object or special tiles, as well as
	determining what tiles are at a specific location. The available
	functions are:

	MapManagerInit				- initializes manager and calculates the
								  values of commonly used variables

	PixelToTile					- converts virtual playfield coordinates
								  to map coordinates in tiles

	GetMetaTileCoord			- given a tile coordinate, the meta tile
								  coordinate will be calculated

	GetMetaTileNumber			- given a meta tile coordinate, the meta
								  tile number will be returned

	GetTileID					- the values of the contour, special, and
								  object tiles at a specified location
								  will be returned

	FindSpecialTile				- start a search at a given location for
								  a specific special tile or continue the
								  search for the next occurance

	FindObjectTile				- start a search at a given location for
								  a specific object tile or continue the
								  search for the next occurance



-------------------------------------------------------------------------- */


/* compiler dependant */

#include <stdlib.h>
#include <assert.h>



/* module dependant */

#include "debug.h"
#include "mapmngr.h"
#include "alexdef.h"





/* ----------  Globally Accessable Variables  ---------- */

GLOBAL	UWORD			mapXMetaSize, mapYMetaSize,			/* map size in meta tiles */
						mapXTileSize, mapYTileSize,			/* map size in tiles */
						mapXPixelSize, mapYPixelSize,		/* map size in pixels */
						mapXMetaSizeP2, mapYMetaSizeP2,		/* map size in meta tiles (product of 2) */
						mapXTileSizeP2, mapYTileSizeP2,		/* map size in tiles (product of 2) */
						mapXPixelSizeP2, mapYPixelSizeP2;	/* map size in pixels (product of 2) */

PRIVATE	const TMetaCharMap	*poMetaCharMap;

PRIVATE	const TMetaMap		*poMetaMap;

PRIVATE	const TFloorTable	*poFloorTable;





/* ----------  private function declarations  ---------- */

PRIVATE UWORD getBitNumber(UWORD);






/* --------------------------------------------------------------------------

Usage:			(void) MapManagerInit(poMetaChr, poMetaMap, poFloorTbl)
				const TMetaCharMap	*poMetaChr		- meta character map
				const TMetaMap		*poMetaMap		- meta map
				const TFloorTable	*poFloorTable	- floor table

Returns:		nothing

Description:	Initializes the map manager routines with the passed information

Note:			Does not check the validity of the passed data

-------------------------------------------------------------------------- */

PUBLIC void
MapManagerInit(	const TMetaCharMap *poMCM,const TMetaMap *poMM,const TFloorTable *poFT)
{
	assert(poMCM);
	assert(poMM);
	assert(poFT);

	poMetaCharMap = poMCM;
	poMetaMap = poMM;
	poFloorTable = poFT;
	mapXMetaSize = poMetaMap->xSize;				/* map size in meta tiles */
	mapYMetaSize = poMetaMap->ySize;
	mapXTileSize = mapXMetaSize << 5;				/* map size in tiles */
	mapYTileSize = mapYMetaSize << 4;
	mapXPixelSize = mapXTileSize << 4;				/* map size in pixels */
	mapYPixelSize = mapYTileSize << 4;
	mapXMetaSizeP2 = getBitNumber(mapXMetaSize);	/* map size in meta tiles (product of 2) */
	mapYMetaSizeP2 = getBitNumber(mapYMetaSize);
	mapXTileSizeP2 = mapXMetaSizeP2 + MTile_xSize_P2;	/* map size in tiles (product of 2) */
	mapYTileSizeP2 = mapYMetaSizeP2 + MTile_ySize_P2;
	mapXPixelSizeP2 = mapXTileSizeP2 + 4;			/* map size in pixels (product of 2) */
	mapYPixelSizeP2 = mapYTileSizeP2 + 4;
}

/* --------------------------------------------------------------------------

Usage:			(void) PixelToTile(x, y, xTile, yTile)
				short	x, y			- coordinates in pixels
				UWORD	*xTile, *yTile	- tile coordinates constrained to map

Returns:		nothing

Description:	converts pixel to tile coordinates

-------------------------------------------------------------------------- */

PUBLIC void
PixelToTile(short x, short y, UWORD *xTile, UWORD *yTile)
{
	*xTile = (x & (mapXPixelSize - 1)) >> 4;	/* constrain to map and divide by 16 */
	*yTile = (y & (mapYPixelSize - 1)) >> 4;
}










/* --------------------------------------------------------------------------

Usage:			(void) GetMetaTileCoord(xTile, yTile, xMetaTile, yMetaTile)
				UWORD	xTile, yTile			- coordinates in tiles
				UWORD	*xMetaTile, *yMetaTile	- meta tile coordinates

Returns:		nothing

Description:	converts tile to meta tile coordinates

Note:			tile coordinates must be constrained to map

-------------------------------------------------------------------------- */

PUBLIC void
GetMetaTileCoord(UWORD xTile, UWORD yTile, UWORD *xMetaTile, UWORD *yMetaTile)
{
	*xMetaTile = xTile >> 5;
	*yMetaTile = yTile >> 4;
}










/* --------------------------------------------------------------------------

Usage:			metaTile = GetMetaTileNumber(xMetaTile, yMetaTile)
				UWORD	xMetaTile, yMetaTile	- meta tile coordinates

Returns:		nothing

Description:	returns the meta tile number pointed to by the coordinates

-------------------------------------------------------------------------- */

PUBLIC UWORD
GetMetaTileNumber(UWORD xMetaTile, UWORD yMetaTile)
{
	xMetaTile &= poMetaMap->xSize-1;

	if ( yMetaTile > poMetaMap->ySize )
		return NULL_TILE;

	return	poMetaMap->wMetaMap[(yMetaTile << mapXMetaSizeP2) + xMetaTile];
}


/* --------------------------------------------------------------------------

Usage:			(void) GetTileID(xTile, yTile, contourID, specialID, objectID)
				UWORD	xTile, yTile,		- tile coordinates
						*contourID,			- tile IDs
						*specialID,
						*objectID

Returns:		nothing

Description:	returns the tile ID at the specified location.  Any of the
					parameters specified as NULL will not be retrieved.

Note:			tile coordinates must be constrained to map

-------------------------------------------------------------------------- */

PUBLIC void
GetTileID(UWORD xTile, UWORD yTile, UWORD *contourID, UWORD *specialID, UWORD *objectID)
{
	unsigned long offset = GetMetaTileNumber(xTile >> MTile_xSize_P2, yTile >> MTile_ySize_P2) * 512;	/* tile number x 512 */

	offset += (yTile & (MTile_ySize - 1)) * MTile_xSize + (xTile & (MTile_xSize - 1));

	offset = poMetaCharMap[offset] >> 3;			/* must divide by 8 to get true offset */

	if ( contourID ) *contourID = poFloorTable[offset].wContourTile;
	if ( specialID ) *specialID = poFloorTable[offset].bSpecialTile;
	if ( objectID ) *objectID = poFloorTable[offset].bObjectTile;
}










/* --------------------------------------------------------------------------

Usage:			found = FindSpecialTile(tileID, xTile, yTile)
				BOOL	found;				- TRUE if tile found
				UWORD	tileID,				- special tile ID
						*xTile, *yTile,		- tile coordinates

Returns:		nothing

Description:	returns the location of the specified tile ID. If tileID
				is 0, a continued search is performed starting at the last
				location that the tile ID was located at.

Note:			tile coordinates must be constrained to map

-------------------------------------------------------------------------- */

PUBLIC BOOL
FindSpecialTile(UWORD tileID, UWORD *xTile, UWORD *yTile)
{
static	UWORD	mx, my, findTile;

		UWORD	specialID, dummy,
				xMetaTile, yMetaTile;

	if (tileID != 0) {							/* if not specifying continue */
		mx = *xTile;							/* start at specified coordinates */
		my = *yTile;
		findTile = tileID;
	}

	else
		mx++;									/* otherwise start at next tile location */

	for ( ; my < mapYTileSize; my++) {				/* loop through y */

		for ( ; mx < mapXTileSize; mx++) {			/* loop through x */

			if ((mx & (MTile_xSize - 1)) == 0) {		/* if now on meta tile boundary */
				if (GetMetaTileNumber(mx >> MTile_xSize_P2, my >> MTile_ySize_P2) == NULL_TILE) {
					mx += MTile_xSize - 1;				/* skip over meta tile to next one */
					continue;
				}
			}

			GetTileID(mx, my, &dummy, &specialID, &dummy);

			if (specialID == findTile) {			/* if matching tile found */
				*xTile = mx;
				*yTile = my;
				return TRUE;
			}
		}

		mx = 0;
	}

	return FALSE;
}










/* --------------------------------------------------------------------------

Usage:			found = FindObjectTile(tileID, xTile, yTile)
				BOOL	found;
				UWORD	tileID,				- special tile ID
						*xTile, *yTile,		- tile coordinates

Returns:		nothing

Description:	returns the location of the specified tile ID. If tileID
				is 0, a continued search is performed starting at the last
				location that the tile ID was located at.

Note:			tile coordinates must be constrained to map

-------------------------------------------------------------------------- */

PUBLIC BOOL
FindObjectTile(UWORD tileID, UWORD *xTile, UWORD *yTile)
{
static	UWORD	mx, my, findTile;

		UWORD	objectID, dummy,
				xMetaTile, yMetaTile;

	if (tileID != 0) {							/* if not specifying continue */
		mx = *xTile;							/* start at specified coordinates */
		my = *yTile;
		findTile = tileID;
	}

	else
		mx++;									/* otherwise start at next tile location */

	for ( ; my < mapYTileSize; my++) {				/* loop through y */

		for ( ; mx < mapXTileSize; mx++) {			/* loop through x */

			if ((mx & (MTile_xSize - 1)) == 0) {		/* if now on meta tile boundary */
				if (GetMetaTileNumber(mx >> MTile_xSize_P2, my >> MTile_ySize_P2) == NULL_TILE) {
					mx += MTile_xSize - 1;				/* skip over meta tile to next one */
					continue;
				}
			}

			GetTileID(mx, my, &dummy, &dummy, &objectID);

			if (objectID == findTile) {			/* if matching tile found */
				*xTile = mx;
				*yTile = my;
				return TRUE;
			}
		}

		mx = 0;
	}

	return FALSE;
}










/* --------------------------------------------------------------------------

Usage:			bit = getBitNumber(word)
				UWORD	bit					- number of bit that was set
				UWORD	word				- word to check

Returns:		bit number of first bit set

Description:	returns the bit location of the first set bit found, 0 being
				the lsb and 15 being the most.

-------------------------------------------------------------------------- */

PRIVATE UWORD
getBitNumber(UWORD w)
{
	UWORD	n = 0;

	while ((n < 16) && (w & 1) == 0) {
		w = w >> 1;
		n++;
	}

	return n;
}



/* MAP.C -- EOF */

