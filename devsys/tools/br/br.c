//==============================================================================
// BR.c: Boot Genesis Rom Version 2.0 - Developer Resources
// Kevin T. Seghetti (c) 1991,92  All Rights Reserved.
//==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <dos.h>

#define DELAYTIME 20

//=============================================================================

#define ERR_NOINPUT 5
#define ERR_FILENOTFOUND 5
#define ERR_FILENOTOPENED 5
#define ERR_NOMEM 5

#define TRUE 1
#define FALSE 0
#define FLAG unsigned int

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

//=============================================================================

unsigned int slaveBase = 0x318;
unsigned char far *slaveBuffer = (unsigned char far *)0xd0000000;

unsigned int slaveCtrl,slaveBank,slaveWorm;
unsigned char far *slaveBufferCmd;
unsigned char far *slaveBufferData;

//=============================================================================

typedef unsigned int word;
void SetBank(int);

#include	"bootrom.src"

//=============================================================================

void
LoadEnvVariables(void)
{
	char *strPtr;
	int temp;
	strPtr = getenv("DRGENESISPORT");
	if(*strPtr)
	 {
		if(sscanf(strPtr,"%x",&temp))
			slaveBase = temp;
	 }

	strPtr = getenv("DRGENESISMEMBUFFER");
	if(*strPtr)
	 {
		if(sscanf(strPtr,"%x",&temp))
			slaveBuffer = (unsigned char far *)((long)temp << 16);
	 }
}

//==============================================================================

void
InitSlaveIO(int base,unsigned char far *slaveBuffer)
{
	slaveCtrl = base++;
	slaveBank = base++;
	slaveWorm = base;
	slaveBufferCmd = slaveBuffer;
	slaveBufferData = slaveBufferCmd+2;
}

//=============================================================================

void
SetBank ( int b)
{
	b = ((( b&1) <<3 ) | ((b&0xe)>>1));
	outportb ( slaveBank, b);
	return;
}

void
Cleanup(int err)
{
	exit (err);
}


//==============================================================================

void
main(int argc,char *argv[])
{
	FILE *inFile;
	unsigned char far *buffPtr;
	int i;

	unsigned long inLen;
	unsigned long saddr;

	printf("BR V2.0 (c) Developer Resources 1991,92\nDR Dopple Board(tm) Boot Rom\nBy Kevin T. Seghetti\n");

	LoadEnvVariables();
	InitSlaveIO(slaveBase,slaveBuffer);

	SetBank(0);
	outportb(slaveCtrl,DB_CTRL_HOSTBUS);

							// copy boot rom into genesis memory
	buffPtr = slaveBuffer;
	for(i=0;i<776;i++)
		*buffPtr++ = bootRom[i];

						// reset genesis
	outportb(slaveCtrl,0x60);
	delay(DELAYTIME);		       // wait for sfx to notice
	outportb(slaveCtrl,0xe0);

	exit(0);
}

//==============================================================================
