//=============================================================================
//
//  hdump.cpp: hexdump utility
//  By Kevin T. Seghetti, (c) 1991,1992 Developer Resources
//
//  History
//	 KTS								Created
//  WBNIV	09 Oct 92	2100	Creates columns correctly
//  WBNIV	11 Dec 92   1938	Displays filename on top of output
//  WBNIV   23 Dec 92   2303  Allows offset into file, negative means from
//											end of file
//  WBNIV   23 Dec 92   2323  Allows specifying a filter (for text display)
//
//=============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <alloc.h>

#include <pclib/libtypes.hpp>
#include <pclib/regexp.hpp>

//=============================================================================

#define ERR_NOINPUT 5
#define ERR_FILENOTFOUND 5
#define ERR_FILENOTOPENED 5
#define ERR_NOMEM 5

//=============================================================================

FILE *inFile;

regexp* r;
char szFilter[128] = "[A-Za-z0-9]";

unsigned char ar[20];

//=============================================================================

inline char ValidChar(char c)
	{
	static char testChar[2] = "?\0";

	*testChar = c;

	return( regexec( r, testChar ) ? c : '.' );
//	return( isprint(c) ? c : '.' );
	}

//=============================================================================

void
PrintLine(int count,long addr)
{
	int i;
	int x_pos = 0;

	if(count < 1)
		return;

	x_pos +=	printf( "%06lX: ", addr );
	for ( i=0; i<count; ++i )
		{
		x_pos += printf("%02X",ar[i]);
		if(!((i+1) % 4))
			x_pos += printf(" ");
		}
	printf( "%*s", 46-x_pos, "" );

	for(i=0;i<count;i++)
		printf("%c",ValidChar(ar[i]));

	printf("\n");
}

//=============================================================================

void
Cleanup(int err)
	{
	if ( inFile )
		fclose( inFile );

	exit(err);
	}

//=============================================================================

void
main(int argc,char *argv[])
	{
	int argCount;
	unsigned long addr = 0UL;
	long offset = 0L;

	if(argc < 2)
	 {
printf(
"HDump V1.13 (c) 1991,92,93 Developer Resources\n"
"Binary File Dumper\n"
"By Kevin T. Seghetti and William B. Norris IV\n"
"\n"
"Usage: hdump -a# -o# -f<regexp> inputfile\n"
"    -a#:         Address\n"
"    -o#:         File offset\n"
"    -f<regexp>:  Filter\n"
	);
		Cleanup(ERR_NOINPUT);
	 }

	argCount = 1;
	while(argCount < argc)
	 {
		if(*argv[argCount] == '-')				// must be command line switch
			switch(argv[argCount][1])
			 {
				case 'a':
				case 'A':
					sscanf(&argv[argCount][2],"%lu",&addr);
					break;
				case 'o':
				case 'O':
					sscanf( &argv[argCount][2], "%ld", &offset );
					break;
				case 'f':
				case 'F':
					strcpy( szFilter, &argv[argCount][2] );
					break;
			 }
		else break;
		argCount++;
	 }

	inFile = fopen(argv[argCount],"rb");
	if ( !inFile )
		{
		printf("File not found\n");
		Cleanup(ERR_FILENOTFOUND);
		}

	fseek( inFile, offset, offset<0 ? SEEK_END : SEEK_SET );

	printf( "hdump: %s\n", argv[argCount] );

	r = regcomp( szFilter );
	int i = 0;
	while ( !feof(inFile) )
		{
		ar[i] = getc(inFile);
		if(!feof(inFile))
			i++;
		if(i == 16)
			{
			PrintLine(16,addr);
			i = 0;
			addr +=16;
			}
		}
	PrintLine(i,addr);
	Cleanup(0);
	}

//=============================================================================

