//==============================================================================
// pbm.cpp: funcions for fucked pbm parser built on iff parser
//==============================================================================

#include <assert.h>
#include <stdlib.h>

#include "iffpbm.hpp"

//==============================================================================

pbmBODY::pbmBODY( rastGraphPort* rp )
	{
	assert( rp );
	this->rp = rp;
	}


pbmBODY::pbmBODY( iffRead* s, ilbmBMHD* bmhd, rastGraphPort* rp )
	{
	assert( bmhd );
	this->bmhd = bmhd;
	assert( rp );
	this->rp = rp;

	*s >> *this;
	}


pbmBODY::~pbmBODY()
	{
	}


errorcode
pbmBODY::Read( iffRead* readPtr )
	{
//?	assert( bmhd->masking == ilbmBMHD::mskNone );
	// I think masking still uses an ILBM-style line

	// Temporary line buffer used to decompress masks into
	ubyte* maskBuffer = NULL;
	if ( bmhd->masking != ilbmBMHD::mskNone )
		{
		maskBuffer = new ubyte[ bmhd->w ];
		assert( maskBuffer );
		}

	for ( int y=0; y<bmhd->h; ++y )
		{
		uint column,oldColumn;
		byte commandByte;
		ubyte data;
		ubyte* linePtr = rp->lineArrayPtr[y];

		switch ( bmhd->compression )
			{
			case ilbmBMHD::cmpNone:
				// construct line
				for ( column = 0; column < bmhd->w; ++column )
					linePtr[column] = readPtr->GetChunkByte();

				readPtr->ChunkWordAlign();
				break;

			case ilbmBMHD::cmpByteRun1:
				column = 0;
  				while ( column < bmhd->w )
  					{
     				oldColumn = column;
     				commandByte = readPtr->GetChunkByte();
     				if (commandByte<0)
       					{
       					data = readPtr->GetChunkByte();
       					while ( column < oldColumn-commandByte+1 )
       						linePtr[column++] = data;
       					}
					else
       					while ( column < oldColumn+commandByte+1 )
       						linePtr[column++] = readPtr->GetChunkByte();
   					}
				break;
	 		}
		}
	delete[] maskBuffer;

	return NOERR;
	}


errorcode
pbmBODY::Write( iffWrite* pWrite )
	{
	pWrite->NewChunk( tagBODY );

	switch ( bmhd->compression )
		{
		case ilbmBMHD::cmpNone:
			{
			for ( int y=0; y<rp->GetHeight(); ++y )
				{
				for ( int x=0; x<rp->GetWidth(); ++x )
					{
					int col = rp->GetPixelIndex( point(x,y) );
					pWrite->WriteChunkByte( col );
					}
				if ( x & 1 )
					pWrite->WriteChunkByte( 0 );	// odd lines must be padded to word
				}
			break;
			}

		case ilbmBMHD::cmpByteRun1:
			{
			for ( int y=0; y<rp->GetHeight(); ++y )
				{
				// Compress a line
				// Output it
				}
			break;
			}

		default:
			cerr << "Unknown compression type requested" << endl;
		}
	return NOERR;
	}

//==============================================================================
