//=============================================================================
// compr.c -- character compressor
// uses rle
// by Kevin T. Seghetti -- (c) 1992 Developer Resources.
// Jeff is a weenie
//=============================================================================

// rle format:
// 1xxx xxxx,n	= repeat n x times
// 0xxx xxxx	= there are x unique bytes following
// each run is zero terminated
//==============================================================================
//	char file format:
/*
struct hdr
{
	UWORD length;
	UBYTE compType;
	UBYTE compTypeData;
}
 followed by the char data(in whatever compression is specified)

enum
{
	compType_none,
	compType_RLE
}

*/

void compress( char *szInFilename, char *szOutfilename, int nModulo );

//==============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <alloc.h>
#include <dos.h>
#include <dir.h>

#include "global.h"

typedef enum { FALSE, TRUE } BOOL;

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

//=============================================================================

unsigned long
	WritePass(FILE *oFile,unsigned char far *buffer,unsigned long len,int addend)
	{
	int count,currByte,nextByte;
	unsigned char far *tempPtr;
	unsigned long outSize = 0L;

	count = 0;

	while(len > 0)
	 {
		tempPtr = buffer;					// remember addr of beginning of run
		count = 1;							// since we read nextByte, count starts at 1
		currByte = *buffer;					// read 1st byte
		buffer += addend;
		nextByte = *buffer;					// read next byte
		buffer+=addend;
		while(currByte == nextByte && count < 127 && count < len)
		{
			nextByte = *buffer;
			buffer+= addend;
			count++;
		}
		buffer -= addend;							// unfetch whatever made us stop
		if(count > 2 || count == len)							// write run
		{
			fputc(count|0x80,oFile);
			fputc(currByte,oFile);
			outSize += 2;
			len-=count;
		}
		else
		{
			count = 0;							// start count over
			buffer = tempPtr;					// go back to begining of unique run
			currByte = *buffer;					// read 1st byte
			buffer += addend;
			nextByte = *buffer;					// read next byte
			buffer+=addend;
			while
			 (
				 (
					nextByte != currByte ||
					nextByte != *buffer
				 ) &&
				count < 127	&&
				count < len
			 )
			 {
				currByte = nextByte;
				nextByte = *buffer;
				buffer+= addend;
				count++;
			 }

			buffer = tempPtr;					// go back to begining of unique run
			fputc(count,oFile);
			++outSize;
			while(count--)
				{
				fputc(*buffer,oFile);
				++outSize;
				buffer+=addend;
				len--;
				}
		}
		currByte = nextByte;
	 }
	fputc(0,oFile);							// terminate run
	++outSize;

	return( outSize );
	}

//==============================================================================
// kts note: place this in a library
//------------------------------------------------------------------------------

void WriteInt(int data,FILE *fp)
{
#ifdef GENESIS
	putc(data >> 8,fp);
	putc(data & 0xff,fp);
#endif

#ifdef SNES
	putc(data & 0xff,fp);
	putc(data >> 8,fp);
#endif
}

//==============================================================================

unsigned long
	WriteCompressedData( FILE *oFile, unsigned char far *buffer,
		unsigned long len, unsigned int modulo )
	{
	int i;
	word outSize = 0L;

	len = len / modulo;

//	fwrite( (word *)&len, sizeof(word), 1, oFile );
	WriteInt(len,oFile);
	fputc( 1, oFile );
	fputc( modulo, oFile );

	for(i=0;i<modulo;i++)
		outSize += WritePass(oFile,buffer+i,len,modulo);

	return( outSize );
}
//==============================================================================


char *szFileList;
int nFileList;
FILE *fpFileList;
char szFileListFileName[MAXPATH];

void
	init_filelist( char *szFileName )
	{
	if ( *szFileName == '@' )
		{
		szFileList = szFileName+1;
		fpFileList = fopen( szFileList, "rt" );
		if ( !fpFileList )
			{
			printf("Error: File %s not found\n", szFileList );
			Cleanup(ERR_FILENOTFOUND);
			}
		nFileList = 0;
		}
	else
		strcpy( szFileListFileName, szFileName );
	}


char *
	get_infilename()
	{
	static char szFileName[ 100 ];

	if ( szFileList )
		{
		do
			{
			fgets( szFileName, 100, fpFileList );
			*strrchr( szFileName, '\n' ) = '\0';
			}
		while ( !*szFileName );
		}
	else
		strcpy( szFileName, szFileListFileName );
	return( szFileName );
	}


	Usage( char *argv[] )
	{
	static char usageText[] =
		{
PLATFORMNAME " Compressor %s (c) 1991-1992 Developer Resources\n\
Binary File Compressor\nBy Kevin T. Seghetti and William B. Norris IV\n\n\
Usage: %s [-m#] <inFile> <outFile>\n\
Switches:\n\
        -m#: Set Modulo used (default = 1)\n\
        SNES recommend (bitplanes/2), Genesis recommend 1\n\
\n"
		};
	char szProgName[ MAXFILE ];
	char szBuffer[ 512 ];

	_fnsplit( argv[0], NULL, NULL, szProgName, NULL );
	strlwr( szProgName );

	sprintf( szBuffer, usageText, szVersion, szProgName );
	printf( szBuffer );
	}


void
	main(int argc,char *argv[])
	{
	int param = 0;
	unsigned int modulo = 1;
	BOOL bBestCompression = FALSE;
	char *szInFilename, *szOutFilename;

	while(*argv[param+1] == '-')
	 {
		switch(*(argv[param+1]+1))
		 {
			case 'm':
			case 'M':
				modulo = *(argv[param+1]+2) - '0';
				if(modulo == 0 || modulo > 9)
				 {
					printf("Cannot have a modulo of %d\nResetting to 1",modulo);
					modulo = 1;
				 }
				break;
			case 'b':
			case 'B':
				bBestCompression = TRUE;
				break;
			default:
				break;
		 }
		param++;
	 }

	if (argc < param + 3)
		{
		Usage( argv );
		Cleanup(ERR_NOINPUT);
		}

	init_filelist( argv[param+1] );
	szInFilename = get_infilename();
		{
		if ( bBestCompression )
			{
			int m;

			for ( m=1; m<=8; ++m )
				compress( szInFilename, argv[param+2], m );
			}
		else
			compress( szInFilename, argv[param+2], modulo );
		}

	exit(0);
	}


void
	compress( char *szInFilename, char *szOutFilename, int nModulo )
	{
	unsigned char far *charBuffer;
	unsigned char far *charPtr;
	unsigned long inLen, outLen;
	FILE *inFile,*outFile;

	inFile = fopen( szInFilename, "rb" );
	if ( !inFile )
		{
		printf("Error: File %s not found\n", szInFilename );
		Cleanup(ERR_FILENOTFOUND);
		}

	outFile = fopen( szOutFilename, "wb" );
	if ( !outFile )
		{
		printf("Error: Cannot open file %s\n", szOutFilename );
		Cleanup(ERR_FILENOTFOUND);
		}

	fseek(inFile,0,2);						// go to end of file
	inLen = ftell(inFile);					// find out how long file is
	fseek(inFile,0,0);						// go back to begining of file

	charPtr = charBuffer = (unsigned char far *)farmalloc(inLen);
	if(!charPtr)
		{
		printf("Error: Out of Memory\n");
		Cleanup(ERR_NOMEM);
		}

	printf( "%s\t", szInFilename );  fflush( stdout );

	while((!feof(inFile)))
		*charPtr++ = getc(inFile);
	fclose(inFile);

	outLen = WriteCompressedData(outFile,charBuffer,inLen,nModulo);
	fclose(outFile);
	printf( "[%d] %lu -> %lu bytes (%d%%)\n",
		nModulo, inLen, outLen, outLen*100/inLen );
	}

//=============================================================================
