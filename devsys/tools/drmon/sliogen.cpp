//============================================================================
//	emulation of slaveio.c: snes devel board stuff
//============================================================================

#include <assert.h>
#include <mem.h>
#include <dos.h>

#include <pclib/boolean.hpp>

#include "global.hpp"
#include "error.hpp"
#include "reg.hpp"

//============================================================================
// hardware addresses and ports
							// port addresses
unsigned int slaveCtrl,slaveBank,slaveWorm;
unsigned char *slaveBufferCmd;
unsigned char *slaveBufferData;
unsigned int slaveDead,genStatus;

unsigned char slaveCtrlBits;
unsigned char slaveRunningCtrlBits;

#define SLAVEBANK_COM 0x80
#define ASYNC_BIT (0x40)

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

//============================================================================
// i/o command bits

#define STATB_BYTEREADY	0
#define STATF_BYTEREADY	1

#define STATB_BYTERECD	1
#define STATF_BYTERECD	2

#define STATB_BUSY	5
#define STATF_BUSY	0x20

#define STATB_ASYNC	6
#define STATF_ASYNC	0x40

#define STATB_READY	7
#define STATF_READY	0x80

//==============================================================================

enum GENCOMMANDS
{
    GEN_NOP,				//   00
    GEN_VERSION_REQ,		//   01
    GEN_SEND_PC2GEN,		//   02
    GEN_SEND_GEN2PC,		//   03
    GEN_RESET,				//   04
    GEN_REGS_PC2GEN,		//   05
    GEN_REGS_GEN2PC,		//   06
    GEN_GO,					//   07
    GEN_READ_VDP,			//   08
    GEN_READ_VSRAM,			//   09
    GEN_READ_CRAM,			//   0A
    GEN_WRITE_VDP,			//   0B
    GEN_WRITE_VSRAM,		//   0C
    GEN_WRITE_CRAM,			//   0D
    GEN_SET_Z80,			//   0E
    GEN_GET_Z80,			//   0F
    GEN_ASYNC,				//   10
    GEN_MESSAGE,			//   11
    GEN_RESET_Z80,			//   12
    GEN_COPY_MEM,			//   13
    GEN_FILL_MEM,			//   14
    GEN_STEP,				//   15
    GEN_SET_BREAK,			//   16
    GEN_CLEAR_BREAK,		//   17
    GEN_STEP_OVER_BREAK,	//   18
    GEN_SEARCH,				//   19
    GEN_SET_LATCH,			//   1A
	GEN_WRITE_BYTE			//   1B
};

struct _exception
{
    unsigned int flavor;
    unsigned int cycleflags;
    unsigned long address;
    unsigned int instruction;
};

_exception Exception;

#define EXCP_MESSAGE 7
#define EXCP_ADDR   0
#define EXCP_ILLEGAL	1
#define EXCP_MAX 7

//============================================================================

void RecBytes (char *buffer,long size)
{
	GetSlaveBus();
	outportb(slaveBank,SLAVEBANK_COM);
	memcpy(buffer,slaveBufferData,size);
	ReturnSlaveBus();
}

void SendBytes (char *buffer,long size)
{
	GetSlaveBus();
	outportb(slaveBank,SLAVEBANK_COM);
	memcpy(slaveBufferData,buffer,size);
	ReturnSlaveBus();
}

void SendDataBytes (char *buffer,char *dest, long size)
{
	GetSlaveBus();
	outportb(slaveBank,SLAVEBANK_COM);
	memcpy(dest,buffer,size);
	ReturnSlaveBus();
}

//============================================================================

char *ExceptionMessages[]=
{
 "Address Error",
 "Illegal Instr",
 "Div 0 Error  ",
 "Trace!!      ",
 "ALine        ",
 "FLine        ",
 "NMI!!!       ",
 "DEBUG!!!     "
};

//============================================================================

void
DebugMessage (void)
{
#if 0
	unsigned int *buff;
	unsigned char a;
	int b=0;

	SendCmd (GEN_MESSAGE);
	GetSlaveBus();
	outportb(slaveBank,SLAVEBANK_COM);
	buff = (unsigned int *)slaveBufferData;
	while ( a = *buff++ )
		textBuffer[b++]=a;
	ReturnSlaveBus();
	PrintToStatWindow (textBuffer);
#endif

	unsigned int *buff;
    unsigned char a=1;
    int b=0;
    SendCmd (GEN_MESSAGE);
	GetSlaveBus();
	outportb(slaveBank,SLAVEBANK_COM);
	buff = (unsigned int *)slaveBufferData;
    while (a!=0)
        {
           a = *buff++;
           textBuffer[b++]=a;
         }
	ReturnSlaveBus();
    PrintToStatWindow (textBuffer);
}

//============================================================================

void
GetMessage(void)
{
   _breakList *bPtr;

   GetAttention ();
   SendCmd(GEN_ASYNC);
   GetSlaveBus();
   RecBytes ((char *) &Exception,sizeof(_exception));
   ReturnSlaveBus();

   Exception.flavor =SwapWord (Exception.flavor);
   if(Exception.flavor == EXCP_ILLEGAL)
	{
		delay(10);
        GetRegsFromSlave(regs);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses"
		if(bPtr = FindBreak(GetReg(REG_INSTRUCTIONPOINTER)))
#pragma GCC diagnostic pop
		{
			if(bPtr->count)
				if(!(--bPtr->count))
					BreakWinClear(bPtr);
			return;
		}
	}

   if (Exception.flavor==EXCP_MESSAGE)   DebugMessage ();
   else
	{
		if(Exception.flavor < EXCP_MAX)
		 {
			PrintMessageStatus (Exception.flavor+EXCEPTION_BASE);
			PrintToStatWindow (ExceptionMessages[Exception.flavor]);
		 }
	}
   if (Exception.flavor==EXCP_ADDR)
    {
        Exception.instruction =SwapWord (Exception.instruction);
        Exception.address =SwapLong (Exception.address);
        sprintf (textBuffer,"at %0lx \n INST: %0x"
            ,Exception.address,Exception.instruction);
        PrintToStatWindow (textBuffer);
    }
    ResumeSlave();
	delay(10);
}

//==============================================================================

int
SlaveRunning(void)
{
	char temp;
	do
	 {
    	temp = inportb (slaveWorm);

	 } while((temp & STATF_BUSY) && !(temp & STATF_READY));
    slaveRunning =!( temp & STATF_READY);
	return(slaveRunning);
}


void
HandleSlaveInput(void)
{
    char a;
    a = inportb (slaveWorm);
    if (a & ASYNC_BIT) GetMessage ();
    if(slaveUpdate)
        GetRegsFromSlave(regs);
    if (slaveDead)
     {
        PrintMessageStatus(STATUS_SLAVE_DEAD);
        return;
     }
	SlaveRunning();
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
// register commands

void
GetRegsFromSlave( ULONG regs[] )
{
     long tempreg[NUMREGS];
     int a;
     if (GetAttention ()) return;
     SendCmd (GEN_REGS_GEN2PC);
     RecBytes ((char *) tempreg,sizeof(long) * NUMREGS);
     for (a=0;a<NUMREGS;a++)
        regs[a] = SwapLong (tempreg[a]) & regMaskArray[a];
    ResumeSlave ();
}

void
PutRegsToSlave( ULONG regs[] )
{
     long tempreg[NUMREGS];
     int a;
     for (a=0;a<NUMREGS;a++)
        tempreg[a] = SwapLong (GetReg(a));
     if (GetAttention ()) return;
     SendBytes ((char *) tempreg,sizeof(long) * NUMREGS);
     SendCmd (GEN_REGS_PC2GEN);
     ResumeSlave ();
}

//============================================================================
// control/misc commands


void
SetSlaveCtrlBits(void)
{
	if(slaveRunning)
		outportb(slaveCtrl,slaveRunningCtrlBits);
	else
		outportb(slaveCtrl,slaveCtrlBits);
}


void
ResumeSlave()
{
	if(genStatus)
    	SendCmd (GEN_GO);
		SetSlaveCtrlBits();
}


void
	ResetPort( void )
	{
	outportb( slaveWorm, 0 );
	}


void
	UnInitSlaveIO( void )
	{
	}


boolean
InitSlaveIO(int base, unsigned short slaveBufferSeg )
{
	slaveCtrl = base++;
	slaveBank = base++;
	slaveWorm = base;

	slaveBufferCmd = (unsigned char*)( slaveBuffer << 16 );
	slaveBufferData = slaveBufferCmd+2;
	slaveCtrlBits = DB_CTRL_NORM;
//	slaveRunningCtrlBits = DB_CTRL_NORM|DB_CTRL_WP;
	slaveRunningCtrlBits = DB_CTRL_NORM;
    ResetPort ();
    if(SlaveGetVer() == SLAVE_VER_NUMBER)
		return(boolean::TRUE);
    return(boolean::FALSE);
}

int SlaveGetVer(void)
{
    int foo;
    if (GetAttention ()) return(0);
	outportb(slaveBank,SLAVEBANK_COM);
    SendCmd (GEN_VERSION_REQ);
	GetSlaveBus();

    foo = *slaveBufferData;				// promote char to int
	ReturnSlaveBus();
    ResumeSlave ();
    return (foo);
}

boolean
CheckSlaveAlive(void)					// boolean::FALSE = alive, boolean::TRUE = dead
{
	return(slaveDead);
}


void
SlaveReset(void)
{
	outportb(slaveCtrl,slaveRunningCtrlBits ^ DB_CTRL_RESET);
	outportb(slaveCtrl,slaveRunningCtrlBits);
}

void SlaveRestart (void)
{
}

void
SlaveSetWriteProtect(void)
{
	slaveRunningCtrlBits |= DB_CTRL_WP;
	SetSlaveCtrlBits();
}

void
SlaveClearWriteProtect(void)
{
	slaveRunningCtrlBits &= ~DB_CTRL_WP;
	SetSlaveCtrlBits();
}

void
GoodByeSlave( void )
{
}

//============================================================================
// execution control

void SlaveRun (void)
{
    if (GetAttention ()) return;
    SendCmd (GEN_GO);
	SetSlaveCtrlBits();
	slaveRunning = boolean::TRUE;
}

void SlaveStop (void)
{
	if(SlaveRunning())
        GetAttention();
}

boolean SingleStep (void)
{
	if(SlaveRunning())
		return(boolean::TRUE);
    if (GetAttention()) return (boolean::FALSE);
    SendCmd (GEN_STEP);
	return(boolean::TRUE);
}

boolean StepOverBreak (long addr,uword inst)
{
	unsigned int *buff;

	buff = (unsigned int *)slaveBufferData;
    if (GetAttention()) return (boolean::FALSE);
	GetSlaveBus();
	outportb(slaveBank,SLAVEBANK_COM);
	*buff++ = SwapWord(addr >> 16);
	*buff++ = SwapWord(addr & 0xffff);
	*buff++ = SwapWord(inst);
	ReturnSlaveBus();
    SendCmd (GEN_STEP_OVER_BREAK);
    ResumeSlave();
    return(boolean::TRUE);
}

//============================================================================
// break control

boolean
SlaveBkClr(long addr, unsigned int inst)
{
	unsigned int *buff;
    unsigned char l,h;

	buff = (unsigned int *)slaveBufferData;
    if (GetAttention()) return (boolean::FALSE);
	GetSlaveBus();
	outportb(slaveBank,SLAVEBANK_COM);
	*buff++ = SwapWord(addr >> 16);
	*buff++ = SwapWord(addr & 0xffff);
	*buff++ = SwapWord(inst);
	ReturnSlaveBus();
    SendCmd (GEN_CLEAR_BREAK);
    ResumeSlave();
    return(boolean::TRUE);
}

uword
SlaveSetBkPt(ulong addr)
{
	unsigned int result;
	uword* buff;
    if (GetAttention()) return(boolean::FALSE);
	buff = (uword*)slaveBufferData;
	GetSlaveBus();
	outportb(slaveBank,SLAVEBANK_COM);
	*buff++ = SwapWord(addr >> 16);
	*buff++ = SwapWord(addr & 0xffff);
	ReturnSlaveBus();
    SendCmd (GEN_SET_BREAK);
	GetSlaveBus();
	result = SwapWord(*(uword*)slaveBufferData);
	ReturnSlaveBus();
    ResumeSlave();
    return (result);
}

//============================================================================
// memory i/o commands

void
ReadSlaveData(unsigned long addr,char *data,unsigned int len)
{
	unsigned long *buffer = (unsigned long *)slaveBufferData;
    /* SLS - fix to handle danger zones in Genesis memory */

    /* first handle the case where he starts outside the dangerzone */
    if (((addr+len) >= 0x800000 ) && ( addr < 0x800000 ) )
        len = (0x800000 - addr);

    /* two regions */
    if (((addr+len) >= 0xb00000 ) && ( addr < 0xb00000 ) )
        len = (0xb00000 - addr);

    /* now handle starting inside danger zone, and runs into safe memory */
    if (( addr >= 0xb00000 ) && ( addr+len >= 0xc00000 ) && (addr < 0xc00000 ))
    {
        len -= ( 0xc00000 - addr ) ;
        addr = 0xc00000;
        data = data + (0xc00000-addr);
    }

    /* in two places */
    if (( addr >= 0x800000) && ( addr+len >= 0xa00000 ) && (addr < 0xa00000))
    {
        len -= ( 0xa00000 -addr );
        addr = 0xa00000;
        data += ( 0xa00000 - addr );
    }

    /* now handle entirely within the danger zone */
    if ( ( addr >= 0x800000 ) && ( addr+len < 0xa00000 ))
        len = 0;

    /* other case */
    if (( addr >= 0xb00000 ) && ( addr+len < 0xc00000 ))
        len = 0;

    if (len)
    {
        if (GetAttention ()) return;
		GetSlaveBus();
		outportb(slaveBank,SLAVEBANK_COM);
		*buffer++ = SwapLong(addr);
		*buffer++ = SwapLong((unsigned long)len);
		ReturnSlaveBus();
        SendCmd (GEN_SEND_GEN2PC);
		GetSlaveBus();
		outportb(slaveBank,SLAVEBANK_COM);
        RecBytes (data,(unsigned long) len);
		ReturnSlaveBus();
        ResumeSlave ();
    }
}

void
WriteSlaveData(unsigned long addr,char *data,unsigned int len)
{
	unsigned long *buff;
    if (len)
    {
        if (GetAttention ()) return;
		GetSlaveBus();
		outportb(slaveBank,SLAVEBANK_COM);
		buff = (unsigned long *)slaveBufferData;
		*buff++ = SwapLong(addr);
		*buff++ = SwapLong((unsigned long)len);
		memcpy(buff,data,(unsigned long)len);
		ReturnSlaveBus();
        SendCmd (GEN_SEND_PC2GEN);
        ResumeSlave ();
    }
}


// for x parser
unsigned long MemRead(unsigned long addr,UBYTE size)
{
    unsigned long foo = 0;
    ReadSlaveData (addr,(char *) &foo,(unsigned int) size);
	switch (size)
	{
		case	1:
			return(foo & 0xff);
		case	2:
			return(SwapWord(foo)&0xffff);

		case	3:
			foo &= 0xffffff;
			return(SwapLong(foo));

		default:
			return(SwapLong(foo));
	}
}

//==============================================================================

errorcode
LoadFileToSlave (FILE *f, unsigned long address, unsigned long len)
{
    void *buffer;
    unsigned int bigchunks;
    unsigned int remainder;
    unsigned long a;
	errorcode error = NOERR;

    bigchunks = (len >> 15);
    remainder = len & 0x7fff;
    buffer = malloc (32768);
	if(!buffer)
		return(ERROR_NOMEM);

    if(GetAttention()) return(NOERR);			// lie
    for (a=0;a<bigchunks;a++)
     {
        fread (buffer,32768,1,f);
        WriteSlaveData (address+(a<<15),(char *) buffer,32768);
     }
    fread (buffer,remainder,1,f);
    WriteSlaveData (address+(a<<15),(char *) buffer,remainder);
    free (buffer);
	return(error);
}

//==============================================================================

errorcode SaveFileFromSlave (FILE *f, unsigned long address, unsigned long len)
{
    void *buffer;
    unsigned int bigchunks;
    unsigned int remainder;
    unsigned long a;
    bigchunks = (len >> 15);
    remainder = len & 0x7fff;
    buffer = malloc (32768);
	if(!buffer)
		return(ERROR_NOMEM);
    for (a=0;a<bigchunks;a++)
     {
        ReadSlaveData (address+(a<<15),(char *) buffer,32768);
        fwrite (buffer,32768,1,f);
		delay(10);
     }
    ReadSlaveData (address+(a<<15),(char *) buffer,remainder);
    fwrite (buffer,remainder,1,f);
    free (buffer);
	return(NOERR);
}

//============================================================================
// internal memory commands

boolean
SlaveCopyMem (long startaddr,long destaddr, long len)
{
	unsigned long *buff;
    if (len)
    {
        if (GetAttention ()) return(boolean::FALSE);
		GetSlaveBus();
		buff = (unsigned long *)slaveBufferData;
		outportb(slaveBank,SLAVEBANK_COM);

		*buff++ = SwapLong(startaddr);
		*buff++ = SwapLong(destaddr);
		*buff++ = SwapLong(len);
		ReturnSlaveBus();
        SendCmd (GEN_COPY_MEM);
        ResumeSlave ();
		return(boolean::TRUE);
    }
	return(boolean::FALSE);
}

boolean
SlaveFillMem (long startaddr,long len,long patlen,char *pattern)
{
	char *buff;
    if (len)
    {
        if (GetAttention ()) return(boolean::FALSE);
		GetSlaveBus();
		buff = (char *)slaveBufferData;
		outportb(slaveBank,SLAVEBANK_COM);
		*buff++ = SwapLong(startaddr);
		*buff++ = SwapLong(len);
		*buff++ = SwapLong(patlen);
        SendDataBytes( pattern, buff, patlen );
		ReturnSlaveBus();
        SendCmd (GEN_FILL_MEM);
        ResumeSlave ();
		return(boolean::TRUE);
    }
	return(boolean::FALSE);
}

//============================================================================
// vdp memory i/o

void ReadSlaveVDP(unsigned long addr,char *data,unsigned int len)
{
	unsigned int * buff;
    unsigned char h,l;
    if (len)
    {
        if (GetAttention ()) return;
		GetSlaveBus();
		buff = (unsigned int *)slaveBufferData;
		outportb(slaveBank,SLAVEBANK_COM);

		*buff++ = SwapWord(addr);
		*buff++ = SwapWord(len);
		ReturnSlaveBus();
        SendCmd (GEN_READ_VDP);
		GetSlaveBus();
		if(*slaveBufferCmd)
		 {
        	RecBytes (data,(unsigned long) len);
		 }
        ResumeSlave ();
    }
}

void ReadSlaveCRAM (char *data)
{
    unsigned char h,l;
        if (GetAttention ()) return;
        SendCmd (GEN_READ_CRAM);
		GetSlaveBus();
		outportb(slaveBank,SLAVEBANK_COM);
		if(*slaveBufferCmd)
        	RecBytes (data,128);
		ReturnSlaveBus();
        ResumeSlave ();
}

void ReadSlaveVSRAM (char *data)
{
        if (GetAttention ()) return;
        SendCmd (GEN_READ_VSRAM);
		GetSlaveBus();
		outportb(slaveBank,SLAVEBANK_COM);
		if(*slaveBufferCmd)
        	RecBytes(data,80);
		ReturnSlaveBus();
        ResumeSlave ();
}

void ReadSlaveZ80(unsigned long /*addr*/, char *data, unsigned int len)
{
        memset(data, 0, len);
}

//============================================================================

