//============================================================================
//	sbr.c: sfx boot rom program
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

#define RBB_CONTR_NMI 6
#define RBF_CONTR_NMI 1<<RBB_CONTR_NMI

// enable card's ram into pc bus
#define RBB_CONTR_CARDENABLE 5
#define RBF_CONTR_CARDENABLE 1<<RBB_CONTR_CARDENABLE

//#define RBB_CONTR_SFXPAUSE 6
//#define RBF_CONTR_SFXPAUSE 1<<RBB_CONTR_SFXPAUSE

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

int slavePortBase;

#define BOOTCOUNT 8

unsigned char bootTbl[BOOTCOUNT] =
{
/*
	clc
	xce
	jml	$208001
*/
	0x18,0xfb,0x5c,0x01,0x80,0x20,0xf6,0xff
};


void
main(void)
{
	int i,counter;
	unsigned char far *slaveBuffer = 0xd0000000;
	slavePortBase = 0x304;

	printf("\
SBR V1.2 (c) 1991,92 Developer Resources\n\
");

	outportb(RBOARD_COM,0);
	outportb(RBOARD_CONTR,(char)RBF_CONTR_CARDENABLE|RBF_CONTR_NMI|RBF_CONTR_WHO);

	outportb(RBOARD_PAGE,0);
	slaveBuffer += 0x7ff6;			// offset to end of 1st bank
	for(i=0;i<BOOTCOUNT;i++)
		*slaveBuffer++ = bootTbl[i];

	delay(100);			// wait for sfx to notice
	outportb(RBOARD_CONTR,(char)RBF_CONTR_RESET|RBF_CONTR_NMI);

	counter = 30;
	while(--counter && (inportb(RBOARD_COM) != 0xa3))
	 {
		delay(10);
	 }
	if(!counter)
		printf("Slave Dead\n");

}

//==============================================================================
