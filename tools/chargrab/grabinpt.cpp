//==============================================================================
// grabinpt.cpp
//==============================================================================

#include <iostream.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <dir.h>

//==============================================================================
#include <pclib/general.hpp>
#include <pclib/fileio.hpp>
#include <pclib/version.hpp>

#include "grabber.hpp"

//==============================================================================

char InputName[_MAX_PATH];
char OutputFile[_MAX_PATH];			// for characters
char MapFile[_MAX_PATH];			// for map

//==============================================================================
// global switches
boolean bPause = boolean::FALSE;
extern int bgpri;
int paloffset = 0;
boolean bAllTransparent = boolean::FALSE;
boolean bGenesis = boolean::TRUE;

//==============================================================================

void
ChangeExtension( char* szFile, char* szExt )
	{
	if ( strrchr( szFile, '.' ) )
		strncpy( strrchr( szFile, '.' ), szExt, 3+1 );
	else
		{
		strcat( szFile, "." );
		strcat( szFile, szExt );
		}
	}

void GetInput (int argc,char *argv[])
{
	nLimitChars = MAXCHARS;
	for ( int i=1; argv[i] && (*argv[i] == '-'); ++i )
		{
		switch(*(argv[i]+1))
			{
			case 's':
			case 'S':
				bGenesis = boolean::FALSE;
				break;

			case 'g':
			case 'G':
				bGenesis = boolean::TRUE;
				break;

#if 0
			case 'd':
			case 'D':
				displayOn = boolean::FALSE;
				break;
#endif

			case 'n':
			case 'N':
				nukeChars = boolean::TRUE;
				break;

			case 'r':
			case 'R':
				checkRedundantChars = boolean::FALSE;
				break;

			case 'f':
			case 'F':
				checkFlips = boolean::FALSE;
				break;

			case '0':
				bTile0Blank = boolean::TRUE;
				break;

			case 'p':
				sscanf( argv[i]+2, "%d", &paloffset );
				break;

			case 'P':
//				sscanf( argv[i]+2, "%d", &bgpri;
				bgpri = 1;
				break;

			case 't':
			case 'T':
				bAllTransparent = boolean::TRUE;
				break;

			case 'w':
			case 'W':
				bPause = boolean::TRUE;
				break;

			case 'c':
			case 'C':
				sscanf( argv[i]+2, "%d", &nLimitChars );
				if ( nLimitChars > MEGAMAXCHARS )
					nLimitChars = MEGAMAXCHARS;
				break;

#if 0
			case 'h':
			case 'H':
				bCutHoriz = boolean::TRUE;
				break;

			case 'v':
			case 'V':
				bCutHoriz = boolean::FALSE;
				break;

			case 's':
			case 'S':
				bScrollAroundWhenDone = boolean::TRUE;
				break;

			case 'b':
			case 'B':
				bScrollAroundWhenDoneIfBad = boolean::TRUE;
				break;

			case 'o':
			case 'O':
				bOldIlbmOnly = boolean::TRUE;
				break;
#endif

			default:
				break;
			}
		}

//	if ( !interactiveMode )
		{
		if ( argc >= 1+i )
			{
			strcpy( InputName, argv[i] );
			if ( !strrchr( InputName, '.' ) )
				strcat( InputName, ".lbm" );

			strcpy( OutputFile, argv[i] );
			ChangeExtension( OutputFile, ".chr" );

			strcpy( MapFile, argv[i] );
			ChangeExtension( MapFile, ".map" );
			}

		if ( argc >= 2+i )
			{
			strcpy( OutputFile, argv[i+1] );
			if ( !strrchr( OutputFile, '.' ) )
				ChangeExtension( OutputFile, ".chr" );
			}

		if ( argc >= 3+i )
			{
			strcpy( MapFile, argv[i+2] );
			if ( !strrchr( MapFile, '.' ) )
				ChangeExtension( MapFile, ".map" );
			}

	if ( argc == i )
		{ // if here then we are not happy with the format, so just print help
		char szProgName[ _MAX_FNAME ];

		_splitpath( argv[0], NULL, NULL, szProgName, NULL );
		strlwr( szProgName );

		cout <<
/*ts->PlatformName() <<*/ "CharGrab " << szVersion << " Copyright 1994 Cave Logic Studios.  All Rights Reserved.\n"
"IFF graphic to " << /*ts->PlatformName() <<*/ " Character & Map converter\n"
"By Kevin T. Seghetti, William B. Norris IV, and Lars Norpchen\n\n"
"Usage: " << szProgName << " [-switches] <inputLbm[.lbm]> [outputChr[.chr] [outputMap[.map]]]\n";

//        1         2         3         4         5         6         7			  8
//2345678901234567890123456789012345678901234567890123456789012345678901234567890
	cout <<
"  Character Switches:\n" <<
"    -g:  Genesis output                -s:  SNES output\n" <<
"    -r:  No redundant character checking\n" <<
"    -f:  No flip checking              -n:  Nukechars (don't use old .chr file)\n" <<
"    -0:  Force tile #0 to be blank     -c#: Limit maximum number of characters\n"
"    -t:  Allow all transparent colors\n"
;

#if 0
	if ( ts->system() == TargetSystem::SYS_SNES )
	cout <<
"    -2:  4 color chars                 -8:  256 color chars\n" <<
"    -7:  Mode 7 chars                  -6:  16x16 chars\n";
#endif

	cout <<
"\n" <<
"  Map Switches:\n" <<
"    -p#: Force palette offset          -P:  Sets priority bit\n";
#if 0
"\n";
"  Input Switches:\n" <<
"    -o:  Original/Old ILBM files only\n" <<
"\n";
"  Display Switches:\n" <<
"    -d:  Display off                   -w:  Wait for keypress after conversion\n" <<
"    -h:  Traverse pict. horizontally   -v:  Traverse vertically (default)\n" <<
"    -s:  Scroll around picture after   -b:  Scroll around picture if bad tiles\n";
#endif

 		exit(0);
 		}
	}
	}
