//============================================================================
// slaveio.hpp: slave i/o declarations, same for every platform
// (different code for each, however)
// These are the calls that must be implemented for each platform
//============================================================================

#ifdef SNES
#define SLAVE_VER_NUMBER 0x42

enum SNESCOMMANDS
	{
	SNES_NOP,						//   00
	SNES_VERSION_REQ,				//   01
	SNES_SEND_PC2SNES,			//   02
	SNES_SEND_SNES2PC,			//   03
	SNES_RESTART,					//   04
	SNES_REGS_PC2SNES,			//   05
	SNES_REGS_SNES2PC,			//   06
	SNES_GO,	   					//   07
	SNES_READ_PPU, 				//   08
	SNES_READ_OBJRAM,				//   09
	SNES_READ_CRAM,				//   0A
	SNES_WRITE_PPU,				//   0B
	SNES_WRITE_OBJRAM,			//   0C
	SNES_WRITE_CRAM,				//   0D
	SNES_SET_SPC700,				//   0E
	SNES_GET_SPC700,				//   0F
	SNES_ASYNC,						//   10
	SNES_MESSAGE,  				//   11
	SNES_RESET_SPC700,			//   12
	SNES_COPY_MEM, 				//   13
	SNES_FILL_MEM, 				//   14
	SNES_STEP,	 					//   15
	SNES_SET_BREAK,				//   16
	SNES_CLEAR_BREAK,				//   17
	SNES_STEP_OVER_BREAK,		//   18
	SNES_SEARCH,   				//   19
	SNES_OBSELETE1,				//   1A
	SNES_OBSELETE2,				//   1B
	SNES_RESTORE_VECTORS			//   1C
	};

#endif

#ifdef GENESIS
#define SLAVE_VER_NUMBER 2
#endif

#include <pclib/boolean.hpp>

boolean InitSlaveIO(int portBase, unsigned short slaveBufferSeg );		// FALSE = no slave, TRUE = ok
void UnInitSlaveIO( void );
void GoodByeSlave(void);						// in case slave needs to know monitor is quiting
void HandleSlaveInput(void);					// read registers, and receive any async messages
void GetRegsFromSlave( ULONG regs[] );		// read slave registers(called from HandleInput)
void PutRegsToSlave( ULONG regs[] );		// write regs to slave(only happens when user changes a register)
void SlaveStop (void);							// stop execution
boolean SlaveFillMem (long startaddr,long len,long patlen,char far *pattern);
boolean SlaveCopyMem (long startaddr,long destaddr, long len);
// load from disk to slave
errorcode LoadFileToSlave (FILE *f, unsigned long address, unsigned long len);
// short memory read command
unsigned long MemRead(unsigned long addr,UBYTE size);
boolean SingleStep (void);
boolean StepOverBreak (long addr,uword inst);
int SlaveRunning(void);
// short memory write command
void WriteSlaveDatum(unsigned long addr,unsigned long datum,unsigned int len);

//unsigned long SwapLong (unsigned long a);


extern "C" {

	unsigned int SwapWord (unsigned int a);
	uword SlaveSetBkPt( ulong addr );

	int SlaveGetVer(void);

	void
	SlaveSetWriteProtect(void);

	void
	SlaveClearWriteProtect(void);

	void
	SlaveSetBRKOnWrite(void);

	void
	SlaveClearBRKOnWrite(void);

	void
	SlaveReset(void);					// cause monitor to run

	void SlaveRestart(void);			// restart user code


	// regular memory read command
	void
	ReadSlaveData(unsigned long addr,char far *data,unsigned int len);

	// regular memory write command
	void
	WriteSlaveData(unsigned long addr,char far *data,unsigned int len);

	// save from slave to disk
	errorcode SaveFileFromSlave (FILE *f, unsigned long address, unsigned long len);


	void SlaveRun (void);					// continue execution

	boolean
	CheckSlaveAlive(void);					// FALSE = alive, TRUE = dead

	boolean
	SlaveBkClr(long addr, unsigned int inst);

	// special memoruy i/o commands
#ifdef GENESIS
	void ReadSlaveVDP(unsigned long addr,char far *data,unsigned int len);

	void WriteSlaveVDP(unsigned long addr,char far *data,unsigned int len);

	void ReadSlaveCRAM (char far *data);

	void WriteSlaveCRAM (char far *data);

	void ReadSlaveVSRAM (char far *data);

	void WriteSlaveVSRAM (char far *data);
#endif

#ifdef SNES
	void ReadSlavePPU(unsigned long addr,char far *data,unsigned int len);
#endif

	}

//============================================================================

