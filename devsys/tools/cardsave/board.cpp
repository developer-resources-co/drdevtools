
#include <assert.h>
#include <alloc.h>

#include <pclib/progmetr.hpp>				// Shouldn't really be in here

#include "board.hpp"

	DevelopmentSystemBoard::DevelopmentSystemBoard(
		Port nBase, unsigned char far* slaveMem )
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
		slaveBuffer_ = (unsigned char far*)((long)slaveBuffer_ << 16);
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


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Communcations RAM

////////////////////////////////////////////////////////////////////////////////
// Read the command
uword
	DevelopmentSystemBoard::command( void )
	{
	return( (*slaveBufferCmd) | (*(slaveBufferCmd+1)<<8) );
	}

////////////////////////////////////////////////////////////////////////////////
// Write a command
uword
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


#if 0
ubyte
	DevelopmentSystemBoard::comram( Address& addr, ubyte data )
	{
	bank( BANK_COMRAM );

	*(slaveBufferData + addr*2) = data;

	addr += sizeof( data );
	return( data );
	}
#endif

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
		uchar far* buff, 				// data buffer to read from
		unsigned long len, 			// number of bytes to read in from file
		unsigned long destAddr 		// offset into emulation RAM
		)
	{
	unsigned bkOff;							// block offset
	ubyte far* boardPtr;
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
			ubyte far* bufferPtr = buff;

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
	ubyte far* boardPtr;
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
		unsigned long srcAddr 		// offset into emulation RAM
		)
	{
	unsigned bkOff;							// block offset
	ubyte far* boardPtr;
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

		// convert destAddr to a board address
		boardPtr = slaveBuffer_ + bkOff;

		for ( int i=0; i<numBytes; ++i )
			{
			fputc( *boardPtr++ );
			++boardPtr;										// SNES only
			}
		}
	}


void 
	DevelopmentSystemBoard::read( 
		uchar far* buff, 				// 
		unsigned long len, 			// number of bytes to read in from memory
		unsigned long srcAddr 		// offset into emulation RAM
		)
	{
	}
