//==============================================================================
// ilbm.cpp: funcions for ilbm parser built on iff parser
//==============================================================================

#include <assert.h>
#include <stdlib.h>

#include <pclib/error.hpp>
#include <pclib/iffilbm.hpp>

//==============================================================================

errorcode
ilbmCMAP::Read(iffRead *readPtr)
{
	ulong chunkLen,i;
	chunkLen = readPtr->GetChunkLen();

	numColors = chunkLen / 3;

	delete[] colorPtr;

	colorPtr = new ubyte[numColors*3];

	if(!colorPtr)
//		throw(ERROR_NOMEM);			// wish I had throw
		return(ERROR_NOMEM);

	readPtr->GetChunkData(colorPtr,numColors*3);
	return(NOERR);
}

errorcode
ilbmCMAP::Write( iffWrite* pWrite )
	{
	for ( int i=0; i < GetNumColors(); ++i )
		{
		pWrite->WriteChunkByte( Red( i ) );
		pWrite->WriteChunkByte( Green( i ) );
		pWrite->WriteChunkByte( Blue( i ) );
		}
	}

//==============================================================================

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

int
	ilbmBODY::decompPlaneLine( iffRead* readPtr, ubyte *plane_line_ptr, int byte_length, ubyte compression)
	{
	int result = EOF;
	ubyte *to_ptr;
	uword bytes_written;

	int i;
	int c;
	int count;

	/* Read a line of plane data into the byte buffer. */
	bytes_written = 0;
	to_ptr = plane_line_ptr;

	/* Decompress using byteRun1. */
	if (compression == ilbmBMHD::cmpByteRun1)
	        {
	        while (1) {
	                count = readPtr->GetChunkByte();
	                if (count == EOF) goto Exit;

	                if (count >= 0x80) {
	                        count = 0x101 - count;

	                        c = readPtr->GetChunkByte();

	                        if (c == EOF) {
	                                goto Exit;
	                                }

	                        if ( (count+bytes_written) >byte_length) {
	                                Error(0, "Decompression problem (1).  Corrupt file?") ;
	                                goto Exit ;
	                                }

	                        memset(to_ptr,c,count) ;        // Optimized
	                        to_ptr += count ;
	                        bytes_written += count ;

	                        if (bytes_written == byte_length) {
	                                break ;
	                                }
	                        }
	                else {
	                        count ++ ;        // Add one to count

	                        if ( (count+bytes_written) >byte_length) {
	                                Error(0, "Decompression problem.  Corrupt file?") ;
	                                goto Exit ;
	                                }

	                        readPtr->GetChunkData( to_ptr, count );
	                        to_ptr += count ;
	                        bytes_written+= count;

	                        if (bytes_written==byte_length)
	                                break ;
	                }
	        }

	}

	/* "Decompress" by loading directly. */
	else if (compression == ilbmBMHD::cmpNone)
	{
	        for (i=byte_length; i>0; i--)
	        {
	                c = readPtr->GetChunkByte();
	                if (c == EOF) goto Exit;
	                *to_ptr++ = c;
	        }
	        goto finished;
	}

//?        readPtr->ChunkWordAlign();

	/* Error: picture uses an unsupported compression type. */
	else
	        goto Exit;


finished:
	result = (int) NULL;                /* It was successful! */

Exit:
	return result;
}



int
	ilbmBODY::decompLine( iffRead* readPtr, ubyte* line_ptr )
	{
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


errorcode ilbmBODY::Read(iffRead *readPtr, errorcode parseLine(ubyte *line))
	{
	ubyte *linePtr;

	linePtr = new ubyte[ bmhd->w ];
	assert( linePtr );

	if ( linePtr )
		{
		uint lines = bmhd->h;

		while ( lines-- )
			{
			decompLine( readPtr, linePtr );

			(*parseLine)( linePtr );

			if ( bmhd->masking == ilbmBMHD::mskHasMask )
				{
				uword byte_length = ((bmhd->w+15)/16)*2;	/* Must be even # bytes! */

				decompPlaneLine( readPtr, linePtr, byte_length, bmhd->compression );
				}
			}
		delete[] linePtr;
		}

	return( NOERR );
	}

//==============================================================================

