//===========================================================================
//	emulation of slaveio.c: snes devel board stuff
//===========================================================================

#include <assert.h>
#include <dos.h>

#include <pclib/boolean.hpp>

#include "mem.hpp"
#include "global.hpp"
#include "error.hpp"
#include "reg.hpp"

boolean bConvertUserBreaks = boolean::TRUE;
	// when a user-placed BRK occurs, replace with a NOP and set a
	// debugger-aware breakpoint

//===========================================================================
// hardware addresses and ports
// port addresses

#include "board.hpp"
static DevelopmentSystemBoard* devSys;

unsigned int slaveCtrlc, slaveBankc, slaveWormc;	// for snesio.asm
char far * slaveBufferCmdc;

unsigned int slaveDead, snesStatus;
unsigned char slaveCtrlBits;
unsigned char slaveRunningCtrlBits;
//#define SLAVEBANK_COM		(0x80)
#define ASYNC_BIT				(0x40)

//===========================================================================
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
#define	DB_CTRL_BRKONWRITE		0x20
// 0 = clear wprot viol, 1 = normal
#define	DB_CTRL_NMI			0x40
// 0 = NMI asserted, 1 = normal
#define	DB_CTRL_RESET		0x80
// 0 = reset asserted, 1 = normal
#define	DB_CTRL_NORM (DB_CTRL_BRKONWRITE|DB_CTRL_NMI)		// reset removed for SNES
//#define DB_CTRL_HOSTBUS DB_CTRL_NORM|DB_CTRL_ENAB|DB_CTRL_WHO
//#define DB_CTRL_TARGETBUS DB_CTRL_NORM

//===========================================================================
// i/o command bits
#define STATB_BYTEREADY	0
#define STATF_BYTEREADY	1<<STATB_BYTEREADY
#define STATB_BYTERECD	1
#define STATF_BYTERECD	1<<STATB_BYTERECD
#define STATB_BUSY		5
#define STATF_BUSY		0x20
#define STATB_ASYNC		6
#define STATF_ASYNC		0x40
#define STATB_READY		7
#define STATF_READY		0x80

//===========================================================================

void RecBytes( byte far* buffer, ulong size )
	{
	devSys->GetSlaveBus();

	for ( long i=0; i<size; ++i )
		*buffer++ = devSys->comram( i );

	devSys->ReturnSlaveBus();
	}


void SendBytes( char far * buffer, long size )
	{
	devSys->GetSlaveBus();

	Address a = 0;
	for ( long i=0; i<size; ++i )
		devSys->comram( a, (uchar)*buffer++ );

	devSys->ReturnSlaveBus();
	}


void SendDataBytes( char far * buffer, Address &addr, long size )
	{
	// %%% Untested

	devSys->GetSlaveBus();
	devSys->bank( BANK_COMRAM );

	for ( long i=0; i<size; ++i )
		devSys->comram( addr, (ubyte)*buffer++ );

	devSys->ReturnSlaveBus();
	}


//===========================================================================


void DebugMessage( void )
	{
	devSys->GetSlaveBus();

	int i = 0;
	Address a = 2;					// After message type
	while ( textBuffer[i++] = devSys->comram( a++ ) )
		if ( a > 40 ) break;

	devSys->ReturnSlaveBus();
	PrintToStatWindow( textBuffer );
	}

//===========================================================================


void
	GetMessage( void )
	{
	devSys->GetAttention();
	devSys->command(SNES_ASYNC);
	devSys->GetSlaveBus();

	int msgType = devSys->comram( (Address)0 );

	devSys->ReturnSlaveBus();

	switch ( msgType )
		{
		case 2:
			// Debug message
			DebugMessage();
			break;

		case 1:
			{ // Breakpoint
			GetRegsFromSlave(regs);
			ulong PCL = GetReg( REG_INSTRUCTIONPOINTER );

			PrintMessageStatus( 4 );

			if ( bConvertUserBreaks && !FindBreak( PCL ) )
				{
				// NOP byte, set breakpoint
				WriteSlaveDatum( PCL, 0xEA, 1 );
				AddBrkPt( PCL, 0, NULL );
				}
			else if( !BreakPointHit( PCL ) )
				{
				delay( 10 );										// ??? needed?
				slaveRunning = 0;
				DoSlaveRun();
				delay( 10 );										// ??? needed?
				return;
				}
			break;
			}

		default:
			PrintToConsoleWindow( "Unexcepted msgType %d", msgType );
			break;
		}

	devSys->ResumeSlave();
	delay( 10 );										// ??? needed?
	}

//===========================================================================


void HandleSlaveInput( void )
	{
	char a, temp;

	a = devSys->wormhole();
	if ( a & ASYNC_BIT )
		GetMessage();
	if ( slaveUpdate )
		GetRegsFromSlave( regs );
	if ( slaveDead )
		{
		PrintMessageStatus( STATUS_SLAVE_DEAD );
		return;
		}

	do
		{
		temp = devSys->wormhole();
		}
	while ( ( temp & STATF_BUSY ) && !( temp & STATF_READY ) );
														// !!! ADD TIMEOUT HERE

	slaveRunning = !( temp & STATF_READY );
	if ( !slaveRunning )
		{
		modeUpdate = boolean::TRUE;
		PrintMessageStatus( STATUS_STOPPED );
		}
	else
		{
		PrintMessageStatus( modeUpdate ? STATUS_RUNNING : STATUS_RUNNOUPDATE );
		}
	}


//===========================================================================
// register commands

void GetRegsFromSlave( ULONG regs[] )
	{
	long tempreg[ NUMREGS ];

	if ( !devSys->GetAttention() )
		{
		devSys->command( SNES_REGS_SNES2PC );

		RecBytes( (byte far*)tempreg, sizeof( ULONG ) * NUMREGS );

		for ( int i = 0; i < NUMREGS; ++i )
			regs[i] = tempreg[i] & 0xFFFF;			// 16 bits for the SNES

		devSys->ResumeSlave();
		LoadPCL();	 										// --- snes code had this
		}
	}


void PutRegsToSlave( ULONG regs[] )
	{
	if ( !devSys->GetAttention() )
		{
		SendBytes( (char far*)regs, sizeof( ULONG ) * NUMREGS );
		devSys->command( SNES_REGS_PC2SNES );
		devSys->ResumeSlave();
		}
	}


//===========================================================================
// control/misc commands

void SetSlaveCtrlBits( void )
	{
	devSys->control( slaveRunning ? slaveRunningCtrlBits : slaveCtrlBits );
	}


void ResumeSlave()
	{
	if ( snesStatus )
		SendCmd( SNES_GO );
	SetSlaveCtrlBits();
	}


void
	UnInitSlaveIO( void )
	{
	devSys->wormhole( 0 );
	delete devSys;
	}


boolean InitSlaveIO( int base, unsigned short slaveBufferSeg )
	{
	devSys = new DevelopmentSystemBoard();

	slaveCtrlBits = DB_CTRL_NORM;
	slaveRunningCtrlBits = DB_CTRL_NORM;

	return( boolean::TRUE );			//SlaveGetVer() == SLAVE_VER_NUMBER );
	}


int SlaveGetVer( void )
	{
	int nVersion;

	if ( devSys->GetAttention() )
		return ( 0 );

	devSys->command( SNES_VERSION_REQ );		// returns control after SNES processed
	devSys->GetSlaveBus();
	nVersion = (int)devSys->comram( (Address)0 );
	devSys->ReturnSlaveBus();
	devSys->ResumeSlave();

	return ( nVersion );
	}


boolean CheckSlaveAlive( void )
	{
	return ( slaveDead );
	}


void SlaveReset( void )
	{
	}


void SlaveRestart( void )
	{ // Pulse reset (restart cartridge)
	devSys->control( slaveRunningCtrlBits ^ DB_CTRL_RESET );
	devSys->control( slaveRunningCtrlBits );
	}


void SlaveSetWriteProtect( void )
	{
	slaveRunningCtrlBits |= DB_CTRL_WP;
	SetSlaveCtrlBits();
	}


void SlaveClearWriteProtect( void )
	{
	slaveRunningCtrlBits &= ~DB_CTRL_WP;
	SetSlaveCtrlBits();
	}

void SlaveClearBRKOnWrite( void )
	{
	slaveRunningCtrlBits |= DB_CTRL_BRKONWRITE;
	SetSlaveCtrlBits();
	}


void SlaveSetBRKOnWrite( void )
	{
	slaveRunningCtrlBits &= ~DB_CTRL_BRKONWRITE;
	SetSlaveCtrlBits();
	}


void GoodByeSlave( void )
	{
	}


//===========================================================================
// execution control

void SlaveRun( void )
	{
	if ( devSys->GetAttention() )
		return;
	devSys->command( SNES_GO );
	slaveRunning = boolean::TRUE;
	SetSlaveCtrlBits();
	}


void SlaveStop( void )
	{
	if ( slaveRunning )
		devSys->GetAttention();
	}


boolean SingleStep( void )
	{
	if ( slaveRunning )
		return ( boolean::TRUE );

	if ( devSys->GetAttention() )
		return ( boolean::FALSE );

	//if ( devSys->command( SNES_STEP ) == -1 )
		{
		GetRegsFromSlave( regs );
		//PutRegsToSlave( regs );	// last command just stepped over A -- restore it
		signed long offset = 0;

		ulong PC = GetReg( REG_INSTRUCTIONPOINTER );
		byte *xBuffer = (byte*)calloc( 4, 1 );
						// keep this signed so that backwards branching works!!!
		assert( xBuffer );
		char *textBuffer = (char*)calloc( 40, 1 );	// for opcode disassembly (remove later)
		assert( textBuffer );

		ReadSlaveData( PC, (char far*)xBuffer, 4 );			// Read instruction to trace over
		//delay( 10 );
		int len = Disassem( PC, (char far*)xBuffer, (char far*)textBuffer, 0 );

		ulong NextInstruction = PC + len;	// general-purpose and
														//  in case branches fail

		switch ( (unsigned char)*xBuffer++ )
			{
			case 0x10:	// û BPL r
				if ( !(GetReg( REG_FLAGS ) & 0x80 ) )
					NextInstruction += long( *xBuffer );
				break;

			case 0x30:	// û BMI r
				if ( (GetReg( REG_FLAGS ) & 0x80 ) )
					NextInstruction += long( *xBuffer );
				break;

			case 0x50:	//   BVC r
				if ( !(GetReg( REG_FLAGS ) & 0x40 ) )
					NextInstruction += long( *xBuffer );
				break;

			case 0x70:	//   BVS r
				if ( (GetReg( REG_FLAGS ) & 0x40 ) )
					NextInstruction += long( *xBuffer );
				break;

			case 0x90:	// û BCC r
				if ( !(GetReg( REG_FLAGS ) & 0x01 ) )
					NextInstruction += long( *xBuffer );
				break;

			case 0xB0:	// û BCS r
				if ( (GetReg( REG_FLAGS ) & 0x01 ) )
					NextInstruction += long( *xBuffer );
				break;

			case 0xD0:	// û BNE r
				if ( !(GetReg( REG_FLAGS ) & 0x02 ) )
					NextInstruction += long( *xBuffer );
				break;

			case 0xF0:	// û BEQ r
				if ( (GetReg( REG_FLAGS ) & 0x02 ) )
					NextInstruction += long( *xBuffer );
				break;

			case 0x80:	// û BRA r
				NextInstruction += long( *xBuffer );
				break;

			case 0x82:	// û BRL rl
				offset  = (ubyte)*xBuffer++;
				offset |= (*xBuffer++)<<8;
				NextInstruction += offset;
				break;

			case 0x20:	// û JSR a
			case 0x4C:	// û JMP a
				NextInstruction &= ~0xFFFFL;				// retain same bank
				NextInstruction |= (ubyte)*xBuffer++;
				NextInstruction |= (uword)(*xBuffer++)<<8;
				break;

			case 0x6C:	// û JMP (a)
				{
				ulong addr = 0;

				addr  = (ubyte)*xBuffer++;
				addr |= (uword)(*xBuffer++)<<8;
				addr |= PC & 0x00FF0000L;

				ReadSlaveData( addr, (char far*)&offset, 2 );
				NextInstruction &= ~0xFFFFL;					// retain same bank
				NextInstruction |= offset;
				break;
				}

			case 0xFC:	// û JSR (a,x)
			case 0x7C:	// û JMP (a,x)
				{
				ulong addr = 0;

				addr  = (ubyte)*xBuffer++;
				addr |= (uword)(*xBuffer++)<<8;
				addr |= PC & 0x00FF0000L;

				ReadSlaveData( addr+GetReg(REG_X), (char far*)&NextInstruction, 2 );
				break;
				}

			case 0x60:	// û RTS s
				{
				uword rts;
				ulong SP = GetReg( REG_STACKPOINTER );

				ReadSlaveData( SP+1, (char far*)&rts, 2 );
				NextInstruction &= ~0xFFFFL;					// retain same bank
				NextInstruction |= rts+1;
				break;
				}

			case 0x22:	// û JSL al
			case 0x5C:	// û JMP al (JML al)
				offset  = (ubyte)*xBuffer++;
				offset |= (uword)(*xBuffer++)<<8;
				offset |= (ulong)(*xBuffer++)<<16;
				NextInstruction = offset;
				break;

			case 0x6B:	// û RTL s
				{
				ulong rtl = 0;
				ulong SP = GetReg( REG_STACKPOINTER );

				ReadSlaveData( SP+1, (char far*)&rtl, 3 );
				NextInstruction = rtl+1;
				break;
				}

			case 0xDC:	// û JML (a)
				{
				ulong addr = 0;
				addr  = (ubyte)*xBuffer++;
				addr |= (uword)(*xBuffer++)<<8;
				ReadSlaveData( addr, (char far*)&NextInstruction, 3 );
				break;
				}


			// Untested, but pretty sure it works
			case 0x40:	//   RTI s
				{
				ulong rts;
				ulong SP = GetReg( REG_STACKPOINTER );

				ReadSlaveData( SP+1+1, (char far*)&rts, 2 );	// Skip Flags
				NextInstruction = rts;
				break;
				}
			}

		boolean existingBreakpoint =
				FindBreak( NextInstruction ) ? boolean::TRUE : boolean::FALSE;

		uword NextInstructionOpcode;

		if ( !existingBreakpoint )
			{ // Set a breakpoint at the next instruction
			NextInstructionOpcode = SlaveSetBkPt( NextInstruction );
			}

		delay( 10 );
		SlaveRun();

		// Wait until ready
		// Add timeout!!!
		while ( !(devSys->wormhole() & STATF_ASYNC) )
			;

//		devSys->GetAttention();
		devSys->command(SNES_ASYNC);
//		devSys->GetSlaveBus();
//		int msgType = devSys->comram( (Address)0 );
//		devSys->ReturnSlaveBus();

		if ( !existingBreakpoint )
			{ // Clear the breakpoint we set to get to the next instruction
			SlaveBkClr( NextInstruction, NextInstructionOpcode );
			}
		}

	return ( boolean::TRUE );
	}


boolean StepOverBreak( long addr, uword inst )
	{
	if ( devSys->GetAttention() )
		return( boolean::FALSE );

	devSys->GetSlaveBus();

	Address a = 0;
	devSys->comram( a, (unsigned long)addr );
	devSys->comram( a, inst );

	devSys->ReturnSlaveBus();
	//if ( devSys->command( SNES_STEP_OVER_BREAK ) == -1 )
		{ // Simulate
		SlaveBkClr( addr, inst );
		SingleStep();													// check TRUE/FALSE?
		SlaveSetBkPt( addr );
		}
	devSys->ResumeSlave();

	return( boolean::TRUE );
	}


//===========================================================================
// break control

boolean SlaveBkClr( long addr, unsigned int inst )
	{
	if ( devSys->GetAttention() )
		return( boolean::FALSE );

#if 0
	devSys->GetSlaveBus();
	devSys->bank( BANK_COMRAM );

	Address a = 0;
	devSys->comram( a, (ulong)addr );
	devSys->comram( a, inst );

	devSys->ReturnSlaveBus();
	if ( devSys->command( SNES_CLEAR_BREAK ) == - 1 )
#endif
		{
		WriteSlaveData( addr, (char far*)&inst, 1 );
		}
	devSys->ResumeSlave();

	return( boolean::TRUE );
	}


uword SlaveSetBkPt( ulong addr )
	{
	uint result;
	Address beginningOfBoard = 0;

	if ( devSys->GetAttention() )
		return( boolean::FALSE );

	devSys->GetSlaveBus();

	// Write the address to set breakpoint at
	devSys->comram( beginningOfBoard, addr );

	devSys->ReturnSlaveBus();
	devSys->command( SNES_SET_BREAK );

	devSys->GetSlaveBus();
	// Read the data which was at that address
	//if ( devSys->command() == -1 )
		{ // Simulate
		uword opcode;

		ReadSlaveData( addr, (char far*)&opcode, 1 );
		result = opcode;

		opcode = 0;			// BRK
		WriteSlaveData( addr, (char far*)&opcode, 1 );
		}
	//else
	//	result = devSys->comram( beginningOfBoard );

	devSys->ReturnSlaveBus();
	devSys->ResumeSlave();

	return( result );
	}


//===========================================================================
// memory i/o commands

void ReadSlaveData( unsigned long addr, char far* data, unsigned int len )
	{
	if ( len && !devSys->GetAttention() )
		{
		devSys->GetSlaveBus();

		Address a = 0;
		// 4 bytes (long) containing address to read
		devSys->comram( a, addr );
		// 4 bytes (long) containing amount of data to read
		devSys->comram( a, (ulong)len );

		devSys->ReturnSlaveBus();
		devSys->command( SNES_SEND_SNES2PC );
		devSys->GetSlaveBus();


		RecBytes( data, (ulong)len );

		devSys->ReturnSlaveBus();
		devSys->ResumeSlave();
		}
	}


void WriteSlaveData( unsigned long addr, char far * data, unsigned int len )
	{
	unsigned long far * buff;

	if ( len && !devSys->GetAttention() )
		{
		devSys->GetSlaveBus();

		Address a = 0;
		// 4 bytes (long) containing address to write
		devSys->comram( a, addr );
		// 4 bytes (long) containing amount of data to write
		devSys->comram( a, (unsigned long)len );

		for ( long i=0; i<len; ++i )
			devSys->comram( a, (uchar)*data++ );

		devSys->ReturnSlaveBus();
		devSys->command( SNES_SEND_PC2SNES );
		devSys->ResumeSlave();
		}
	}


// for x parser

unsigned long MemRead( unsigned long addr, UBYTE size )
	{
	unsigned long data;

	ReadSlaveData( addr, (char far*)&data, 4 );
	data &= 0xffffffff >> ( ( 4 - size ) * 8 );
	return ( data );
	}

//===========================================================================

//const int BUFSIZE = 32768;


errorcode LoadFileToSlave( FILE * f, unsigned long address, unsigned long len )
	{
	errorcode error = NOERR;

	if ( !devSys->GetAttention() )
		{
		devSys->GetSlaveBus();
		devSys->write( f, len, address );
		devSys->ReturnSlaveBus();
		devSys->ResumeSlave();
		}

#if 0
	void * buffer;
	unsigned int bigchunks;
	unsigned int remainder;
	unsigned long a;

	bigchunks = len / BUFSIZE;
	remainder = len % BUFSIZE;
	buffer = malloc( BUFSIZE );
	if ( !buffer )
		return ( ERROR_NOMEM );
	if ( GetAttention() )
		return ( NOERR );			  // lie
	for ( a = 0; a < bigchunks; ++a )
		{
		fread( buffer, BUFSIZE, 1, f );
		WriteSlaveData( address + ( a * BUFSIZE ), (char far*)buffer, BUFSIZE );
		}
	fread( buffer, remainder, 1, f );
	WriteSlaveData( address + ( a * BUFSIZE ), (char far*)buffer, remainder );
	free( buffer );
#endif

	return ( error );
	}

//===========================================================================


errorcode SaveFileFromSlave( FILE * f, unsigned long address, unsigned long len
							 )
	{
#if 0
	void * buffer;
	unsigned int bigchunks;
	unsigned int remainder;
	unsigned long a;

	bigchunks = len / BUFSIZE;
	remainder = len % BUFSIZE;
	buffer = malloc( BUFSIZE );
	if ( !buffer )
		return ( ERROR_NOMEM );
	for ( a = 0; a < bigchunks; a++ )
		{
		ReadSlaveData( address + ( a * BUFSIZE ), (char far*)buffer, BUFSIZE );
		fwrite( buffer, BUFSIZE, 1, f );
		delay( 10 );
		}
	ReadSlaveData( address + ( a * BUFSIZE ), (char far*)buffer, remainder );
	fwrite( buffer, remainder, 1, f );
	free( buffer );
#endif

	return ( NOERR );
	}


//===========================================================================
// internal memory commands

boolean SlaveCopyMem( long startaddr, long destaddr, long len )
	{
#if 0
	unsigned long far * buff;

	if ( len )
		{
		if ( GetAttention() )
			return ( boolean::FALSE );
		GetSlaveBus();
		buff = (unsigned long far*)slaveBufferData;
		outportb( slaveBank, SLAVEBANK_COM );
		*buff++ = startaddr;
		*buff++ = destaddr;
		*buff++ = len;
		ReturnSlaveBus();
		SendCmd( SNES_COPY_MEM );
		ResumeSlave();
		return ( boolean::TRUE );
		}
	return( boolean::FALSE );
#else
	return( boolean::TRUE );
#endif
	}


// @@@ patlen & pattern don't seem to work (i.e., requester above this
// @@@ function call doesn't parse things like "1,2")
boolean SlaveFillMem( long startaddr, long len, long patlen, char far * pattern )
	{
	word command;

	if ( len && !devSys->GetAttention() )
		{
		devSys->GetSlaveBus();

		Address a = 0;
		devSys->comram( a, (unsigned long)startaddr );
		devSys->comram( a, (unsigned long)len );

		for ( int i=len; i>patlen; i-=patlen )
			devSys->comram( a, (ubyte)*pattern );

		devSys->ReturnSlaveBus();
		devSys->command( SNES_SEND_PC2SNES );
		devSys->ResumeSlave();

		return boolean::TRUE;
		}

#if 0
		{
		Address a = 0;
		devSys->comram( a, (ulong)startaddr );				// *buff++ = startaddr;
		devSys->comram( a, (ulong)len );						// *buff++ = len;
		devSys->comram( a, (ulong)patlen );					// *buff++ = patlen;
		SendDataBytes( pattern, a, patlen );

		devSys->ReturnSlaveBus();
		command = devSys->command( SNES_FILL_MEM );
		devSys->ResumeSlave();
		return( command != -1 );
		}
#endif
	return( boolean::FALSE );
	}


//===========================================================================
// ppu memory i/o

void ReadSlavePPU( unsigned long addr, char far * data, unsigned int len )
	{
	uword command;

	if ( len )
		{
		if ( devSys->GetAttention() )
			return;

		devSys->GetSlaveBus();

		Address a = 0;
		devSys->comram( a, (ulong)addr );
		devSys->comram( a, (uword)len );

		devSys->ReturnSlaveBus();
		command = devSys->command( SNES_READ_PPU );
		devSys->GetSlaveBus();

//		if ( command )
			{
			RecBytes( data, (unsigned long)len );
			}
		ResumeSlave();
		}
	}


void ReadSlaveCRAM( char far * data )
	{
#if 0
	unsigned char h, l;

	if ( GetAttention() )
		return;
	SendCmd( SNES_READ_CRAM );
	GetSlaveBus();
	outportb( slaveBank, SLAVEBANK_COM );
	if ( *slaveBufferCmd )
		RecBytes( data, 128 );		 // --- CHECK # OF BYTES ON SNES
	ReturnSlaveBus();
	ResumeSlave();
#endif
	}


void ReadSlaveObjRAM( char far * data )
	{
#if 0
	if ( GetAttention() )
		return;
	SendCmd( SNES_READ_OBJRAM );
	GetSlaveBus();
	outportb( slaveBank, SLAVEBANK_COM );
	if ( *slaveBufferCmd )
		RecBytes( data, 80 );		  // --- CHECK # OF BYTES ON SNES
	ReturnSlaveBus();
	ResumeSlave();
#endif
	}

