//=============================================================================
// split.c -- break a file into two halves
// Scott Statton -- Developer Resources.
// Jeff is a weenie
//=============================================================================

#include <stdio.h>
#include <alloc.h>
#include <dos.h>

//=============================================================================

#define ERR_NOINPUT 5
#define ERR_FILENOTFOUND 5
#define ERR_FILENOTOPENED 5
#define ERR_NOMEM 5

//=============================================================================

typedef unsigned int word;

Cleanup(int foo)
{
	exit(foo);
}

//=============================================================================

void
main(int argc,char *argv[])
{
	FILE *inFile,*oFile,*eFile;
	char oFname[12],eFname[12];
	char *s;
	char *t;
	char t1,t2;

	unsigned long inLen;
	unsigned long saddr;

	if(argc < 2)
	{
		printf("Split V1.00 (c) 1991 Developer Resources\nFile Splitter\nBy Scott L. Statton\n\nFormat: Split <inputfile>\n");
		Cleanup(ERR_NOINPUT);
	}

	inFile = fopen(argv[1],"rb");
	if(inFile == 0)
	{
		printf("File not found\n");
		Cleanup(ERR_FILENOTFOUND);
	}

	t = oFname;
	s = argv[1];
	for ( ; *s != '.' && *s; *t++ = *s++ );
	*t = 0;
	strcat (oFname,".odd");

	s = argv[1];
	t = eFname;
	for ( ; *s != '.' && *s; *t++ = *s++ );
	*t = 0;
	strcat (eFname,".evn");

	eFile = fopen(oFname, "wb");
	oFile = fopen(eFname, "wb");

	if ( eFile && oFile )
		{
			t1 = fgetc(inFile);
			t2 = fgetc(inFile);

			while(!feof(inFile))
			{
				fputc(t1,eFile);
				fputc(t2,oFile);
				t1 = fgetc(inFile);
				t2 = fgetc(inFile);
			}
		}
	else
		{
			printf("\nCouldn't open file %s, or %s\n",oFname,eFname);
			exit(1);
		}

	printf("\nFile split complete.\n");
	exit(0);
}

//=============================================================================
