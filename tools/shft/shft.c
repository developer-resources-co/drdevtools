//==============================================================================
// shift.c: shift bytes sighed
// By Kevin T. Seghetti
// (c) 1991,92 Developer Resources
//==============================================================================

//==============================================================================

#include "stdio.h"

FILE *infp;
FILE *outfp;

//==============================================================================

#define	PROGNAME SHIFT

enum
{
	FALSE,TRUE
};

#define FLAG unsigned char

enum
{
	LEFT,RIGHT
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

main (int argc,char *argv[])
{
    long a;
	unsigned int charCount = 0;
	int argCount;
    unsigned char buffer;
	unsigned char name[20];
	char ch;
	FLAG shiftDir = LEFT;
	unsigned int shiftVal = 0;

	argCount = 1;
	while(argCount < argc)
	 {
		if(*argv[argCount] == '-')				// must be command line switch
			switch(argv[argCount][1])
			 {
				case 'L':
				case 'l':
					sscanf(&argv[argCount][2],"%u",&shiftVal);
					shiftDir = LEFT;
					break;
				case 'r':
				case 'R':
					sscanf(&argv[argCount][2],"%u",&shiftVal);
					shiftDir = RIGHT;
					break;
			 }
		else break;
		argCount++;
	 }

	if(argc-argCount < 2)
        {
            printf ("\
Shift V1.0 (c) Developer Resources 1992\n\
Shift\n\
By Kevin T. Seghetti\n\
Format: Shift [<switches>] <infile> <outfile>\n\
Switches:\n\
		-l#: shift left # of bits\n\
		-r#: shift right # of bits\n");
            exit (1);
        }

    infp = fopen (argv[0+argCount],"rb");
    if (infp==NULL)
        {
            printf ("Error opening input file:%s",argv[0+argCount]);
            exit (1);
        }
	strcpy(name,argv[0+argCount]);
	*FindChar(name,'.') = 0;
    outfp = fopen (argv[1+argCount],"wb");
    if (outfp==NULL)
        {
            printf ("Error opening output file:%s",argv[1+argCount]);
            exit (1);
        }

	ch = getc(infp);
    while (!feof (infp))
     {
		switch(shiftDir)
		 {
			case LEFT:
				ch <<= shiftVal;
				break;
			case RIGHT:
				ch >>= shiftVal;
				break;
		 }
		putc(ch,outfp);
		ch = getc(infp);
     }

    fclose (infp);
    fclose (outfp);
    printf ("\ndone.");
	exit(0);
}

//==============================================================================
