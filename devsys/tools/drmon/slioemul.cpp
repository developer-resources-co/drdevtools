//============================================================================
//	emulation of slaveio.c: snes devel board stuff
//============================================================================

#include <assert.h>
#include <dos.h>

#include <pclib/boolean.hpp>

#include "global.hpp"
#include "error.hpp"
#include "reg.hpp"
#include "mem.hpp"

boolean
	CheckSlaveAlive(void)
	{
	return(boolean::FALSE);
	}

void
	UnInitSlaveIO( void )
	{
	}


boolean
InitSlaveIO(int base, unsigned short slaveBufferSeg )
{
	return(boolean::TRUE);
}

int SlaveGetVer(void)
{
	return(SLAVE_VER_NUMBER);
}

unsigned int
SwapWord(unsigned int w)
{
	return((w>>8)| w <<8);
}

//============================================================================

void
ReadSlaveData(unsigned long addr,char far *data,unsigned int len)
{
	char far* bPtr = (char far *)addr;
	while ( len-- )
		*data++ = *bPtr++;
}

#ifdef GENESIS
 void ReadSlaveVDP(unsigned long addr,char far *data,unsigned int len)
#endif

#ifdef SNES
 void ReadSlavePPU(unsigned long addr,char far *data, unsigned int len)
#endif
{
	char far *bPtr;
	bPtr = (char far *)addr;

	while(len--)
		*data++ = *bPtr++;
}

 void ReadSlaveCRAM (char far *data)
{
}

#ifdef GENESIS
 void ReadSlaveVSRAM (char far *data)
{
}
#endif

#ifdef SNES
 void ReadSlaveObjRAM( char far *data )
	{
	}
#endif

//============================================================================

 void
SlaveSetWriteProtect(void)
{
}

//============================================================================

 void
SlaveClearWriteProtect(void)
{
}

//============================================================================

 void
HandleSlaveInput(void)
{
    if(!slaveRunning)
     {
        modeUpdate = boolean::TRUE;
        PrintMessageStatus(STATUS_STOPPED);
     }
    else
     {
        if(modeUpdate)
            PrintMessageStatus(STATUS_RUNNING);
        else
            PrintMessageStatus(STATUS_RUNNOUPDATE);
     }
}

//============================================================================
// for when user quits, slave will know

void
GoodByeSlave(void)
{
}

//============================================================================

void
WriteSlaveData(unsigned long addr,char far *data,unsigned int len)
{
}

//============================================================================

errorcode
LoadFileToSlave (FILE *f, unsigned long len, unsigned long address)
{
return( NOERR );
}

//============================================================================

void
GetRegsFromSlave( ULONG regs[] )
{
}

//============================================================================

void
PutRegsToSlave( ULONG regs[] )
{
}

//============================================================================

boolean
SlaveCopyMem (long startaddr,long destaddr, long len)
{
	return(boolean::FALSE);
}

//============================================================================

boolean
SlaveFillMem (long startaddr,long len,long patlen,char far *pattern)
{
	return(boolean::FALSE);
}


 void
SlaveReset(void)
{
}

 void SlaveRun (void)
{
	slaveRunning = boolean::TRUE;
}


void SlaveStop (void)
{
	slaveRunning = boolean::FALSE;
}

 void SlaveRestart (void)
{
}

boolean SingleStep (void)
{
	return( boolean::TRUE );
}

 boolean
SlaveBkClr(long addr, unsigned int inst)
{
	return( boolean::TRUE );
}

uword
SlaveSetBkPt( ulong addr )
{
	return( 0 );
}


void SlaveClearBRKOnWrite( void )
	{
	}


void SlaveSetBRKOnWrite( void )
	{
	}


errorcode SaveFileFromSlave (FILE *f, unsigned long address, unsigned long len)
{
	return( NOERR );
}

boolean StepOverBreak (long addr, uword inst)
{
	return( boolean::TRUE );
}

//============================================================================

unsigned long MemRead(unsigned long addr,UBYTE size)
{
	unsigned long data;
	unsigned long far *bPtr;
	bPtr = (unsigned long far *)addr;

	data = *bPtr;
	data &= 0xffffffff >> ((4-size)*8);
	return(data);
}

//----------------------------------------------------------------------------
// kts 12-29-92 06:36am

void SendCmd ( char a)
{
}

unsigned int slaveCtrlc, slaveBankc, slaveWormc;	// for snesio.asm
unsigned char far * slaveBufferCmdc;

//----------------------------------------------------------------------------

