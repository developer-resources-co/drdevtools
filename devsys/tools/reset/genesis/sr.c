//=============================================================================
// Genesis Restart V1.1 - (c) 1991,92 Developer Resources
// By Kevin T. Seghetti All Rights Reserved.
//=============================================================================

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

//=============================================================================

unsigned int slaveBase = 0x318;
unsigned char far *slaveBuffer = (unsigned char far *)0xd0000000;

unsigned int slaveCtrl,slaveBank,slaveWorm;
unsigned char far *slaveBufferCmd;
unsigned char far *slaveBufferData;


//------------------------------------------------------------------------------

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

typedef unsigned int word;

Cleanup(int foo)
{
	exit(foo);
}

void
main(int argc,char *argv[])
{
	printf("GR (c) 1991,92 Developer Resources\nGenesis Reseter\nBy Kevin T. Seghetti\n\n");
	LoadEnvVariables();
	InitSlaveIO(slaveBase,slaveBuffer);
	outportb(slaveCtrl,0x60);
	delay(DELAYTIME);		       // wait for sfx to notice
	outportb(slaveCtrl,0xe0);
	exit(0);
}
