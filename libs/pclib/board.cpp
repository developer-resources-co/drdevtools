
#include <assert.h>
#include <stdlib.h>

#include <pclib/progmetr.hpp>				// Shouldn't really be in here

#include "board.hpp"

	DevelopmentSystemBoard::DevelopmentSystemBoard(
		Port nBase, unsigned char* slaveMem )
	{
	// Allocate buffer
	buffer = (unsigned char*)malloc( BUFSIZE );
//	assert( buffer );

	// Load environment variables
	char *strPtr;

	if ( strPtr = getenv( "DR_SNESPORT" ) )
		{
		sscanf( strPtr, "%x", &nBase );
		}

	if ( strPtr = getenv( "DR_SNESMEMBUFFER" ) )
		{
		sscanf( strPtr, "%x", &slaveBuffer_ );
		slaveBuffer_ = (unsigned char*)((long)slaveBuffer_ << 16);
		}

	// Read any .ini file settings

extern unsigned int slaveCtrlc, slaveBankc, slaveWormc;	// for snesio.asm
extern unsigned char* slaveBufferCmdc;

	slaveCtrlc = slaveCtrl = nBase++;
	slaveBankc = slaveBank = nBase++;
	slaveWormc = slaveWormhole = nBase;

	slaveBuffer_ = slaveMem;
	slaveBufferCmdc = slaveBufferCmd = slaveBuffer_;
	slaveBufferData = slaveBufferCmd+2*2;

	ResetPort();
	}


	DevelopmentSystemBoard::~DevelopmentSystemBoard()
	{
	}


///////////////////////////////////////////////////////////////////////////////

int
	DevelopmentSystemBoard::bank( int nBank )
	{
	bank_ = nBank;
	if ( nBank != BANK_COMRAM )
		nBank = ((( nBank&1) <<3 ) | ((nBank&0xE)>>1));
	outportb( slaveBank, nBank );
	return( bank() );
	}


controlBits
	DevelopmentSystemBoard::control( int cb )
	{
	outport( slaveCtrl, cb_ = cb );
	return( control() );
	}


#if 0
void
	DevelopmentSystemBoard::Reset( void )
	{
	control( slaveRunningCtrlBits ^ DB_CTRL_RESET );
	control( slaveRunningCtrlBits );
	}
#endif


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Communcations RAM

////////////////////////////////////////////////////////////////////////////////
// Read the command
word
	DevelopmentSystemBoard::command( void )
	{
	return( (*slaveBufferCmd) | (*(slaveBufferCmd+2)<<8) );
	}

////////////////////////////////////////////////////////////////////////////////
// Write a command
word
	DevelopmentSystemBoard::command( uword cmd )
	{
	// SendCmd stuff here
	bank( BANK_COMRAM );

	::SendCmd( cmd );

	return( command() );
	}

ubyte
	DevelopmentSystemBoard::comram( Address addr )
	{
	bank( BANK_COMRAM );
	return( *(slaveBufferData+addr*2) );		// SNES <= 512K
	}


#define LOBYTE(i)		((ubyte)( i & 0xFF ))
#define HIBYTE(i)		((ubyte)( i >> 8 ))

#define LOWORD(w)		((uword)( w & 0xFFFF ))
#define HIWORD(w)		((uword)( w >> 16 ))

uword
	DevelopmentSystemBoard::comram( Address& addr, uword data )
	{
	comram( addr, LOBYTE(data) );
	comram( addr, HIBYTE(data) );
	return( data );
	}

ulong
	DevelopmentSystemBoard::comram( Address& addr, ulong data )
	{
	comram( addr, LOWORD(data) );
	comram( addr, HIWORD(data) );
	return( data );
	}

////////////////////////////////////////////////////////////////////////////////

void
	DevelopmentSystemBoard::write(
		uchar* buff, 				// data buffer to read from
		unsigned long len, 			// number of bytes to read in from file
		unsigned long destAddr 		// offset into emulation RAM
		)
	{
	unsigned bkOff;							// block offset
	ubyte* boardPtr;
	unsigned numBytes;
	unsigned long originalLen = len;

	assert( buff );

//	progressMeter pm( 0L, len );

	for ( /*pm.proc(0)*/;
		/*pm.proc(originalLen-len),*/ len;
		destAddr += numBytes, len -= numBytes )
		{
//		bank( destAddr>>16 );								// Genesis
		bank( (destAddr&(512*1024-1)) >> (16-1) );	// SNES >512K

		// read (up to) 32K chunk into buffer
		numBytes = len > BUFSIZE ? BUFSIZE : len;		// min function
		assert( numBytes <= 32768 );

//		bkOff = destAddr & (BUFSIZE-1);					// Genesis
		bkOff = bank() < 0x10 ? 0 : 1;					// SNES >512K

//		if ( fread( (void*)buffer, 1, numBytes, fp ) == numBytes )
			{
			ubyte* bufferPtr = buff;

			// convert destAddr to a board address
			boardPtr = slaveBuffer_ + bkOff;

			for ( int i=0; i<numBytes; ++i )
				{
				*boardPtr++ = *bufferPtr++;
				++boardPtr;										// SNES only
				}
			}
		}
	}


void
	DevelopmentSystemBoard::write(
		FILE* fp, 						// file pointer to input file
		unsigned long len, 			// number of bytes to read in from file
		unsigned long destAddr 		// offset into emulation RAM
		)
	{
	unsigned bkOff;							// block offset
	ubyte* boardPtr;
	unsigned numBytes;
	unsigned long originalLen = len;

	assert( buffer );

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

		if ( fread( (void*)buffer, 1, numBytes, fp ) == numBytes )
			{
			ubyte* bufferPtr = buffer;

			// convert destAddr to a board address
			boardPtr = slaveBuffer_ + bkOff;

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


void
	DevelopmentSystemBoard::read(
		FILE* fp, 						// file pointer to output file
		unsigned long len, 			// number of bytes to read in from memory
		unsigned long destAddr 		// offset into emulation RAM
		)
	{
	}


void
	DevelopmentSystemBoard::read(
		uchar* buff, 				//
		unsigned long len, 			// number of bytes to read in from memory
		unsigned long destAddr 		// offset into emulation RAM
		)
	{
	}
