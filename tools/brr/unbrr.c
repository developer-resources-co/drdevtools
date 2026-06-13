//=============================================================================
// unbrr.c -- decompress brr files into 8-bit samples
// by Kevin T. Seghetti -- Developer Resources.
// Jeff is a weenie
//=============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <dos.h>

#define TRUE 1
#define FALSE 0

//=============================================================================

#define ERR_NOINPUT 5
#define ERR_FILENOTFOUND 5
#define ERR_FILENOTOPENED 5
#define ERR_NOMEM 5

#define FLAG unsigned char

FLAG sixteenBit = FALSE;				// put this on a switch

//=============================================================================

typedef unsigned int word;

void
Cleanup(int foo)
{
	exit(foo);
}

//=============================================================================

char brrBuffer[16];			// buffer containing brr delta data
unsigned char brrByte = 0;					// brr header byte
int outBuffer[16];			// buffer containing raw sample output

int prevSample,prev2Sample;	// previous 2 samples(for filters)

//=============================================================================
// read binary sample into buffer from disk

void
WriteSoundChunk(FILE *file,int *bPtr)
{

	int value;
	int count = 16;
	while(count--)
	 {
		if(sixteenBit)
		 {
			fputc(*bPtr>>8,file);
			fputc(*bPtr,file);
		 }
		else
		 {
			fputc(*bPtr>>8,file);
		 }
		bPtr++;
	 }
}

//==============================================================================

#include "unbrr.inc"

//==============================================================================
// write out one BRR record

void
ReadBrrChunk(FILE *file,char *oPtr)
{
	unsigned int i,t;
	brrByte = fgetc(file);
	for(i=0;i<8;i++)
	 {
		t = fgetc(file);
		*oPtr++ = (t>>4) & 0xf;
		*oPtr++ = t & 0xf;
	 }
}

//=============================================================================

void
main(int argc,char *argv[])
{
	FILE *inFile,*oFile;
	char oFname[12];
	char *s;
	char *t;

	unsigned int chunkCount,chunkNum;

	unsigned long chunkSize,remainder;
	unsigned long saddr;

	if(argc < 3)
	{
		printf("UnBrr V1.00 (c) 1992 Developer Resources\nBrr decompressor\nBy Kevin T Seghetti\n\nFormat: unbrr <inputfile> {<outputfile>}\n");
		Cleanup(ERR_NOINPUT);
	}

	inFile = fopen(argv[1],"rb");
	if(inFile == 0)
	{
		printf("File %s not found\n",argv[1]);
		Cleanup(ERR_FILENOTFOUND);
	}

	oFile = fopen(argv[2], "wb");
	if ( !oFile )
	 {
			printf("\nCouldn't open output file %s\n",argv[2]);
			exit(1);
	 }

	prev = prev2 = 0;
	while(!feof(inFile) && !(brrByte&1))
	 {
		ReadBrrChunk(inFile,brrBuffer);
		UnDoBrr(brrBuffer,outBuffer,brrByte);
		WriteSoundChunk(oFile,outBuffer);
	 }
	fclose(inFile);
	fclose(oFile);

	printf("\nFile decompression complete.\n");
	exit(0);
}

//=============================================================================
