//==============================================================================
// flipsign.c: convert midi file format v1.0 to Developer Resources song format
//==============================================================================

//==============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int MAXLINELENGTH = 20;

FILE *inFile;
FILE *outFile;

enum
{
	FALSE,TRUE
};

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



main (int argc,char *argv[])
{
    long a;
    int linesize=0;
	unsigned int charCount = 0;
	int argCount,firstLine;
    unsigned char buffer;
	unsigned char name[20];
	unsigned char ch;

	argCount = 1;

	if(argc-argCount < 2)
        {
            printf ("\
FlipSign V1.0 (c) Developer Resources 1992\n\
By Kevin T. Seghetti\n\
Format: FlipSign [<switches>] <midiinfile> <sourceoutfile>\n\
Switches:\n\
		-n: none so far\n");
            exit (1);
        }
    inFile = fopen (argv[0+argCount],"rb");
    if (inFile==NULL)
        {
            printf ("Error opening input file:%s",argv[1]);
            exit (1);
        }
	strcpy(name,argv[0+argCount]);
	*FindChar(name,'.') = 0;
    outFile = fopen (argv[1+argCount],"wb");
    if (outFile==NULL)
     {
    	printf ("Error opening output file:%s",argv[2]);
    	exit (1);
     }
	ch = fgetc(inFile);
    while (!feof (inFile))
     {
	 	fputc(ch^0x80,outFile);
		ch = fgetc(inFile);
     }

    fclose (inFile);
    fclose (outFile);
	exit(0);
}

//==============================================================================
