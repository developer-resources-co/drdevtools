/*===========================================================================*/
/* vdp.c: Video Display Processor inteface code
/*===========================================================================*/
/*
 * VDP.C -- C library routines for dealing with the VDP on a slightly
 * higher level
 */

#include <assert.h>

#include <cd.h>
#include <cdebug.h>
#include <genesis.h>
#include <gentypes.h>
#include <vmalloc.h>
#include <compress.h>
#include <alexdef.h>

/*===========================================================================*/

#include "vdp.h"
#include "sprite.h"

/*===========================================================================*/

const TCharSetData oBlankCharset =
{
	32L,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/*===========================================================================*/

short LoadCharsAtAddress( const TCharSetData* const p, unsigned short VDPCharPointer )
	{
	unsigned short i;
	unsigned long  pp = (unsigned long) p->wData;

	/* kts addions to support new compression code 04-30-93 07:13pm */
	if(*(UBYTE *)p == COMPTYPE_INVALID)			/* then must be old format */
		{

		i = p->iSize;

		if ( pp >> 16 == ((pp+i) >> 16))
			{ 	/* No bank crossing on DMA's */
			DMACopy( VDPCharPointer << 5, i >> 1, (void*)p->wData );
			}
		else
			{ /* Different banks -- split into 2 DMA's */

			unsigned long nBytes = ((pp | 0xffffUL ) - pp + 1);
			unsigned long nBytes2 = i-nBytes;

			/* copy first portion */
			DMACopy( VDPCharPointer*32, nBytes/2, (void*)(pp));

			/* Copy remainder */
			DMACopy( VDPCharPointer*32 + nBytes, nBytes2/2, (char *)(pp+nBytes));

			}
		return VDPCharPointer;
		}
	else						/* otherwise do new decompression code */
		{
		DecompressToVDP( (TComprData*)p, VDPCharPointer*CHARSIZE );
		return VDPCharPointer;
		}
	}

/*===========================================================================*/

short LoadChars( const TCharSetData * const  p )
{
	if ( p )
	{
		unsigned short VDPCharPointer;

		/* kts additions to support new compression code 04-30-93 07:13pm */
		if(*(UBYTE *)p == COMPTYPE_INVALID)			/* then must be old format */
		{
			unsigned short i = p->iSize;

			VDPCharPointer = vmalloc( i>>5 );

			if ( VDPCharPointer )
				LoadCharsAtAddress( p, VDPCharPointer );
			return( VDPCharPointer );
		}
		else						/* otherwise do new decompression code */
		{
			TComprData *tcd = (TComprData *)p;

			VDPCharPointer = vmalloc( tcd->dataLen >> 5 ) ;

			if (VDPCharPointer)
				DecompressToVDP(tcd,VDPCharPointer*CHARSIZE);

			return VDPCharPointer;
		}
	}
	else
	{
		InitVM();
		VMConsume( VDP_TOPFREE/CHARSIZE, (VDP_MEM_SIZEOF-VDP_TOPFREE)/CHARSIZE );	 /* force unavailable */
		LoadChars(&oBlankCharset);
		return( 0 );
	}
}

/*===========================================================================*/
