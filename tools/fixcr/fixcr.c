//=============================================================================
//  fixcr.c: convert line-feeds to carrige returns
//  By Kevin T. Seghetti, (c) 1991,93 Developer Resources
//=============================================================================

#include <stdio.h>
#include <alloc.h>

//=============================================================================

#define ERR_NOINPUT 5
#define ERR_FILENOTFOUND 5
#define ERR_FILENOTOPENED 5
#define ERR_NOMEM 5


#define ULONG unsigned long
#define UWORD unsigned int
#define UBYTE unsigned char

//=============================================================================

FILE *inFile,*outFile;

char textBuffer[100];

void Cleanup();

//=============================================================================

void
main(int argc,char *argv[])
{
	unsigned long inLen,addr;
	unsigned int offset;
	int cont;
	unsigned char ch,lastchar;
	char far *buff;

	if(argc < 2)
	 {
		printf("fixcr V1.1 (c) Developer Resources 1991,93 By Kevin T. Seghetti\nfixcr inputfile <outputfile>\n");
		Cleanup(ERR_NOINPUT);
	 }

	inFile = fopen(argv[1],"rb");
	if(inFile == 0)
	 {
		printf("File not found\n");
		Cleanup(ERR_FILENOTFOUND);
	 }

	if(argc == 2)							// if only one input, output to screen
	 {
		printf("\n");
		outFile = stdout;
	 }
	else									// otherwise, write to desired output file
	 {
		outFile = fopen(argv[2],"wb");
		if(outFile == 0)
	 	{
			printf("Could not open output file\n");
			Cleanup(ERR_FILENOTOPENED);
	 	}
	 }

	ch = getc(inFile);
	lastchar = 0;
	while(!feof(inFile))
	 {
		if(ch == 10 && lastchar != 13)
			putc(13,outFile);

		if(lastchar == 13 && ch != 10)
			putc(10,outFile);

			//ch = 13;					// convert line-feeds to carrige returns
		lastchar = ch;
		putc(ch,outFile);
		ch = getc(inFile);
	 }
Cleanup(0);
}

//=============================================================================

void
Cleanup(int err)
{
	if(inFile != 0)
		fclose(inFile);
	if(outFile != 0)
		fclose(outFile);

	exit(err);
}

//=============================================================================
