////////////////////////////////////////////////////////////////////////////////
// Downloader Version 2.0 - Developer Resources
// Scott Statton (c) 1991, All Rights Reserved.
// Improved to handle files > 64K
// 18 June 1991
// SNES modifications by William B. Norris IV

///////////////////////////////////////////////////////////////////////////////
// sdl.cpp -- Binary Downloader for SNES V2.0
// Scott Statton & Kevin T. Seghetti -- Developer Resources.
// Jeff is a weenie
///////////////////////////////////////////////////////////////////////////////

#include <iostream.h>

///////////////////////////////////////////////////////////////////////////////

#define ERR_NOINPUT 5
#define ERR_FILENOTFOUND 5
#define ERR_FILENOTOPENED 5
#define ERR_NOMEM 5

#include <pclib/general.h>
#include <pclib/fileio.h>
#include <pclib/filename.h>
#include <pclib/version.h>

#include "board.hpp"
#include "progmetr.h"

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

void
	Cleanup( int err = 0 )
	{
	exit( err );
	}


void
	main( int argc, char *argv[] )
	{
	FILE *inFile;
	unsigned long inLen;
	unsigned long startAddr;
	DevelopmentSystemBoard board( 0x318 );

	cout << "sdl " << szVersion << " (c) 1992,93 Adept Creations.  All Rights Reserved\n"
		  << "SNES downloader\n"
		  << "By William B. Norris IV\n";

	if ( argc < 2 )
		{
		printf("sdl inputfile <start-address>");
		Cleanup( ERR_NOINPUT );
		}

	if ( argc == 2 )
		startAddr = 0x00;
	else
		if (!sscanf(argv[2],"%lx",&startAddr)) startAddr = 0x0;

	if ( !(inFile = fopen(argv[1],"rb") ) )
		{
		printf( "File not found\n" );
		Cleanup( ERR_FILENOTFOUND );
		}
	inLen = ffilesize( inFile );

	cout << "Loading file \"" << argv[1] << "\" to address $" << hex << startAddr 
		  << "  size: $" << inLen << '\n';

	board.control( 0x63 );
	board.write( inFile, inLen, startAddr );
	board.control( 0x61 );				// 0x70 );

	fclose ( inFile);

	exit(0);
	}

////////////////////////////////////////////////////////////////////////////////

