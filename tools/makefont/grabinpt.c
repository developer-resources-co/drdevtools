//==============================================================================
// grabinpt.c
//==============================================================================

#include <stdio.h>
#include <string.h>

//==============================================================================

#include "grabber.h"

//==============================================================================

char IFFName[20] = "test.lbm";
char OutputFile[20] = "test.fon";

FLAG interactiveMode = FALSE;
FLAG displayOn = TRUE;

//==============================================================================

void
ChangeExtension(char *string,char *newExt)
{
	int i;
	while(*string != 0 && *string != '.')
		string++;
	strcpy(string,newExt);
}

//==============================================================================

void GetInput (int argc,char *argv[])
{
	int i = 0;

	while(*argv[i+1] == '-')
	 {
		switch(*(argv[i+1]+1))
		 {
			case 'd':
			case 'D':
				displayOn = FALSE;
				break;
			default:
				break;
		 }
		i++;
	 }

	if(interactiveMode == FALSE)
	 {
    	if (argc==3+i)
	 	{
			strcpy (IFFName,argv[1+i]);
			strcpy (OutputFile,argv[2+i]);
			return;
	 	}
		if(argc==2+i)
	 	{
        	strcpy (IFFName,argv[1+i]);
        	strcpy (OutputFile,argv[1+i]);
			ChangeExtension(OutputFile,".fnt");
			return;
	 	}

	  // if here then we are not happy with the format, so just print help

	  printf("\
MakeFont V1.3 (c) 1992 Developer Resources\n\
IFF graphic to Font converter\n\
By Kevin T. Seghetti and William B. Norris IV\n\n\
Format: MakeFont <inputfile>.lbm\n\
\n");
	  exit(0);
	 }

	// if here, interactive mode must be on

    clrscr ();
    printf ("MakeFont v1.0 (c) 1992 Developer Resources\nby Kevin T. Seghetti & William B. Norris IV\n\n");
    printf ("Please enter IFF source file:");
    gets (IFFName);
    if (strlen(IFFName)==0)
        strcpy (IFFName,"test.lbm");
    printf ("\nPlease enter raw output file:");
    gets (OutputFile);
    if (strlen(OutputFile)==0)
        strcpy (OutputFile,"test.fon");

    printf ("\n\r\Scanning IFF file %s to font data file %s",IFFName,OutputFile);
    printf ("\n\n Hit any key to begin processing...");
    while (!kbhit());
}

//==============================================================================

