//==============================================================================
// deltabin.cpp: file difference generator using xor
// By Kevin T. Seghetti
// (c) 1991,92 Developer Resources
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
FILE *fpIn2;
FILE *fpOut;

//==============================================================================

const char szAppName[] = "DeltaBin";

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
	unsigned char ch1,ch2;

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
" (c) 1992 Developer Resources.  All Rights Reserved.\n"
"Binary File Difference (Delta)\n"
"By Kevin T. Seghetti and William B. Norris IV\n"
"Format: " << szProgName << " [<switches>] <newinfile> <oldinfile> <deltaoutfile>\n"
"or      " << szProgName << " [<switches>] <deltainfile> <oldinfile> <newoutfile>\n"
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

    fpIn2 = fopen (argv[1+argCount],"rb");
    if (fpIn2==NULL)
        {
            printf ("Error opening input file:%s",argv[1+argCount]);
            exit (1);
        }

    fpOut = fopen (argv[2+argCount],"wb");
    if (fpOut==NULL)
        {
            printf ("Error opening output file:%s",argv[2+argCount]);
            exit (1);
        }

	ch1 = getc(fpIn);
	ch2 = getc(fpIn2);

	while (!feof (fpIn))
        {
			if(feof(fpIn2))
				ch2 = 0;
			putc(ch1 ^ ch2,fpOut);

			ch1 = getc(fpIn);
			ch2 = getc(fpIn2);
        }

    fclose (fpIn);
    fclose (fpIn2);
    fclose (fpOut);
    printf ("\ndone.");
	exit(0);
}

//==============================================================================

