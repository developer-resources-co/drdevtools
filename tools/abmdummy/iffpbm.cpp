//==============================================================================
// pbm.cpp: funcions for fucked pbm parser built on iff parser
//==============================================================================

#include <assert.h>
#include <stdlib.h>

#include "iffpbm.hpp"

//==============================================================================

pbmBODY::pbmBODY( ilbmBMHD* newBMHD, iffRead* readPtr, rastGraphPort* rp )
	{
	assert( newBMHD );
	bmhd = newBMHD;

//?	assert( bmhd->masking == ilbmBMHD::mskNone );
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
		ubyte* linePtr = rp->mapPtr + rp->lineArrayPtr[y];

		switch(bmhd->compression)
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
	}

//==============================================================================

errorcode pbmBODY::Read(iffRead *readPtr, errorcode parseLine(ubyte *line))
{
	ubyte* linePtr;
	uint column,oldColumn;
	byte commandByte;
	ubyte data;
	uint lines = bmhd->h;

	linePtr = new ubyte[bmhd->w];
	assert( linePtr );

	if(linePtr)
	 {
		switch(bmhd->compression)
		 {
			case ilbmBMHD::cmpNone:
				while(lines--)
	 			{
					// construct line
					for( column = 0; column < bmhd->w; column++)
						linePtr[column] = readPtr->GetChunkByte();

					readPtr->ChunkWordAlign();

					// now allow user to do something with it
					(*parseLine)(linePtr);					// call user supplied line parser
	 			}
				break;

			case ilbmBMHD::cmpByteRun1:
				while(lines--)
	 			{
					column = 0;
  					while ( column < bmhd->w )
  					 {
     					oldColumn = column;
     					commandByte = readPtr->GetChunkByte();
     					if (commandByte<0)
       					 {
       						data = readPtr->GetChunkByte();
       						while (column<oldColumn-commandByte+1)
       							linePtr[column++] = data;
       					 }
						else
       						while (column<oldColumn+commandByte+1)
       					linePtr[column++] = readPtr->GetChunkByte();
   					}

					// now allow user to do something with it
					(*parseLine)(linePtr);					// call user supplied line parser
	 			}

				break;
		 }
		delete[] linePtr;
	 }
	else
	 {
		return(ERROR_NOMEM);
	 }

	return(NOERR);
}

//==============================================================================

