/*
 * file types important to the Genesis
 */


#ifndef _GENTYPES_H_
#define _GENTYPES_H_


#include "alexdef.h"


typedef unsigned short	TMetaCharMap;

typedef unsigned char	TContourDef;

typedef unsigned int Tick;

typedef unsigned short TColor;

typedef struct {
	LONG	iSize;
	UBYTE	wData[32];
} TCharSetData;

typedef struct {
	UWORD	xSize;
	UWORD	ySize;
	UWORD	wMap[1];
} TMapData;

typedef struct {
	UWORD	xSize;
	UWORD	ySize;
	UWORD	wMetaMap[1];
} TMetaMap;

typedef struct {
	UWORD wUpperLeft;
	UWORD wUpperRight;
	UWORD wLowerLeft;
	UWORD wLowerRight;
} TXlateTable;

typedef struct {
	UWORD	wContourTile;
	UBYTE	bSpecialTile;
	UBYTE	bObjectTile;
} TFloorTable;

typedef unsigned short TPalette;

typedef struct {
	LONG	iSize;
	UWORD	wColor[1];
} TPaletteData;

typedef struct {
		short	nVPos;
		UBYTE  	bSize;
		UBYTE	bLink;
		UWORD	wBase;
		short	nHPos;
} THardwareSprite;

typedef struct {
		short	xOffset;
		short 	yOffset;
		UWORD 	xSize;
		UWORD 	ySize;
		UWORD 	wHardXOffset;
		UWORD	wHardYOffset;
		UWORD	wHardXSize;
		UWORD 	wHardYSize;
		UWORD 	wCount;
		THardwareSprite oHardSprite[1];
} TLogicalSpriteData;

typedef struct {
		UBYTE 	bSizeByte;
		UBYTE 	xOffset;
		UBYTE 	yOffset;
		UBYTE 	bPad;
} TMapSpriteHWS;	/* hardware sprite */

typedef struct {
		UWORD 	wNumSprites;
		TMapSpriteHWS oMapSpriteHWS[1];	/* actually (n) entries */
} TMapSpriteData;

typedef struct {
		UWORD 	wBase;				/* base of field in VDP */
		UWORD 	wNum;					/* 0 = A, 2 = B */
		UWORD 	wWidth;
		UWORD 	wHeight;
} TPlayField;



typedef struct {
		TPlayField 		oPlayField;				/* an entire playfield struct */
		short			xPosition;
		short			yPosition;
		TMapData 		*poMap;
		UWORD			wFirstChar;
		UWORD			wMapWidth;
		UWORD			wMapWidthPixel;
		UWORD			wMapHeight;
		UWORD			wMapHeightPixel;
		UWORD			wRightLimit;			/* pixels */
		UWORD			wBotLimit;
		UWORD			(*ScreenAddrRout)(void);	/* function to return screen address */
		UWORD			wPrevDirection;			/* 0 = left, -1 = right */
		TMetaMap		*poMetaMap;
		TXlateTable		*poCompTileTbl;
		UWORD			wMetaWidth;				/* metaTiles */
		UWORD			wMetaHeight;
		UWORD			wMapWidthMask;			/* map width -1 */
		UWORD			wMapWidthSh;			/* shift count ?*/
		UWORD			wMetaWidthSh;
		UWORD			wMapHeightMask;			/* map height - 1*/
} TScrollField;


struct DynamicSprite {
		void *poSpr;
		void *poChr;
};

typedef struct { signed char x1,y1,x2,y2; } TSmallRect;

typedef struct { short	x1,y1,x2,y2; } TRect;

typedef struct
{
	TFixedPoint xLoc;
	TFixedPoint yLoc;
} Point;


#endif


/* GENTYPES.H -- EOF */

