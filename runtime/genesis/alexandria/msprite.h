
#ifndef _MSPRITE_H_
#define _MSPRITE_H_

#include <alexdef.h>
#include <gentypes.h>
/*
 *  C header file for mapsprit.asm library
 */

typedef struct
	{
		char  * pBufferBase;
		char  *	pCharBuffer;
		short	nBufferSize;	/* in characters */
		short	nNumChars;
		short 	nNumDMAs;
		void  *	pDeferDMA;
		char  	DeferDMAArray[80*14];
	}
		TMapSpriteHeader;

TMapSpriteHeader oMSH[];

void InitSprites (short x);

void ClearMSpriteTable (short);

short CharWithinMap (TMapData *pMapData, short x, short y);

short CountSpriteChars(TLogicalSpriteData *ls );

static char *MakeMapSpriteChars
	(TMapData *pMapData, char *pChar, char *pBuffer,
	 TLogicalSpriteData *pLSprite);

/*
char *AddMSprite
	(short n,short xOffset, short yOffset, short desc, TMapData *pMapData,
	 char *cChar, TLogicalSpriteData *pLSprite);
*/

void DownloadMappedSprites (short n);

void InitMSprites(short n, short nNumCharsToMalloc);


#endif
