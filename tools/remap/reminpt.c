#include "stdio.h"
#include "string.h"
#include	"stdlib.h"

#include "utypes.h"
#include "remap.h"

char diffFileName[20] = "test.dff";
char oldMapFileName[20] = "test.chr";
char newMapFileName[20] = "test2.chr";

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
			strcpy (diffFileName,argv[1]);
			strcpy (oldMapFileName,argv[2]);
			strcpy (newMapFileName,argv[3]);
			return;
	 	 }
#if 0
    	if (argc==3)
	 	 {
			strcpy (oldCharFileName,argv[1]);
			strcpy (newCharFileName,argv[2]);
			strcpy (diffFileName,argv[2]);
			ChangeExtension(diffFileName,".dff");
			return;
		 }
#endif
	  // if here then we are not happy with the format, so just print help

	  printf("\
Remap V1.0 (c) 1991,92 Developer Resources\n\
Appy char differences file to map\n\
By Kevin T Seghetti\n\n\
Format: Remap <difffile> <oldmapfile> <newmapfile>\n");
	  exit(0);
}

//=============================================================================
