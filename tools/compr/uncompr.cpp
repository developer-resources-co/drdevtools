//=============================================================================
// uncmpr.c -- character uncompressor
// uses rle
// by Kevin T. Seghetti -- (c) 1992 Developer Resources.
// Jeff is a weenie
//=============================================================================
// rle format:
// 1xxx xxxx,n	= repeat n x times
// 0xxx xxxx	= there are x unique bytes following
// 00  = run finished
//==============================================================================
//	char file format:
/*
struct hdr
{
	UBYTE compType;
	UWORD length;
}
 followed by the char data(in whatever compression is specified)

enum
{
	compType_none,
	compType_RLE
}

*/
 //==============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <alloc.h>
#include <dos.h>

//=============================================================================

#define ERR_NOINPUT 5
#define ERR_FILENOTFOUND 5
#define ERR_FILENOTOPENED 5
#define ERR_NOMEM 5

//==============================================================================

unsigned int modulo = 1;
unsigned char far *dPtr;

//=============================================================================

typedef unsigned int word;

Cleanup(int foo)
{
	exit(foo);
}

//=============================================================================

unsigned char far *
WritePass(unsigned char far *buffer,unsigned char far *destBuffer,int addend)
{
	int count,cmdByte,dataByte;
	unsigned char far *destPtr;

	while(1)
	 {
		cmdByte = *buffer++;
		if(cmdByte == 0)
		 {
			dPtr = destBuffer - addend;
			return(buffer);
		 }
		if(cmdByte < 0x80)
		 {
			while(cmdByte--)
			 {
				*destBuffer = *buffer++;
				destBuffer += addend;
			 }
		 }
		else
		 {
			cmdByte &= 0x7f;
			dataByte = *buffer++;
			while(cmdByte--)
		 	{
				*destBuffer = dataByte;
				destBuffer += addend;
		 	}
		 }
	 }
}

//==============================================================================

void
WriteUnCompressedData(unsigned char far *buffer,unsigned char far *destBuffer)
{
	int i;
	for(i=0;i<modulo;i++)
		buffer = WritePass(buffer,destBuffer+i,modulo);
	return;
}

//==============================================================================

void
main(int argc,char *argv[])
{
	unsigned char far *charBuffer;
	unsigned char far *destBuffer;
	unsigned long outLen;
	FILE *inFile,*outFile;
//	char oFname[12];
//	char *s;
//	char *t;
	int param;

	unsigned char far *charPtr;
	unsigned long inLen,chunkSize,remainder;

	param = 0;

	while(*argv[param+1] == '-')
	 {
		switch(*(argv[param+1]+1))
		 {
			case 'm':
			case 'M':
				modulo = *(argv[param+1]+2) - '0';
				if(modulo == 0 || modulo > 9)
				 {
					printf("Cannot have a modulo of %d\nResetting to 1",modulo);
					modulo = 1;
				 }
				break;
			default:
				break;
		 }
		param++;
	 }


	if(argc <  param + 3)
	{
		printf("\
UnCompr V1.00 (c) 1992 Developer Resources\n\
Char File Decompressor\n\
By Kevin T Seghetti\n\n\
Format: UnCompr [switches] <inputfile> <outputfile>\n\
Switches:\n\
	   -m#: Set Modulo used\n\
	            Default = 1\n\
	            SNES recommend 4, Genesis recommend 1\n\
\n");
		Cleanup(ERR_NOINPUT);
	}

	inFile = fopen(argv[param+1],"rb");
	if(inFile == 0)
	{
		printf("Error: File %s not found\n",argv[param+1]);
		Cleanup(ERR_FILENOTFOUND);
	}

	outFile = fopen(argv[param+2],"wb");
	if(outFile == 0)
	{
		printf("Error: Cannot open file %s\n",argv[param+2]);
		Cleanup(ERR_FILENOTFOUND);
	}

	fseek(inFile,0,2);						// go to end of file
	inLen = ftell(inFile);							// find out how long file is
	fseek(inFile,0,0);						// go back to begining of file

	charPtr = charBuffer = (unsigned char far *)farmalloc(inLen);
	if(!charPtr)
	 {
		printf("Error: Out of Memory\n");
		Cleanup(ERR_NOMEM);
	 }

	destBuffer = (unsigned char far*)farmalloc(0xf000);
	if(!destBuffer)
	 {
		printf("Error: Out of Memory\n");
		Cleanup(ERR_NOMEM);
	 }

	printf("Reading compressed char file...\n");

	while((!feof(inFile)))
		*charPtr++ = getc(inFile);
	fclose(inFile);

	printf("\nWriting uncompressed file...\n");
	WriteUnCompressedData(charBuffer,destBuffer);
	outLen = (dPtr-destBuffer)+1;
	charPtr = destBuffer;
	while(outLen--)
		putc(*charPtr++,outFile);
	fclose(outFile);
	farfree(destBuffer);
	farfree(charBuffer);
	printf("\nDecompression complete.\n");
	exit(0);
}

//=============================================================================

