//=============================================================================
// summer.c -- checksum a sega Genesis binary
// By Scott Statton Kevin Seghetti -- (c) 1991,92 Developer Resources.
// SNES modifications by William B. Norris IV
// Jeff is a weenie (that's what I've heard -WBNIV)
//=============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <dos.h>
#include <dir.h>
#include <conio.h>

#include "global.h"
char szProgName[ MAXFILE ];

typedef unsigned int word;
typedef unsigned int BOOL;

/////////////////////////////////////////////////////////////////////////////

#ifdef GENESIS
#define HEADER_SIZEOF  (0x200)
const long HEADER_OFFSET = 0L;
#endif
#ifdef SNES
#define GAMENAME_SIZEOF	 (21)
#define HEADER_SIZEOF  (0x100)
const long HEADER_OFFSET = 0x007F00L;
#endif

//=============================================================================

#define TRUE 1
#define FALSE 0

#define ERR_NOINPUT 5
#define ERR_FILENOTFOUND 5
#define ERR_FILENOTOPENED 5
#define ERR_NOMEM 5

//=============================================================================

void
	Cleanup(int foo)
	{
	exit(foo);
	}

//=============================================================================

void
	Usage( int argc, char *argv[] )
	{
	static char usageText[] =
		{
PLATFORMNAME " Checksum %s (c) 1991,92 Developer Resources\n"
PLATFORMNAME " Game Checksummer\n\
By Kevin T. Seghetti & Scott Statton\n"
#ifdef SNES
"SNES modifications by William B. Norris IV\n"
#endif
"Format: %s [-s#] [-d] <inputfile> [<outputfile>]\n\
	Switches:\n\
		-s#:	set file size in bytes (ignore header)\n\
			(note: size will not cause file to shrink)\n\
			(see last column in chart for size)\n\
		-d:	Don't insert checksum into file\n\
\n\
	Game Sizes Chart:\n\
		 1 megabit  =  128K                  =  131072 bytes\n\
		 2 megabits =  256K = 0.25 megabyte  =  262144 bytes\n\
		 4 megabits =  512K = 0.50 megabyte  =  524288 bytes\n\
		 6 megabits =  768K = 0.75 megabyte  =  786432 bytes\n\
		 8 megabits = 1024K = 1.00 megabyte  = 1048576 bytes\n\
		12 megabits = 1536K = 1.50 megabytes = 1572864 bytes\n\
		16 megabits = 2048K = 2.00 megabytes = 2097152 bytes\n\
\n"
		};
	char szBuffer[ 1024 ];

	sprintf( szBuffer, usageText, szVersion, szProgName );
	fprintf( stderr, szBuffer );
	Cleanup( ERR_NOINPUT );
	}


BOOL addCheck = TRUE;

void
	main(int argc,char *argv[])
	{
	FILE *inFile;
	int argCount;
	long size = 0L;

	// Create name of program
	fnsplit( argv[0], NULL, NULL, szProgName, NULL );
	strlwr( szProgName );

	argCount = 1;
	while(argCount < argc)
		{
		if (*argv[argCount] == '-')				// must be command line switch
			switch(argv[argCount][1])
				{
				case 's':
				case 'S':
					sscanf(&argv[argCount][2],"%ld",&size);
					break;
				case 'd':
				case 'D':
					addCheck = FALSE;
					break;
				}
		else break;
			argCount++;
		}

	if (argc-argCount < 1)
		Usage( argc, argv );

	inFile = fopen(argv[argCount+0],"r+b");
	if ( !inFile )
		{
		printf("File not found\n");
		Cleanup(ERR_FILENOTFOUND);
		}

#if 0
	if (argc < argCount+2)
		{
		char *s, *t;

		t = outFile;
		s = argv[argCount+0];
		for ( ; *s != '.' && *s; *t++ = *s++ );
		*t = 0;
		strcat (outFile,".sum");
		}
	else
		strcpy(outFile,argv[argCount+1]);
#endif

	checksum( inFile, size );

	Cleanup( EXIT_SUCCESS );
	}


int
	checksum( FILE *inFile, long size )
	{
	unsigned long len;
	unsigned long lenFile;
	unsigned char header[ HEADER_SIZEOF ];
	unsigned int c;
	unsigned int temp;

	int nChecksum;

	word buffer[10];

	{ // Draw display
	clrscr();
	printf( PLATFORMNAME " Checkum %s (c)1991,1992 Developer Resources.  "
		"All Rights Reserved.\n", szVersion );

	gotoxy( 1, 3 );
	printf( "ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿" );
	printf( "³                                                                              ³" );
	printf( "ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ" );
	}

	fseek( inFile, HEADER_OFFSET, SEEK_SET );
	fread( header, HEADER_SIZEOF, 1, inFile );
	// affect len

	// Validate for a real .ROM file
#ifdef GENESIS
	if(strncmp((char *)&header[0x100],"SEGA GENESIS",11) && !size)
		{
		printf("\nFile does not contain valid Sega Genesis header\n");
		exit(1);
		}
#endif

#ifdef SNES
	// No real way to verify it's a ROM for the SNES, but this will half work
	if ( ! ( (header[0xD5] == 0x20) || (header[0xD5] == 0x21) ) )
		{
		printf("\nFile does not contain valid map mode\n" );
		exit( 1 );
		}
#endif

	{ // Print the ROM game registration area
	static char szGameName[ GAMENAME_SIZEOF+1 ];

	gotoxy( 1, 8 );
	sprintf( szGameName, "%*s", GAMENAME_SIZEOF, header+0xC0 );
	printf( "Game Name: [%*s]\n", GAMENAME_SIZEOF, szGameName );
	}

	if ( !size )
		{
#ifdef GENESIS
		size = (((unsigned long)header[0x1a4])<<24) +  (((unsigned long)header[0x1a5])<<16) +  (((unsigned long)header[0x1a6])<<8) + header[0x1a7];
		size++; 		// since the header has it 1 too small
#endif
#ifdef SNES
		int i;

		for ( size=2L, i=1; i<header[0xD7]; ++i )
			size *= 2L;
		size *= 1024L;
#endif
		printf(" \nFile header block indicates size of " );
		}
	else
		printf("\nUser chosen file size of " );
	printf( "%ld bytes (%ldK)\n", size, size/1024L );

	if ( size > 0x00FFFFFF )
		{
		printf("\nSize is too big\n");
		exit(1);
		}

#ifdef GENESIS
	// Zero-out appropriate information to calculate correct checksum
#endif

#ifdef SNES
	fseek( inFile, HEADER_OFFSET+0xDC, SEEK_SET );
	buffer[0] = 0xFFFF;
	fwrite( buffer, sizeof(word), 1, inFile );

	fseek( inFile, HEADER_OFFSET+0xDE, SEEK_SET );
	buffer[0] = 0;
	fwrite( buffer, sizeof(word), 1, inFile );
#endif

	fseek( inFile, 0L, SEEK_END );
	lenFile = ftell( inFile );

	{ // Calculate checksum while displaying progress meter
	int nChars;
	int nKPerChar;
	BOOL bFill;

	// Initialize some variables for the progress meter
	nChars = 79-2;
	nKPerChar = max( 1, size / nChars );
	bFill = FALSE;
	gotoxy( 2, 4 );

	for ( fseek(inFile,0L,SEEK_SET), len=0L, nChecksum = 0; len<size; ++len )
		{
		if ( len < lenFile )
			c = fgetc(inFile);
		else
			{
			if ( !bFill )
				{
				fseek( inFile, 0L, SEEK_END );
				bFill = TRUE;
				}
			fputc( c=0xFF, inFile );
			}

		if ( len & 1 )
			nChecksum += c + (temp<<8);

		if ( ( len % nKPerChar ) == 0L )
			{
			putchar( bFill ? 'Û' : '±' );
//			printf( "\r%4ldK", (len/1024L)+1 );
			}
		}
	}
	gotoxy( 1, 15 );

	if (addCheck)
		{
		printf( "Writing Checksum\n" );
		fseek( inFile, HEADER_OFFSET+0xDC, SEEK_SET );
		fwrite( (void *)~nChecksum, sizeof(word), 1, inFile );
		fseek( inFile, HEADER_OFFSET+0xDE, SEEK_SET );
		fwrite( (void *)nChecksum, sizeof(word), 1, inFile );
		}

	printf("checksum = %04X, complement = %04X\n", nChecksum, ~nChecksum );
	}

//=============================================================================

