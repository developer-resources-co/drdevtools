//=============================================================================
// chunk.c -- break a file into chunks
// by Kevin T. Seghetti & Scott Statton -- Developer Resources.
// Jeff is a weenie
//=============================================================================

#include <stdio.h>
#include <alloc.h>
#include <dos.h>

//=============================================================================

#define ERR_NOINPUT 5
#define ERR_FILENOTFOUND 5
#define ERR_FILENOTOPENED 5
#define ERR_NOMEM 5

//=============================================================================

typedef unsigned int word;

Cleanup(int foo)
{
	exit(foo);
}

//=============================================================================

WriteChunk(FILE *inFile, char *outFileName,int chunkNum,unsigned long count)
{
	FILE *oFile;
	oFile = fopen(outFileName, "wb");
	if ( !oFile )
	 {
			printf("\nCouldn't open output file %s\n",outFileName);
			exit(1);
	 }
	while(!feof(inFile) && count--)
	 {
		fputc(fgetc(inFile),oFile);
	 }
	close(oFile);
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

	unsigned long inLen,chunkSize,remainder;
	unsigned long saddr;

	if(argc < 3)
	{
		printf("Chunk V1.00 (c) 1991 Developer Resources\nFile Cutter\nBy Kevin T Seghetti\n\nFormat: Chunk <# of files to break into> <inputfile>\n");
		Cleanup(ERR_NOINPUT);
	}

	inFile = fopen(argv[2],"rb");
	if(inFile == 0)
	{
		printf("File %s not found\n",argv[2]);
		Cleanup(ERR_FILENOTFOUND);
	}

	sscanf(argv[1],"%u",&chunkCount);

	fseek(inFile,0,2);						// go to end of file
	inLen = ftell(inFile);							// find out how long file is
	fseek(inFile,0,0);						// go back to begining of file

	chunkSize = inLen/chunkCount;
	remainder = inLen%chunkCount;
	chunkNum = 0;

	while(chunkCount > chunkNum)
	 {
		t = oFname;
		s = argv[2];
		for ( ; *s; *t++ = *s++ );
		t--;
		*t++ = chunkNum+'1';
		*t = 0;
		WriteChunk(inFile,oFname,chunkNum++,chunkSize+remainder);
		remainder = 0;
	 }

	printf("\nFile chunking complete.\n");
	exit(0);
}

//=============================================================================
