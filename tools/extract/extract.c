//==============================================================================
// xtract.c: xtract a section of a binary file
// By Kevin T. Seghetti
// (c) 1992 Developer Resources
//==============================================================================

//==============================================================================

#include "stdio.h"

FILE *inFile;
FILE *outFile;

//==============================================================================

#define	PROGNAME Extract

enum
{
	FALSE,TRUE
};

//==============================================================================

unsigned char *
FindChar(char *sourceString,unsigned char c)
{
	while(*sourceString)
	 {
		if(*sourceString == c)
			break;
		sourceString++;
	 }
	return(sourceString);
}

//==============================================================================

int
main (int argc,char *argv[])
{
    long a;
	unsigned int charCount = 0;
	int argCount;
	long count,len;
	unsigned char c;
    unsigned char buffer;
	unsigned char name[20];

	argCount = 1;
	while(argCount < argc)
	 {
		if(*argv[argCount] == '-')				// must be command line switch
			switch(argv[argCount][1])
			 {
				case 'c':
					break;
			 }
		else break;
		argCount++;
	 }

	if(argc-argCount < 4)
        {
            printf ("\
Extract V1.0 (c) Developer Resources 1992\n\
Extract a section of a binary\n\
By Kevin T. Seghetti\n\
Format: Xtract [<switches>] <infile> <outFile> <offset> <count>\n\
Switches:\n\
		-n: none so far");
            exit (1);
        }

    inFile = fopen (argv[0+argCount],"rb");
    if (inFile==NULL)
        {
            printf ("Error opening input file:%s",argv[1]);
            exit (1);
        }
	strcpy(name,argv[1+argCount]);
    outFile = fopen (argv[1+argCount],"wb");
    if (outFile==NULL)
        {
            printf ("Error opening output file:%s",argv[2]);
            exit (1);
        }

	sscanf(argv[2+argCount],"%ld",&count);
	sscanf(argv[3+argCount],"%ld",&len);

	fseek(inFile,count,0);
	c = fgetc(inFile);
	while(!feof(inFile) && len--)
	 {
		fputc(c,outFile);
		c = fgetc(inFile);
	 }

    fclose (inFile);
    fclose (outFile);
    printf ("\ndone.");
	return(0);
}

//==============================================================================
