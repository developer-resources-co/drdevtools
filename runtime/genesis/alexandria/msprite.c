#include <stdlib.h>

#include "debug.h"
#include "alexdef.h"
#include "gentypes.h"
#include "vdp.h"
#include "genesis.h"
#include "msprite.h"

extern void ClearSpace(char *c);

const static char xSize[] = { 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4 };
const static char ySize[] = { 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4 };

TMapSpriteHeader	oMSH[2];

/*============================================================================*/
/*
 * InitMSprites takes two parameters, which MapSprite phase-block you're
 * working with, and a pointer in 68K memory to hold the DMA buffer.
 *
 */

void
InitMSprites(short block, short nNumCharsToMalloc)
{
	char * p;

	if ( p = malloc ( nNumCharsToMalloc << 5))
	{
		oMSH[block].pBufferBase = oMSH[block].pCharBuffer = p;
		oMSH[block].nBufferSize = nNumCharsToMalloc;
		ClearMSpriteTable(block);
	}
	else
		Crash("InitMSprites:  malloc failed");
	return;
}

/*============================================================================*/

void
ClearMSpriteTable(short block)
{
	oMSH[block].pCharBuffer	= oMSH[block].pBufferBase;
	oMSH[block].nNumDMAs	= 0;
	oMSH[block].pDeferDMA	= oMSH[block].DeferDMAArray;
	oMSH[block].nNumChars	= 0;
	return;
}

/*============================================================================*/

PUBLIC short
CountSpriteChars(TLogicalSpriteData *ls )
{
	short charCount = 0;
	short	count;
	THardwareSprite *hsp;

	hsp = ls->oHardSprite;

	count = ls->wCount;
	while( count--)
	 {
		charCount += ySize[hsp->bSize] * xSize[hsp->bSize];
		hsp++;				/* point to next hardware sprite */
	 }
	return charCount;
}

/*============================================================================*/
#if 0
#define CharWithinMap( m, x, y ) ( (m->wMap)[x+y*m->xSize])

#ifndef CharWithinMap
short int
CharWithinMap(TMapData *mapptr,short x ,short y)
{
	return ( (mapptr->map)[x+y*mapptr->xsize]);
}
#endif

char *
AddMSprite(short n, short x, short y, short desc,TMapData *m, char *c,
	TLogicalSpriteData *ls )
{
	char *temp = oMSH[n].pCharBuffer;
	short numChars;

	oMSH[n].pCharBuffer = MakeMapSpriteChars ( m, c, oMSH[n].pCharBuffer, ls );
	numChars = (oMSH[n].pCharBuffer - temp) >> 5;

	AddLSpriteBase(ls,x,y,desc,0);
	oMSH[n].nNumChars += numChars;
	oMSH[n].nNumDMAs++;
}
#endif


#ifdef USEONEDMA

PUBLIC void
DownloadMappedSprites(short n)
{
	short nNumChars;

	nNumChars = (oMSH[n].nNextChar - oMSH[n].nFirstChar);
	if(nNumChars)
		DMACopy((oMSH[n].nFirstChar)<<5,nNumChars<<4,oMSH[n].CharBuffer);
}

#else

PUBLIC void
DownloadMappedSprites(short n)
{
	ExecuteDeferedDMA(oMSH[n].DeferDMAArray,oMSH[n].nNumDMAs);
}

#endif
