// lzse.cpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "progmetr.hpp"

typedef unsigned char UBYTE;
typedef signed char BYTE;
typedef unsigned short UWORD;
typedef signed short WORD;
typedef unsigned long ULONG;
typedef signed long LONG;
typedef unsigned int BOOL;

static ULONG putindex;

static void
StartEncoder( void )
	{
	putindex = 0UL;
	}


static void
EncodeChar( UBYTE* outbuffer, unsigned c )
	{
	if ( c < 255 )
		outbuffer[ putindex++ ] = c;
	else
		outbuffer[ putindex++ ] = 255, outbuffer[ putindex++ ] = c-255;
	}


ULONG LZSEncode( UBYTE* inbuffer, UBYTE* outbuffer, ULONG inlength )
	{
	long ix, maxlength, position, tpos, tlen;

	StartEncoder();
	progressMeter pm( 0L, inlength );  pm.proc( 0 );
	EncodeChar( outbuffer, inlength>>8 );
	EncodeChar( outbuffer, inlength&0xFF );

	for ( ix=0; ix<inlength; )
		{
		if ( (ix%20) == 0 ) pm.proc( ix );
		//if ( (ix%100) == 0 ) putchar( '.' );

		maxlength = 0;
		position = ix;

		for ( tpos=ix-1; tpos>=0; --tpos )
			{
			for ( tlen=0; ; ++tlen )
				{
				if ( inbuffer[ ix+tlen ] != inbuffer[ tpos+tlen ] ) break;
				if ( ix+tlen >= inlength ) break;
				}

			if ( tlen > maxlength )
				{
				maxlength = tlen;
				position = ix - tpos;
				}
			}

		if ( maxlength > 3 )
			{
			ix += maxlength;
			if ( maxlength < 259 && position < 256 )
				{
				EncodeChar( outbuffer, 256 );
				EncodeChar( outbuffer, maxlength-4 );
				EncodeChar( outbuffer, position );
				}
			else if ( maxlength < 259 )
				{
				EncodeChar( outbuffer, 257 );
				EncodeChar( outbuffer, maxlength - 4);
				EncodeChar( outbuffer, position>>8 );
				EncodeChar( outbuffer, position & 0xFF );
				}
			else if ( position < 256 )
				{
				EncodeChar( outbuffer, 258 );
				maxlength -= 259;
				EncodeChar( outbuffer, maxlength>>8 );
				EncodeChar( outbuffer, maxlength&0xFF );
				EncodeChar( outbuffer, position );
				}
			else
				{
				EncodeChar( outbuffer, 259 );
				maxlength -= 259;
				EncodeChar( outbuffer, maxlength>>8 );
				EncodeChar( outbuffer, maxlength&0xFF );
				EncodeChar( outbuffer, position>>8 );
				EncodeChar( outbuffer, position&0xFF );
				}
			}
		else
			EncodeChar( outbuffer, inbuffer[ ix++ ] );
		}
	pm.proc( ix );
	return putindex;
	}
