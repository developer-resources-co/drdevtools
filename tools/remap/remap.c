//==============================================================================
// remap.c: remaps map files based on dff(chardiff) file
//==============================================================================

#include "utypes.h"
#include "mem.h"
#include "dos.h"
#include "stdlib.h"
#include "stdio.h"
#include "alloc.h"
#include "reminpt.h"

#include "remap.h"

#define CHAR_X 8
#define CHAR_Y 8
#define MAXGRID_X 40
#define MAXGRID_Y 25

FILE *fp;

int diffMap[MAXCHARS];

int x,y;
int chars,chars2;

//#define CHARSIZE (CHAR_X*CHAR_Y/2)
//uchar huge *data[MAXCHARS];
//uchar huge *data2[MAXCHARS];
//uchar huge *tempchar;
//uchar huge *tempchar2;

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
	unsigned int xSize,ySize,entry,newEntry,count;
	int temp;
	int a,i;
	FILE *oldFP,*newFP;
	GetInput (argc,argv);


	for(i=0;i<MAXCHARS;i++)
		diffMap[i] = -1;
	y=x=chars=0;
	ReadDiffFile(diffFileName);

    oldFP = fopen (oldMapFileName,"rb");
    if (oldFP==NULL) Error ("Cannot find old map file");

    newFP = fopen (newMapFileName,"wb");
    if (newFP==NULL) Error ("Cannot open new map file");

	xSize = fgetc(oldFP) << 8;
	xSize |= fgetc(oldFP);

	ySize = fgetc(oldFP) << 8;
	ySize |= fgetc(oldFP);

	fputc((unsigned char)(xSize >> 8),newFP);
	fputc((unsigned char)xSize,newFP);

	fputc((unsigned char)(ySize >> 8),newFP);
	fputc((unsigned char)ySize,newFP);

	count = ySize*xSize;
	for(i=0;i<count;++i)
	 {
		entry = fgetc(oldFP) << 8;
		entry |= fgetc(oldFP);

		temp = diffMap[entry & 0x7ff];
		if(temp == -1)
			newEntry = entry;
		else
		 {
			newEntry = temp & 0x7ff;
			newEntry |= entry & 0xf800;
		 }
		fputc((unsigned char)(newEntry >> 8),newFP);
		fputc((unsigned char)newEntry,newFP);
	 }

	close(oldFP);
	close(newFP);
//	WriteDiffFile(diffFileName);
	Error ("done...");
}

//=============================================================================

void ReadDiffFile( char *inputFileName)
{
    int a;
    fp = fopen (inputFileName,"rb");
    if (fp==NULL) Error ("Cannot find diff file");
    fread (&diffMap[0],MAXCHARS,2,fp);
//    fwrite (&diffMap[0],chars,2,fp);
    fclose (fp);
}

//==============================================================================

#if 0
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
#endif

//==============================================================================
