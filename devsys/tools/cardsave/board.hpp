
#if !defined( PCLIB_BOARD_H )
#define PCLIB_BOARD_H

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#include <pclib/general.hpp>

#include "send.hpp"

typedef unsigned short Port;
typedef unsigned long Address;

#if 0
//=============================================================================
// board equates
// ctrl equates
#define	DB_CTRL_ENAB		1
// 0 = mem off bus, 1 = mem on bus
#define	DB_CTRL_WHO			2
// 0 = target, 1 = host
#define	DB_CTRL_INHIB		4
// 0 = memory enables, 1 = memory disabled
#define	DB_CTRL_EIGHTBIT	8
// 0 = ?, 1 = ?
#define	DB_CTRL_WP			0x10
// 0 = target writes enabled, target writes inhibited
#define	DB_CTRL_CLEAR		0x20
// 0 = clear wprot viol, 1 = normal
#define	DB_CTRL_NMI			0x40
// 0 = NMI asserted, 1 = normal
#define	DB_CTRL_RESET		0x80
// 0 = reset asserted, 1 = normal

#define	DB_CTRL_NORM DB_CTRL_CLEAR|DB_CTRL_NMI|DB_CTRL_RESET

#define DB_CTRL_HOSTBUS DB_CTRL_NORM|DB_CTRL_ENAB|DB_CTRL_WHO
#define DB_CTRL_TARGETBUS DB_CTRL_NORM
#endif


const unsigned int BUFSIZE = 0x8000;

#define BANK_COMRAM 0x80

class DevelopmentSystemBoard
	{
public:
	enum controlBits
		{
		ENABLE_MEMORY = 1,
		DISABLE_MEMORY = 0,
		WHO_HOST = 2,
		WHO_TARGET = 0,
		INHIBIT = 4,											// never turn on
//		EIGHTBIT = 8,
		WRITE_PROTECT = 0x10,
		WRITE_ENABLE = 0,
		CLEAR_WRITE_PROTECT_VIOLATION = 0x20,			// ??
		NMI = 0x40,
		RESET = 0x80
		};
//	const NORMAL = CLEAR_WRITE_PROTECT_VIOLATION;
//	const HOSTBUS = NORMAL|ENABLE_MEMORY|WHO_HOST;
//	const TARGETBUS = NORMAL;


//	DevelopmentSystemBoard( Port nBase = 0x304 );
	DevelopmentSystemBoard( Port nBase = 0x318, unsigned char far* slaveMem=(unsigned char far*)0xD0000000 );
	virtual ~DevelopmentSystemBoard( void );

	int bank( int nBank );
	int bank( void )								{ return( bank_ ); }

	controlBits control( int cb );
	controlBits control( controlBits cb )	{ return( control( (int)cb ) ); }
	controlBits control( void )				{ return( cb_ ); }

	ubyte wormhole( ubyte wh )					{ outportb( slaveWormhole, wh ); return( wh ); }
	ubyte wormhole( void )						{ return( inportb( slaveWormhole ) ); }

	int GetAttention( void )					{ return( ::GetAttention() ); }
	void GetSlaveBus( void )					{ ::GetSlaveBus(); }
	void ReturnSlaveBus( void )				{ ::ReturnSlaveBus(); }
	void ResumeSlave( void )					{ ::ResumeSlave(); }
	void ResetPort( void )						{ wormhole( 0 ); }

	////////////////////////////////////////////////////////////////////////////
	// Communications RAM
	uword command( void );						// Read a command
	uword command( uword cmd );				// Write a command

	ubyte comram( Address );
	ubyte comram( Address &addr, ubyte data )
		{
		bank( BANK_COMRAM );
		*( slaveBufferData + addr*2 ) = data;
		addr += sizeof( data );
		return( data );
		}
	uword comram( Address&, uword );
	ulong comram( Address&, ulong );


	////////////////////////////////////////////////////////////////////////////
	// Emulation RAM
	void write( uchar far* buff, unsigned long len, unsigned long addr );
	void write( FILE* fp, unsigned long len, unsigned long addr );
	void read( uchar far* buff, unsigned long len, unsigned long addr );
	void read( FILE* fp, unsigned long len, unsigned long addr );


	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
private:

	Port slaveCtrl;
	enum controlBits cb_;
	Port slaveBank;
	int bank_;
	Port slaveWormhole;

	ubyte far* slaveBuffer_;
	ubyte far* slaveBufferCmd;
	ubyte far* slaveBufferData;

	ubyte* buffer;
	};


#endif

