//============================================================================
//	slaveio.h: sfx devel board stuff
//============================================================================

// hardware addresses and ports

							// port addresses
#define RBOARD_CONTR (slavePortBase+0)
#define RBOARD_PAGE  (slavePortBase+1)
#define RBOARD_COM   (slavePortBase+2)
#define RBOARD_WINDOW (((unsigned long)slaveWindowBase) << 28)

#define RBB_CONTR_WRPROTECT 0
#define RBF_CONTR_WRPROTECT 1<<RBB_CONTR_WRPROTECT
#define RBB_CONTR_SHADOW 1
#define RBF_CONTR_SHADOW 1<<RBB_CONTR_SHADOW
// who owns bus, 0 = sfx, 1 = pc
#define RBB_CONTR_WHO 2
#define RBF_CONTR_WHO 1<<RBB_CONTR_WHO

#define RBB_CONTR_RESET 3
#define RBF_CONTR_RESET 1<<RBB_CONTR_RESET

#define RBB_CONTR_NMI 4
#define RBF_CONTR_NMI 1<<RBB_CONTR_NMI

// enable card's ram into pc bus
#define RBB_CONTR_CARDENABLE 5
#define RBF_CONTR_CARDENABLE 1<<RBB_CONTR_CARDENABLE

#define RBB_CONTR_SFXPAUSE 6
#define RBF_CONTR_SFXPAUSE 1<<RBB_CONTR_SFXPAUSE

//============================================================================
// software handshaking with sfx


					// for now, final put at end of memory
#define RBOARD_COMMAND 0x01
#define RBOARD_DATA		RBOARD_COMMAND+1


// PROT = protocol
	// outgoing bits


	 // bus arbitration
#define PROTB_WANTBUS	0
#define PROTF_WANTBUS	1<<PROTB_WANTBUS

	// command passing(note: will not read command until bus freed
#define PROTB_COMWAITING 1
#define PROTF_COMWAITING 1<<PROTB_COMWAITING

	// command passing(note: will not read command until bus freed
#define PROTB_SFXCOMRECEIVED 1
#define PROTF_SFXCOMRECEIVED 1<<PROTB_SFXCOMRECIEVED

//============================================================================
// incoming bits

	// bus arbitration
#define PROTB_BUSFREE	0
#define PROTF_BUSFREE   1<<PROTB_BUSFREE
					// 0 = bus not available, 1 = bus free

#define PROTB_COMRECEIVED 1
	 // command passing
#define PROTF_COMRECEIVED 1<< PROTB_COMRECEIVED

#define PROTB_SFXCOMWAITING 2
#define PROTF_SFXCOMWAITING 1<<PROTB_SFXCOMWAITING

#define PROTB_SFXRUNNING 7
#define PROTF_SFXRUNNING 1<<PROTB_SFXRUNNING


	// sfxrom commands
#define SL_CMD_GETVER	0
#define SL_CMD_GETREGS	1
#define SL_CMD_SETREGS	2
#define SL_CMD_GETMEM	3
#define SL_CMD_SETMEM	4
#define SL_CMD_RUN		5
#define SL_CMD_BREAK	6

#define SFX_CMD_DEBUG	0
#define SFX_CMD_BREAK	1

//============================================================================

FLAG
InitSlaveIO(void);

void
SlaveReset(void);

void
HandleSlaveInput(void);

void
SetBoardAddr(unsigned long addr);

void
ReleaseSlave(void);

void
LockSlave(void);

void
LockBus(void);

void
ReleaseBus(void);

void
SendSlaveCommand(unsigned char command);

void
ReadSlaveData(unsigned long addr,char far *data,unsigned int len);

unsigned long MemRead(unsigned long addr,UBYTE size);

void
WriteSlaveData(unsigned long addr,char far *data,unsigned int len);

void
LoadFileToSlave (FILE *f, unsigned long address, unsigned long len);

void
GoodByeSlave(void);

void
GetRegsFromSlave( ULONG regs[] );

void
PutRegsToSlave(void);

//============================================================================
