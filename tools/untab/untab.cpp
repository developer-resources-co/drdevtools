//==============================================================================
// untab.cpp: remove tabs fro a text file(assumes tabstop = 8)
// By Kevin T. Seghetti
// (c) 1991,92 Adept Creations
//==============================================================================

//==============================================================================

#include <iostream.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dir.h>

#include "local.h"
#include "version.h"

FILE *fpIn;
FILE *fpOut;

//==============================================================================

const char szAppName[] = "UnTab";

typedef enum
	{
	FALSE, TRUE
	} BOOL;

//==============================================================================

// !!! this is equal to strchr( string, char )
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
	unsigned char ch;

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

	if(argc-argCount < 2)
		{
		char szProgName[_MAX_FNAME];

		_fnsplit( argv[0], NULL, NULL, szProgName, NULL );
		strlwr( szProgName );

		cout << szAppName << " V" << szVersion <<
" (c) 1992 Adept Creations.  All Rights Reserved.\n"
"Removes tabs from text files\n"
"By Kevin T. Seghetti\n"
"Format: " << szProgName << " [<switches>] <infile> <outfile>\n"
"Switches:\n"
"		-n: none so far\n";
            exit (1);
        }

    fpIn = fopen (argv[0+argCount],"rb");
    if (fpIn==NULL)
        {
            printf ("Error opening input file:%s",argv[0+argCount]);
            exit (1);
        }
	strcpy(name,argv[0+argCount]);
	*FindChar(name,'.') = 0;
    fpOut = fopen (argv[1+argCount],"wb");
    if (fpOut==NULL)
        {
            printf ("Error opening output file:%s",argv[1+argCount]);
            exit (1);
        }

	int colCount = -1;
	ch = getc(fpIn);
    while (!feof (fpIn))
        {
			// replace this code with your own

			if(ch == 10 || ch == 13)
				colCount = -1;

			if(ch == 0x9)
			 {
				if(!(colCount & 0x7))
				 {
					putc(' ',fpOut);
					colCount++;
				 }
				while(colCount & 0x7)
				 {
					putc(' ',fpOut);
					colCount++;
				 }
			 }
			else
			 {
				putc(ch,fpOut);
				colCount++;
			 }


			ch = getc(fpIn);
        }

    fclose (fpIn);
    fclose (fpOut);
    printf ("\ndone.");
	exit(0);
}

//==============================================================================

