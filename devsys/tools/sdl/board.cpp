
#include <assert.h>
#include <alloc.h>

#include "progmetr.h"
#include "board.hpp"

	DevelopmentSystemBoard::DevelopmentSystemBoard( 
		Port nBase, unsigned char far* slaveMem )
	{
	// Allocate buffer
	buffer = (unsigned char*)malloc( BUFSIZE );
	assert( buffer );

	// Load environment variables
	char *strPtr;

	if ( strPtr = getenv( "DR_SNESPORT" ) )
		{
		sscanf( strPtr, "%x", &nBase );
		}

	if ( strPtr = getenv( "DR_SNESMEMBUFFER" ) )
		{
		sscanf( strPtr, "%x", &slaveBuffer_ );
		slaveBuffer_ = (unsigned char far*)((long)slaveBuffer_ << 16);
		}

	// Read any .ini file settings


	slaveCtrl = nBase++;
	slaveBank = nBase++;
	slaveWormhole = nBase;

	slaveBuffer_ = slaveMem;
	}


	DevelopmentSystemBoard::~DevelopmentSystemBoard()
	{
	}


///////////////////////////////////////////////////////////////////////////////

int
	DevelopmentSystemBoard::bank( int nBank )
	{
	bank_ = nBank;
	if ( nBank != 0x80 )
		nBank = ((( nBank&1) <<3 ) | ((nBank&0xE)>>1)) | (nBank & 0x10);
	outportb( slaveBank, nBank );
	return( bank() );
	}


controlBits
	DevelopmentSystemBoard::control( int cb )
	{
	outport( slaveCtrl, cb_ = cb );
	return( control() );
	}


ubyte
	DevelopmentSystemBoard::wormhole( ubyte wh )
	{
	outport( slaveWormhole, wormhole_ = wh );
	return( wormhole() );
	}

////////////////////////////////////////////////////////////////////////////////

void 
	DevelopmentSystemBoard::write( 
		FILE* fp, 						// file pointer to input file
		unsigned long len, 			// number of bytes to read in from file
		unsigned long destAddr 		// offset into emulation RAM
		)
	{
	unsigned bkOff;							// block offset
	ubyte far* boardPtr;
	unsigned numBytes;
	unsigned long originalLen = len;

	progressMeter pm( 0L, len );

	for ( pm.proc(0); 
		pm.proc(originalLen-len), len; 
		destAddr += numBytes, len -= numBytes )
		{
//		bank( destAddr>>16 );								// Genesis
		bank( (destAddr&(512*1024-1)) >> (16-1) );	// SNES >512K

		// read (up to) 32K chunk into buffer
		numBytes = len > BUFSIZE ? BUFSIZE : len;		// min function
		assert( numBytes <= 32768 );

		bkOff = destAddr & (BUFSIZE-1);					// Genesis & SNES
		bkOff |= bank() < 0x10 ? 0 : 1;					// SNES >512K

		if ( fread( (void*)buffer, 1, numBytes, fp ) == numBytes )
			{
			ubyte* bufferPtr = buffer;

			// convert destAddr to a board address
			boardPtr = slaveBuffer_ + bkOff*2;

			for ( int i=0; i<numBytes; ++i )
				{
				*boardPtr++ = *bufferPtr++;
				++boardPtr;										// SNES only
				}
			}
		else
			{
			cerr << "Error reading file\n";
			break;
			}
		}
	}


#if 0
void 
	DevelopmentSystemBoard::write( 
		FILE* fp, 						// file pointer to output file
		unsigned long len, 			// number of bytes to read in from memory
		unsigned long destAddr 		// offset into emulation RAM
		)
	{
	unsigned bkOff;							// block offset
	ubyte far* boardPtr;
	unsigned numBytes;
	unsigned long originalLen = len;

	progressMeter pm( 0L, len );

	for ( pm.proc(0); 
		pm.proc(originalLen-len), len; 
		destAddr += numBytes, len -= numBytes )
		{
//		bank( destAddr>>16 );								// Genesis
		bank( (destAddr&(512*1024-1)) >> (16-1) );	// SNES >512K

		// read (up to) 32K chunk into buffer
		numBytes = len > BUFSIZE ? BUFSIZE : len;		// min function
		assert( numBytes <= 32768 );

//		bkOff = destAddr & (BUFSIZE-1);					// Genesis
		bkOff = bank() < 0x10 ? 0 : 1;					// SNES >512K

		boardPtr = slaveBuffer_ + bkOff;


		if ( fread( (void*)buffer, 1, numBytes, fp ) == numBytes )
			{
			ubyte* bufferPtr = buffer;

			// convert destAddr to a board address
			boardPtr = slaveBuffer_ + bkOff;

			// crunch bytes
			for ( int i=0; i<numBytes; ++i )
				{
				*boardPtr++ = *bufferPtr++;
				++boardPtr;										// SNES only
				}
			}
		else
			{
			cerr << "Error reading file\n";
			break;
			}
		}
	}
#endif

