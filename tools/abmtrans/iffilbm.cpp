//==============================================================================
// iffilbm.cpp: funcions for ilbm parser built on iff parser
//==============================================================================

#include <iomanip.h>

#include <assert.h>
#include <stdlib.h>

#include <pclib/error.hpp>
#include "iffilbm.hpp"

//==============================================================================

ostream& operator<<( ostream& s, ilbmCMAP& cmap )
	{
	s << "[ CMAP ]" << endl;

	for ( int i=0; i<cmap.GetNumColors(); ++i )
		s << dec << setw( 3 ) << i << ": " << hex << setw( 2 ) <<
			int( cmap.Red(i) ) << " " <<
			int( cmap.Green(i) ) << " " <<
			int( cmap.Blue(i) ) << endl;

	return s;
	}


errorcode
ilbmCMAP::Read( iffRead* pRead )
	{
	ulong chunkLen;
	chunkLen = pRead->GetChunkLen();

	numColors = chunkLen / 3;

	delete[] colorPtr;

	colorPtr = new ubyte[numColors*3];
	assert( colorPtr );

	if ( colorPtr )
		pRead->GetChunkData( colorPtr, numColors*3 );

	return NOERR;
	}


errorcode
ilbmCMAP::Write( iffWrite* pWrite )
	{
	pWrite->NewChunk( tagCMAP );
	for ( int i=0; i < GetNumColors(); ++i )
		{
		pWrite->WriteChunkByte( Red( i ) );
		pWrite->WriteChunkByte( Green( i ) );
		pWrite->WriteChunkByte( Blue( i ) );
		}
	}

//==============================================================================

ostream& operator<<(ostream& s, ilbmBMHD &bmhd)
	{
	return s <<
		"[ BMHD ]" << endl <<
		"Width: " << bmhd.w << endl <<
		"Height: " << bmhd.h << endl <<
		"x: " << bmhd.x << endl <<
		"y: " << bmhd.y << endl <<
		"nPlanes: " << int(bmhd.nPlanes) << endl <<
		"masking: " << int(bmhd.masking) << endl <<
		"compression: " << int(bmhd.compression) << endl <<
		"transparentColor: " << bmhd.transparentColor << endl <<
		"xAspect: " << int(bmhd.xAspect) << endl <<
		"yAspect: " << int(bmhd.yAspect) << endl <<
		"pageWidth: " << bmhd.pageWidth << endl <<
		"pageHeight: " << bmhd.pageHeight << endl;
	}

errorcode
ilbmBMHD::Read(iffRead *readPtr)
{
	ulong chunkLen,i;

	w = readPtr->GetChunkWord();
	h = readPtr->GetChunkWord();
	x = readPtr->GetChunkWord();
	y = readPtr->GetChunkWord();
	nPlanes = readPtr->GetChunkByte();
	masking = readPtr->GetChunkByte();
	compression = readPtr->GetChunkByte();
	readPtr->GetChunkByte();					// skip pad
	transparentColor = readPtr->GetChunkWord();
	xAspect = readPtr->GetChunkByte();
	yAspect = readPtr->GetChunkByte();
	pageWidth = readPtr->GetChunkWord();
	pageHeight = readPtr->GetChunkWord();

	return(NOERR);
}

errorcode
ilbmBMHD::Write( iffWrite* pWrite )
	{
	pWrite->NewChunk( tagBMHD );

	pWrite->WriteChunkWord( w );
	pWrite->WriteChunkWord( h );
	pWrite->WriteChunkWord( x );
	pWrite->WriteChunkWord( y );
	pWrite->WriteChunkByte( nPlanes );
	pWrite->WriteChunkByte( masking );
	pWrite->WriteChunkByte( compression );
	pWrite->WriteChunkByte( 0 );					// skip pad
	pWrite->WriteChunkWord( transparentColor );
	pWrite->WriteChunkByte( xAspect );
	pWrite->WriteChunkByte( yAspect );
	pWrite->WriteChunkWord( pageWidth );
	pWrite->WriteChunkWord( pageHeight );

	return NOERR;
	}

//==============================================================================

ilbmBODY::ilbmBODY( iffRead* s, ilbmBMHD* bmhd, rastGraphPort* rp )
	{
	assert( bmhd ); this->bmhd = bmhd;
	assert( rp );  this->rp = rp;
	*s >> *this;
	}

ilbmBODY::Read( iffRead* readPtr )
	{
	// Temporary line buffer used to decompress masks into
	ubyte* maskBuffer = NULL;
	if ( bmhd->masking != ilbmBMHD::mskNone )
		{
		maskBuffer = new ubyte[ bmhd->w ];
		assert( maskBuffer );
		}

	for ( int y=0; y<bmhd->h; ++y )
		{
		// decompress straight into buffer
		decompLine( readPtr, rp->mapPtr + rp->lineArrayPtr[y] );

		if ( bmhd->masking == ilbmBMHD::mskHasMask )
			{
			uword byte_length = ((bmhd->w+15)/16)*2;	/* Must be even # bytes! */
			decompPlaneLine( readPtr, maskBuffer, byte_length, bmhd->compression );
			}
		}
	delete[] maskBuffer;
	}


int
	ilbmBODY::decompLine( iffRead* readPtr, ubyte* line_ptr )
	{
//	cout << '.';

	int result = EOF;

	ubyte *byte_buff = NULL;
	ubyte *from_ptr;
	ubyte *to_ptr;
	uword pixel_length = bmhd->w;
	uword byte_length = ((pixel_length+15)/16)*2;	/* Must be even # bytes! */
	uword x;

	int c;
	int plane;
	int i;
	int n_planes = bmhd->nPlanes;
	int plane_mask;


	byte_buff = (ubyte *)malloc(byte_length);
	if (byte_buff == NULL) goto Exit;

	memset(line_ptr, 0, pixel_length);        /* Zero out pixel buffer. */

	/* Decompress a line from each plane, and merge into pixels. */
	plane_mask = 1;
	for (plane=0;  plane<n_planes; plane++)
	{
	        /* Read and decompress a line of bytes in this plane. */
	        if (decompPlaneLine(readPtr, byte_buff, byte_length, bmhd->compression) != NULL)
	                goto Exit;

	        /* Now we have a line of plane data.  Convert planes to packed pixels. */
	        from_ptr = byte_buff;
	        to_ptr = line_ptr;
	        for (x=byte_length; x>0; x--) {

	                c = *from_ptr++;

	                for (i=8; i>0; i--) {

	                        if ( (c & 0x80) != 0)
	                                *to_ptr |= plane_mask;

	                        to_ptr++;
	                        c <<= 1;                        /* Pull off the bits, combine with pixels. */
	                }
	        }

	        plane_mask <<= 1;                /* Move plane mask left to select next plane. */
	}


	result = 0;                                        /* It was successful! */

Exit:
	if (byte_buff != NULL) free(byte_buff);
	return result;
	}

//==============================================================================

errorcode ilbmBODY::Write( iffWrite* s )
	{
//	pWrite->NewChunk( tagBODY );
	}

