//==============================================================================
// Downloader Version 2.0 - Developer Resources
// Scott Statton (c) 1991, All Rights Reserved.
// Improved to handle files > 64K
// 18 June 1991

//=============================================================================
// gdl.C -- Binary Downloader for Genesis V2.0
// Scott Statton & Kevin T. Seghetti -- Developer Resources.
// Jeff is a weenie
//=============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <dos.h>

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

//=============================================================================

typedef unsigned int word;
void SetBank(int);

//=============================================================================

signed long
LoadHunk (FILE * fptr, word len, word subaddress, word segaddress)
{
	struct SREGS segment;

	union REGS in,out;
	int fhandl;

	fhandl = fileno(fptr);
	in.h.ah = 0x3f;  /* read from file func number */
	in.x.bx = fhandl;
	in.x.cx = len;
	in.x.dx = subaddress;

	segment.ds = segaddress; /* get that from the setup */

	int86x ( 0x21 , &in,&out, &segment);	/* BANG! */

	if (out.x.cflag)
			return ( - out.x.ax);
		else
			return ( out.x.ax );
}

//==============================================================================

void
LoadFile (FILE *f, long len, long address)
{
	word subAddress;	/* offset in 80x86 parlance */
	int bank;			/* current bank # */
	word bkOff;			/* block offset */
	word numBytes;

	do {
		bank = address >> 16;
		SetBank(bank);
		subAddress = address & 0xffff;		/* start subaddress */
		bkOff = address & 0x7fff;		/* offset into 32K block */
		numBytes = ( (bkOff + len) >= 0x8000 ? 0x8000 - bkOff : bkOff+len );
		LoadHunk ( f, numBytes, subAddress , (word)((long)slaveBuffer>>16) );
		address += numBytes;
		len -= numBytes;
	}
	while (len);
	return;
}

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

void
main(int argc,char *argv[])
{
	FILE *inFile;

	unsigned long inLen;
	unsigned long saddr;

	printf("Gdl V2.0 (c) Developer Resources 1991,92\nGenesis downloader \nBy Scott L. Statton\n");

	if(argc < 2)
	 {
		printf("Gdl inputfile <start-address>");
		Cleanup(ERR_NOINPUT);
	 }

	LoadEnvVariables();
	InitSlaveIO(slaveBase,slaveBuffer);

	inFile = fopen(argv[1],"rb");
	if(inFile == 0)
	 {
		printf("File not found\n");
		Cleanup(ERR_FILENOTFOUND);
	 }

    if ( argc == 2 )
	    saddr = 0x00;
    else
	    if(!sscanf(argv[2],"%lx",&saddr)) saddr = 0x0;

	fseek(inFile, 0L, SEEK_END);
	inLen = ftell(inFile);
	fseek(inFile, 0L, SEEK_SET );

    printf("loading file %s to address %lx\n",argv[1],saddr);

	outportb (slaveCtrl,0xf3);
	LoadFile( inFile, inLen, saddr);
	outportb (slaveCtrl,0xf0);
	fclose ( inFile);
	exit(0);
}

//==============================================================================
