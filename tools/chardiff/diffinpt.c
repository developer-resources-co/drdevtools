#include "stdio.h"
#include "string.h"
#include	"stdlib.h"

#include "utypes.h"
#include "chardiff.h"

//char IFFName[20] = "test.lbm";
char oldCharFileName[20] = "test.chr";
char newCharFileName[20] = "test2.chr";
char diffFileName[20] = "test.dff";

// global switches
//FLAG nukeChars = FALSE;
//FLAG checkRedundantChars = TRUE;
//FLAG displayOn = TRUE;
//FLAG interactiveMode = FALSE;


void
ChangeExtension(char *string,char *newExt)
{
	int i;
	while(*string != 0 && *string != '.')
		string++;
	strcpy(string,newExt);
}


void GetInput (int argc,char *argv[])
{
    	if (argc==4)
	 	{
			strcpy (oldCharFileName,argv[1]);
			strcpy (newCharFileName,argv[2]);
			strcpy (diffFileName,argv[3]);
			return;
	 	}
    	if (argc==3)
	 	{
			strcpy (oldCharFileName,argv[1]);
			strcpy (newCharFileName,argv[2]);
			strcpy (diffFileName,argv[2]);
			ChangeExtension(diffFileName,".dff");
			return;
	 	}
	  // if here then we are not happy with the format, so just print help

	  printf("\
CharDiff V1.0 (c) 1991 Developer Resources\n\
Generate old char to new char differences file\n\
(use remap to apply diff file to maps)\n\
By Kevin T Seghetti\n\n\
Format: CharDiff <oldcharfile> <newcharfile> [difffile]\n");
	  exit(0);
}

//=============================================================================
