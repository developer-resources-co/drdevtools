//============================================================================
// board.cpp: classes to deal with the DevSys board hardware
//============================================================================
//
//	see board.hpp for class interface
//
//============================================================================

#include <stdlib.h>
#include <assert.h>
//#include <alloc.h>

#ifdef DOSX286
#include <phapi.h>
#endif

//#include <pclib/progmetr.hpp>				// Shouldn't really be in here

#include "compat.hpp"
#include "moninc.hpp"


#ifdef SNES
#include "board.hpp"

	DevelopmentSystemBoard::DevelopmentSystemBoard(
		Port nBase, unsigned short slaveMemSeg )
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
		sscanf( strPtr, "%x", &slaveMemSeg );
		}

	// Read any .ini file settings

extern unsigned int slaveCtrlc, slaveBankc, slaveWormc;	// for snesio.asm
extern unsigned char* slaveBufferCmdc;

	slaveCtrlc = slaveCtrl = nBase++;
	slaveBankc = slaveBank = nBase++;
	slaveWormc = slaveWormhole = nBase;

#ifdef DOSX286
	unsigned short sel;

	DosMapRealSeg( slaveMemSeg, 65536, &sel );
	slaveBuffer_ = (char far*)MAKEP( sel, 0 );
#else
	slaveBuffer_ = (unsigned char far*)((long)slaveMemSeg << 16);
#endif
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
		nBank = ((( nBank&1) <<3 ) | ((nBank&0xE)>>1)) | (nBank & 0x10);
#ifndef EMUL
	outportb( slaveBank, nBank );
#endif
	return( bank() );
	}


DevelopmentSystemBoard::controlBits
	DevelopmentSystemBoard::control( int cb )
	{
#if !defined( EMUL )
	outport( slaveCtrl, cb_ = (DevelopmentSystemBoard::controlBits)cb );
#endif
	return( control() );
	}


#if 0
void
	DevelopmentSystemBoard::reset( void )
	{
	devSys->control( slaveRunningCtrlBits ^ DB_CTRL_RESET );
	devSys->control( slaveRunningCtrlBits );
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

// taken from phapi.h

#if 0

#ifndef LOBYTE
#define LOBYTE(val) ((ubyte)(val))
#endif

#ifndef HIBYTE
#define HIBYTE(val) ((ubyte)(((uword)(val)) >> 8))
#endif

#ifndef LOWORD
#define LOWORD(val) ((uword)(val))
#endif

#ifndef HIWORD
#define HIWORD(val) ((uword)(((ulong)(val)) >> 16))
#endif


#else
#define LOBYTE(i)		((ubyte)( i & 0xFF ))
#define HIBYTE(i)		((ubyte)( i >> 8 ))

#define LOWORD(w)		((uword)( w & 0xFFFF ))
#define HIWORD(w)		((uword)( w >> 16 ))
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

static int boardBank[ 32*2 ] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	};

static int boardOffset[ 32*2 ] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	};

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
//	unsigned long originalLen = len;

//	progressMeter pm( 0L, len );

	for ( /*pm.proc(0)*/;
		/*pm.proc(originalLen-len),*/ len;
		destAddr += numBytes, len -= numBytes )
		{
		bank( boardBank[ destAddr>>15 ] );

		// read (up to) 32K chunk into buffer
		numBytes = len > BUFSIZE ? BUFSIZE : len;		// min function
		assert( numBytes <= 32768 );

		bkOff = destAddr & (BUFSIZE-1);					// Genesis & SNES

		if ( fread( (void*)buffer, 1, numBytes, fp ) == numBytes )
			{
			ubyte* bufferPtr = buffer;

			// convert destAddr to a board address
			boardPtr = slaveBuffer_ + (bkOff<<1) + boardOffset[ destAddr>>15 ];

			for ( int i=0; i<numBytes; ++i )
				{
				*boardPtr++ = *bufferPtr++;
				++boardPtr;										// SNES only
				}
			}
		else
			{
			PrintMessageBar( "Error loading file." );
			break;
			}
		}

	command( SNES_RESTORE_VECTORS );
	}


#if 0
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
		bank( (destAddr&(512*1024-1)) >> (16-1) );	// +SNES >512K

		// read (up to) 32K chunk into buffer
		numBytes = len > BUFSIZE ? BUFSIZE : len;		// min function
		assert( numBytes <= 32768 );

		bkOff = destAddr & (BUFSIZE-1);					// Genesis
		bkOff |= bank() < 0x10 ? 0 : 1;					// +SNES >512K



			ubyte far* bufferPtr = buff;

			// convert destAddr to a board address
			boardPtr = slaveBuffer_ + bkOff * 2;

			for ( int i=0; i<numBytes; ++i )
				{
				*boardPtr++ = *bufferPtr++;
				++boardPtr;										// SNES only
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

	assert( fp );

//	progressMeter pm( 0L, len );

	for ( /*pm.proc(0)*/;
		/*pm.proc(originalLen-len),*/ len;
		destAddr += numBytes, len -= numBytes )
		{
//		bank( destAddr>>16 );								// Genesis
		bank( (destAddr&(512*1024-1)) >> (16-1) );	// +SNES >512K

		// read (up to) 32K chunk into buffer
		numBytes = len > BUFSIZE ? BUFSIZE : len;		// min function
		assert( numBytes <= 32768 );

		bkOff = destAddr & (BUFSIZE-1);					// Genesis
		bkOff |= bank() < 0x10 ? 0 : 1;					// +SNES >512K

		if ( fread( (void*)buffer, 1, numBytes, fp ) == numBytes )
			{
			ubyte far* bufferPtr = buffer;

			// convert destAddr to a board address
			boardPtr = slaveBuffer_ + bkOff * 2;

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
		uchar far* buff, 				//
		unsigned long len, 			// number of bytes to read in from memory
		unsigned long destAddr 		// offset into emulation RAM
		)
	{
	}
#endif

#endif

