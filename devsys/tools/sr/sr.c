//============================================================================
//	sr.c: sfx reset program
//============================================================================

// hardware addresses and ports

							// port addresses
#define BOARD_CTRL (slavePortBase+0)
#define BOARD_BANK  (slavePortBase+1)
#define BOARD_WORM   (slavePortBase+2)
#define BOARD_WINDOW (((unsigned long)slaveWindowBase) << 28)

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
#define BOARD_COMMAND 0x01
#define BOARD_DATA		BOARD_COMMAND+1


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

void
NukeMon(void)
{
	unsigned char far *baddr;
	unsigned int delayCount;

	// check and see if rom is running
	if(inportb(BOARD_WORM) == 0XA3)
	 {
		// rom appears to be running, lets tell it to nuke wormhole
		outportb(BOARD_CTRL,0x6c);
		outportb(BOARD_BANK,0xf);			// go to comm page
		baddr = 0xd000d100;
		*baddr = 5;							// run command
		outportb(BOARD_CTRL,0x68);
		outportb(BOARD_WORM,2);
		delayCount = 0xfffe;
		while((inportb(BOARD_WORM) & 1) && delayCount--);
		outportb(BOARD_WORM,0);
		if(delayCount ==  0xffff)
		 {
			printf("Slave Dead\n");
			return;
		 }

		delayCount = 0xfffe;
		while(!(inportb(BOARD_WORM) & 1) && delayCount--);
		if(delayCount ==  0xffff)
			printf("Slave Dead\n");
     }
}




void
main(void)
{
	slavePortBase = 0x304;

	NukeMon();

	outportb(BOARD_CTRL,
		(char)RBF_CONTR_NMI);
	delay(100);			// wait for sfx to notice
	outportb(BOARD_CTRL,
		(char)RBF_CONTR_RESET|RBF_CONTR_NMI);

	outportb(BOARD_WORM,0);


}
