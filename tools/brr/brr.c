//=============================================================================
// brr.c -- compress 8 bit sound files into sony brr format
// by Kevin T. Seghetti -- Developer Resources.
// Jeff is a weenie
// input is assumed to be signed
// Note: if sound is to be looped, original sample file should be
// a multiple of 16 bytes long(32 if 16-bit samples)
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

int soundBuffer[16];			// buffer containing raw sample
unsigned char brrByte;					// brr header byte
char outBuffer[16];			// buffer containing brr delta data

int prevSample,prev2Sample;	// previous 2 samples(for filters)

//=============================================================================
// read binary sample into buffer from disk

void
ReadSoundChunk(FILE *inFile,int *bPtr)
{

	int value;
	int count = 16;
	while(count--)
	 {
		if(sixteenBit)
		 {
			value = (unsigned char)fgetc(inFile);
			value |= fgetc(inFile) << 8;
			*bPtr = value;
		 }
		else
		 {
			value = (int)(char)fgetc(inFile);			// sign extend
			*bPtr = value<<8;
		 }
		if(feof(inFile))
			*bPtr = 0;
		bPtr++;
	 }
}

//==============================================================================

int bitTbl[16] =
{
	0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,
	0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000
};

//------------------------------------------------------------------------------
// find MSB that is set

int DiffBit(int sample)
{
	int i;
	int topBit = (sample >> 15) & 1;
	for(i=14;i>0;i--)
	 {
		if(!!(sample & bitTbl[i]) != topBit)
				return(i+1);
	 }
	return(i);
}

//==============================================================================

#include "unbrr.inc"

//==============================================================================
// given an 8-bit binary sample, compress into sony BRR format
// note: the filters use globals to remember the previous 2 samples

int cPrev,cPrev2; 			// these are the previous 2 samples, as they actually appear in the decompressed data

void
DoBrr(int iPtr[],char oPtr[16],unsigned char *brrHdr,int compType)
{
	int i;                  // iteration counter through block(0-15)
	int temp;				// temp holder of db
	int db;					// msb with data in it for this block
	int current;			// the current output sample
	int desiredSample;		// temp storage for input sample
	int filtOut;			// filter output for this sample
	long a,b;				// filter values
	long ax,bx; 			// filters after applying previous samples

	switch(compType)
	 {
		// filter 0 comrpess
		case 0:
			a = 0;					// no filtration
			b = 0;
			break;
		case 1:
			a = 61440;				// .9375 * 65536
			b = 0;					// first order only
			break;
		case 2:
			a = 124928;				// 1.90625 * 65536
			b = -61440;				// -.9375 * 65536
			break;
		case 3:
			a = 117760;				// 1.796875 * 65536
			b = -53248;				// -0.8125 * 65536
			break;
	 }
		// first find out the msb with a 1
		// note this only works correctly for compType 0, need some magic
		// here to determine the correct shift values for the other types
	db = 0;
	for(i=0;i<16;i++)
	 {
		desiredSample = iPtr[i];
		temp = DiffBit(desiredSample);
		if(temp > db)
			db = temp;
	 }
			// cannot have a msb less than 3
	if(db < 3)
		db = 3;

	// convert from msb to lsb of sample
	db = db-3;

	// convert the data
	for(i=0;i<16;i++)
	 {
		desiredSample = iPtr[i]>>db;

			// now apply filter
		ax = a*(prev<<16);			   
		bx = b*(prev2<<16);			   
		filtOut = (ax+bx)>>16; 					// calc output of filter
		current = desiredSample-(filtOut>>db);
	 	oPtr[i] = current;

	 	prev2 = prev;
	 	prev = (current<<db)+filtOut;				// generate actual output of uncompressed data
	 }
	*brrHdr = (db << 4) | (compType << 2);
}

//==============================================================================

BrrCompress(int iPtr[],char oPtr[],FLAG endBlock)
{
	int ct;							// compression type(0-3)
	int i;							// temp counter
	long t;	   						// temp delta
	long delta[4];
	char oBuff[4][16];			// buffer containing brr delta data
	int sBuff[4][16];			// buffer containing decompressed data
	unsigned char brrHdr[4];  				// buffer containing brr header bytes

								// run all 4 compression types
	for(ct=0;ct<4;ct++)
	 {
								// compress with each type
		DoBrr(iPtr,&oBuff[ct],&brrHdr[ct],ct);
								// decompress to find how close this compression type got
		UnDoBrr(&oBuff[ct],&sBuff[ct],brrHdr[ct]);
								// simply add up the deltas between actual and compressed data
		for(i=0,t=0;i<16;i++)
			t += abs(iPtr[i]-sBuff[ct][i]);
		delta[ct] = t;
	 }

	ct = 0;
	t = delta[0];
	// now choose best compression type for this block
	// whichever one has the smallest delta wins
	for(i=1;i<4;i++)
	 {
		if(delta[i] < t)
		 {
			ct = i;
			t = delta[i];
		 }
	 }

	brrByte = brrHdr[ct];
	brrByte |= ct << 2;				// encode compression type
	brrByte |= (!!endBlock);
								// copy the compression type that worked the best
	for(i=0;i<16;i++)
		oPtr[i] = oBuff[ct][i];
}

//==============================================================================
// write out one BRR record

void
WriteBrrChunk(FILE *oFile,char *oPtr)
{
	unsigned int i,t1,t2;
	fputc(brrByte,oFile);
	for(i=0;i<8;i++)
	 {
		t1 = *oPtr++ & 0xf;
		t2 = *oPtr++ & 0xf;
		fputc((t1<<4) | t2,oFile);
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
		printf("Brr V1.00 (c) 1992 Developer Resources\n8 bit sound compressor\nBy Kevin T Seghetti\n\nFormat: brr <inputfile> {<outputfile>}\n");
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

				// clear out compression and decompression filters
	prev = prev2 = cPrev = cPrev2 = 0;
	while(!feof(inFile))
	 {
		ReadSoundChunk(inFile,soundBuffer);
		BrrCompress(soundBuffer,outBuffer,feof(inFile));
		WriteBrrChunk(oFile,outBuffer);
	 }
	fclose(inFile);
	fclose(oFile);

	printf("\nFile compression complete.\n");
	exit(0);
}

//=============================================================================
