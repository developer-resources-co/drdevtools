//=============================================================================
//  diss.c: 6502 dissasembler
//  By Kevin T. Seghetti, (c) 1991 Developer Resources
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
char far *inBuffer;
char far *outBuffer;

char textBuffer[120];
int	procMode = 0;
int dissMode = 0;

void Cleanup();

//=============================================================================

#include	"general.c"				// general stuff
//#include	"dis6502.c"
#include	"dis68000.c"

//=============================================================================

void
main(int argc,char *argv[])
{

	int argCount;
	unsigned long inLen,addr,counter;
	unsigned int offset;
	int cont;
	char far *buff;

	addr = 0;

	printf("Dis68 V1.0 (c) 1991 Developer Resources\n");
	if(argc < 2)
	 {
		printf("68000 file dissasembler\nBy Lars Norpchen & Kevin T. Seghetti\n\n\
Format: Dis68 [switches] <binaryinfile> [outputtextfile]\n\
Switches:\n\
	-d#  set dis mode\n\
		0 = dissassembly only\n\
		1 = addresses & dissassebly\n\
		2 = addresses,hex  & dissassebly\n\
	-a#  set starting address\n\
		# = starting address\n\
		\n");

		Cleanup(ERR_NOINPUT);
	 }

	argCount = 1;
	while(argCount < argc)
	 {
		if(*argv[argCount] == '-')				// must be command line switch
			switch(argv[argCount][1])
			 {
				case 'd':
				case 'D':
					sscanf(&argv[argCount][2],"%d",&dissMode);
					break;
				case 'a':
				case 'A':
					sscanf(&argv[argCount][2],"%lx",&addr);
					break;
			 }
		else break;
		argCount++;
	 }

	inFile = fopen(argv[argCount],"rb");
	if(inFile == 0)
	 {
		printf("File not found\n");
		Cleanup(ERR_FILENOTFOUND);
	 }

	if(argc == argCount+1)							// if only one input, output to screen
	 {
		printf("\n");
		outFile = stdout;
	 }
	else									// otherwise, write to desired output file
	 {
		outFile = fopen(argv[argCount+1],"w");
		if(outFile == 0)
	 	{
			printf("Could not open output file\n");
			Cleanup(ERR_FILENOTOPENED);
	 	}
	 }

									// find length of input file
	fseek(inFile,0,SEEK_END);
	inLen = ftell(inFile);
	fseek(inFile,0,SEEK_SET);

	if(!inLen)
	 {
		printf("boy that was a small file!\n");
		Cleanup(0);
	 }
									// get buffer for input file
	inBuffer = farmalloc(inLen);
	if(inBuffer == 0)
	 {
		printf("Out of memory\n");
		Cleanup(ERR_NOMEM);
	 }

	buff = inBuffer;
	while(!feof(inFile))
		*buff++ = getc(inFile);

	fprintf(outFile,"; 68000 dissassembly produced by dis68, (c) 1991 Developer Resources\n");

	buff = inBuffer;
	cont = 1;
	counter = inLen;
	while(counter)
	 {
		offset = Disassem(addr,buff,textBuffer,dissMode);
		buff += offset;
		addr += offset;
		if(offset > counter)
			counter = 0;
		else
			counter -= offset;
		if(dissMode == 0)
			fprintf(outFile,"\t%s\n",textBuffer);
		else
			fprintf(outFile,"%s\n",textBuffer);
		//if((char huge *)buff>=(char huge *) (inBuffer+inLen) )
		//	cont = 0;
	 }
Cleanup(0);
}

//=============================================================================

void
Cleanup(int err)
{
	if(inBuffer != 0)
		farfree(inBuffer);
	if(inFile != 0)
		fclose(inFile);
	if(outFile != 0)
		fclose(outFile);
	exit(err);
}