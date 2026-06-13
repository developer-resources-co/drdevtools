//==============================================================================
// chardiff.c: generate char ordering differences file
//==============================================================================

#include "utypes.h"
#include "mem.h"
#include "dos.h"
#include "stdlib.h"
#include "stdio.h"
#include "alloc.h"
#include "diffinpt.h"

#include "chardiff.h"

#define CHAR_X 8
#define CHAR_Y 8
#define MAXGRID_X 40
#define MAXGRID_Y 25

FILE *fp;

diffMap[MAXCHARS];

int x,y;
//int a,b,c,d,e,f;
int chars,chars2;
//int checkflag;

#define CHARSIZE (CHAR_X*CHAR_Y/2)
uchar huge *data[MAXCHARS];
uchar huge *data2[MAXCHARS];

uchar huge *tempchar;
uchar huge *tempchar2;

//int a2,b2;
//int i,j;

//==============================================================================

extern int charHeight,width;
extern FLAG displayOn,nukeChars,checkRedundantChars;

//==============================================================================

void MemError (unsigned long size,char type,unsigned long left)
{
    printf ("Memory error:\n%ld bytes\n",size);
    if (type) printf (" of far ");
         else printf (" of reg ");
    printf ("memory...\n%ld bytes available...\n\n",left);
    exit(0);
}

void Error (char *a)
{
    printf ("%s",a);
    exit(0);
}

char *SafeMalloc (unsigned size)
{
 void *a;
 a = malloc (size);
 if (a==NULL) MemError (size,0,(long) coreleft ());
 return a;
}

char far *SafeFarMalloc (unsigned long size)
{
 void far *a;
 a = (void far *) farmalloc (size);
 if (a==NULL) MemError (size,1,farcoreleft ());
 return a;
}


//=============================================================================

main (int argc,char *argv[])
{
	int a,i;
	GetInput (argc,argv);
	for (a=0;a<MAXCHARS;a++)
	 {
		data[a]=SafeFarMalloc   ((long) CHARSIZE);
		data2[a]=SafeFarMalloc  ((long) CHARSIZE);
	 }
	tempchar = SafeFarMalloc    ((long) CHARSIZE);
	tempchar2 = SafeFarMalloc   ((long) CHARSIZE);

	y=x=chars=0;
   	chars = OpenExistingCharFile (data,oldCharFileName);
   	chars2 = OpenExistingCharFile (data2,newCharFileName);

	for(i=0;i<chars;++i)
	 {
		diffMap[i] = FindChar(data[i],data2);
	 }

	WriteDiffFile(diffFileName);
	Error ("done...");
}

//=============================================================================

int
FindChar( uchar huge *chr,uchar huge *charArray[MAXCHARS])
{
	int i;
	for(i=0;i<chars2;i++)
	 {
		if(!memcmp(charArray[i],chr,32))
			return(i);
	 }
	return(-1);
}

//==============================================================================

void WriteDiffFile( char *outputFileName)
{
    int a;
    fp = fopen (outputFileName,"wb");
    if (fp==NULL) Error ("Cannot open diff output file");
    fwrite (&diffMap[0],chars,2,fp);
    fclose (fp);
}

//==============================================================================

int OpenExistingCharFile(uchar huge *buffer[MAXCHARS],char *fileName)
{
    int a,ch;
    if ((fp=fopen (fileName,"rb"))!=NULL)
        {
            fseek (fp,0,2);                             // get fsize
            ch =  ftell (fp) / CHARSIZE;
            fseek (fp,0,0);                            // back to beginning
            if (ch>MAXCHARS) Error ("Too many chars");
            for (a=0;a<ch;a++)
                fread (buffer[a],1,CHARSIZE,fp);   // load existing chars
            fclose (fp);
			return(ch);
        }
	Error("Cannot open file");
}

//==============================================================================
