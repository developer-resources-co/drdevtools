//=============================================================================
// merge.c -- merge two files into one whole
//
// Scott Statton -- Developer Resources.
// Jeff is a weenie
//=============================================================================

#include <stdlib.h>
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

void
Cleanup(int foo)
{
	exit(foo);
}

void
main(int argc,char *argv[])
{
	FILE *inFile1,*inFile2,*oFile,*eFile;
	char oFname[12],eFname[12];
	char *s;
	char *t;
	char i1,i2;

	unsigned long inLen;
	unsigned long saddr;

	printf(
"Merge V2.0 (c) 1992,93 Developer Resources.  All Rights Reserved.\n"
"By Scott L. Statton\n" );

	if(argc < 4)
	{
		printf(
"merge eveninputfile oddinputfile outputfile\n");
		Cleanup(ERR_NOINPUT);
	}

	inFile1 = fopen(argv[1],"rb");
	if(inFile1 == 0)
	{
		printf("File %s not found\n",argv[1]);
		Cleanup(ERR_FILENOTFOUND);
	}

	inFile2 = fopen(argv[2],"rb");
	if(inFile2 == 0)
	{
		printf("File %s not found\n",argv[2]);
		Cleanup(ERR_FILENOTFOUND);
	}

//	t = oFname;
//	s = argv[1];
//	for ( ; *s != '.' ; *t++ = *s++ );
//	*t = 0;
//	strcat (oFname,".odd");

//	s = argv[1];
//	t = eFname;
//	for ( ; *s != '.' ; *t++ = *s++ );
//	*t = 0;
//	strcat (eFname,".evn");

//	eFile = fopen(oFname, "wb");
	oFile = fopen(argv[3], "wb");

	if (oFile )
		{
			i1 = fgetc(inFile1);
			i2 = fgetc(inFile2);
			 while ((!feof(inFile1)) && (!feof(inFile2)))
			{
				fputc(i1,oFile);
				fputc(i2,oFile);
				i1 = fgetc(inFile1);
				i2 = fgetc(inFile2);
			}
		}
	else
		{
			printf("\nCouldn't open file %s\n",argv[3]);
			Cleanup(ERR_FILENOTOPENED);
		}

	printf("\nFile merge complete.\n");
exit(0);
}
