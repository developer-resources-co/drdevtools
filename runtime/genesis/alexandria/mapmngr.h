/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Title:		Map Manager Header File

File:		MAPMNGR.H

See Also:	MAPMNGR.C

-------------------------------------------------------------------------- */

#ifndef	_MAPMNGR_H_
#define	_MAPMNGR_H_

#include <gentypes.h>

#define	MTile_xPixSizeP2	9							/* meta tile x size in pix (power of 2) */
#define	MTile_yPixSizeP2	8							/* meta tile y size in pix (power of 2) */
#define	MTile_xPixSize		(1 << MTile_xPixSizeP2)		/* meta tile x size in pix */
#define	MTile_yPixSize		(1 << MTile_yPixSizeP2)		/* meta tile y size in pix */

#define	MTile_xSize_P2	5								/* meta tile size in tiles (product of 2) */
#define	MTile_ySize_P2	4

#define	Tile_xPixSize_P2	4							/* tile size in pixels (product of 2) */
#define	Tile_yPixSize_P2	4							/* tile size in pixels (product of 2) */

#define	MTile_xSize		(1 << MTile_xSize_P2)		/* meta tile size in tiles */
#define	MTile_ySize		(1 << MTile_ySize_P2)

#define	Tile_xSize		(1 << Tile_xSize_P2)		/* tile size in pixels */
#define	Tile_ySize		(1 << Tile_ySize_P2)

#define	NULL_TILE		0


#define ConstrainX(oldx) ( oldx & (mapXPixelSize-1) )
#define ConstrainY(oldy) ( oldy & (mapYPixelSize-1) )



extern	UWORD	mapXMetaSize, mapYMetaSize,			/* map size in meta tiles */
		 		mapXTileSize, mapYTileSize,			/* map size in tiles */
		 		mapXPixelSize, mapYPixelSize,		/* map size in pixels */
		 		mapXMetaSizeP2, mapYMetaSizeP2,		/* map size in meta tiles (product of 2) */
		 		mapXTileSizeP2, mapYTileSizeP2,		/* map size in tiles (product of 2) */
		 		mapXPixelSizeP2, mapYPixelSizeP2;	/* map size in pixels (product of 2) */

	void	MapManagerInit(const TMetaCharMap *, const TMetaMap *, const TFloorTable *);
	void	PixelToTile(short, short, UWORD *, UWORD *);
	void	GetMetaTileCoord(UWORD, UWORD, UWORD *, UWORD *);
	UWORD	GetMetaTileNumber(UWORD, UWORD);
	void	GetTileID(UWORD, UWORD, UWORD *, UWORD *, UWORD *);
	BOOL	FindSpecialTile(UWORD, UWORD *, UWORD *);
	BOOL	FindObjectTile(UWORD, UWORD *, UWORD *);


#endif


/* MAPMNGR.H -- EOF */

