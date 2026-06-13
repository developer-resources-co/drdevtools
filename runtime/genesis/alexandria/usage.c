#include <alexdef.h>
#include <gentypes.h>
#include <vdp.h>
#include <text.h>

#include "usage.h"

/* VDP_HVCOUNT     EQU	$C00008 */

volatile unsigned short * const BeamPosition = (unsigned short *)(0xc00008);

typedef struct
	{
		char  	CharBuffer[8192];
		char  *	pCharBuffer;
       	short 	nFirstChar;
       	short 	nNextChar;
		short 	nNumDMAs;
		void  *	pDeferDMA;
		char  	DeferDMAArray[20*14];
	}
		TMapSpriteHeader;


extern TMapSpriteHeader oMSH[2];

PUBLIC int
PercentUsed(void)
{
	/* Retrieve vertical component of beam position, 
		throw away horizontal value. */
	short nScanline = *BeamPosition >> 8;		
	nScanline -= 0xE4;
	nScanline += ( nScanline < 0 ? 256 : 0 );
	return (100*nScanline) / 0xff;
}

PUBLIC int
VDPDMAPercentUsed(short n)
{
	return ((oMSH[n].nNextChar - oMSH[n].nFirstChar)*3200)/8192;
}

