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
#include <pclib/profile.hpp>

#include <pclib/iff.hpp>
#include <pclib/iffilbm.hpp>
#include <pclib/iffpbm.hpp>
#include <pclib/iffanim.hpp>
#include <pclib/grphport.hpp>
#include <pclib/ini.hpp>

#include "anmsplit.hpp"
#include "anim.hpp"
#include "cloption.hpp"

//==============================================================================

char AnimName[ _MAX_PATH ];
char szSrcName[ _MAX_PATH ];

//==============================================================================
// global switches

char *pLabel = NULL;

boolean bPause;								// = boolean::FALSE;
boolean bReferenceFrameOne;		// = boolean::FALSE;
colorMapIndex colorToTrack;		// = 16;
boolean bDisplay;							// = boolean::TRUE;

//==============================================================================

void GetInput (int argc,char *argv[])
{
	{ // Read all options in before local (command-line) overrides
	// LoadPrefs()
	Ini *prefs = new Ini( argv[0] );

	bDisplay = prefs->Boolean( "Display", boolean::TRUE );
	colorToTrack = prefs->Int( "Color", 16 );
	bReferenceFrameOne = prefs->Boolean( "ReferenceFrameOne", boolean::FALSE );
	bPause = prefs->Boolean( "Pause", boolean::FALSE );

	delete prefs;
	}

	int i = 0;

	while ( *argv[i+1] == '-' )
		{
		switch(*(argv[i+1]+1))
			{
			case 'd':
			case 'D':
				bDisplay = boolean::FALSE;
				break;

			case 'r':
			case 'R':
				bReferenceFrameOne = boolean::TRUE;
				break;

			case 'c':
			case 'C':
				colorToTrack = atoi(argv[i+1]+2);
				break;

			case 'l':
			case 'L':
				pLabel = argv[i+1]+2;
				break;

			case 'o':
			case 'O':
				strcpy( szSrcName, argv[i+1]+2 );
				break;

			case 'p':
			case 'P':
				bPause = boolean::TRUE;
				break;

			default:
				break;
			}
		++i;
		}

		{
		if ( argc == 2+i )
			{
        	strcpy( AnimName, argv[1+i] );
			if ( !strrchr( AnimName, '.' ) )
				strcat( AnimName, ".abm" );

			if ( !*szSrcName )
				{
				strcpy( szSrcName, AnimName );
				ChangeExtension( szSrcName, ".dat" );
				}

			if ( !strrchr( szSrcName, '.' ) )
				strcat( szSrcName, ".dat" );

			return;
		 	}

		{ // if here then we are not happy with the format, so just print help
		char szProgName[ _MAX_PATH ];

		_splitpath( argv[0], NULL, NULL, szProgName, NULL );
		strlwr( szProgName );

		printf(
"AbmCoord %s  (c) 1993 Alexandria, Inc.  All Rights Reserved.\n"
"IFF animation to coordinates\n"
"By William B. Norris IV\n"
"Based on libraries by Kevin T. Seghetti and William B. Norris IV\n"
"(c) 1992, 1993 Developer Resources.  All Rights Reserved.\n\n"
"Usage: %s [<switches>] <inputAnim>[.abm]\n"
"", szVersion, szProgName );

//        1         2         3         4         5         6         7			  8
//2345678901234567890123456789012345678901234567890123456789012345678901234567890
	cout <<
"    -d:  display off\n"
"    -r:  Use Frame #1 as reference point\n"
"    -c#: Set color to track (default is 16)\n"
"    -p:  Pause and wait for keypress at end\n"
"    -l<label>: Override default label\n"
"    -o<filename>[.dat]: Set output filename\n"

#if 0
	d: disable display
	p: output FORM PBM
	m: multiple .lbm's
#endif
		;
 		exit(0);
 		}
	}
	}

//==============================================================================

