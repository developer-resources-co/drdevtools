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
#include <pclib/math.hpp>				// should be fixed.hpp

#include <pclib/iff.hpp>
#include <pclib/iffilbm.hpp>
#include <pclib/iffpbm.hpp>
#include <pclib/iffanim.hpp>
#include <pclib/grphport.hpp>
#include <pclib/ini.hpp>

#include "abmshow.hpp"
#include "anim.hpp"
#include "cloption.hpp"

//==============================================================================

char AnimName[ _MAX_PATH ];

//==============================================================================
// global switches

boolean bPause;								// = boolean::FALSE;
boolean bDisplay;							// = boolean::TRUE;
int nLoop;									// = 1;
int xOfs;									// = 0;
int yOfs;                                   // = 0;

//==============================================================================

void GetInput (int argc,char *argv[])
{
	{ // Read all options in before local (command-line) overrides
	// LoadPrefs()
	Ini* prefs = new Ini( argv[0] );

	bDisplay = prefs->Boolean( "Display", boolean::TRUE );
	bPause = prefs->Boolean( "Pause", boolean::FALSE );
	nLoop = prefs->Int( "Loop", 1 );

	delete prefs;
	}

	for ( int i=1; *argv[i] == '-'; ++i )
		{
		switch( *( argv[i]+1 ) )
			{
			case 'd':
			case 'D':
				bDisplay = boolean::FALSE;
				break;

			case 'p':
			case 'P':
				bPause = boolean::TRUE;
				break;

			case 'l':
			case 'L':
				if ( *( argv[i]+2 ) )
					sscanf( argv[i]+2, "%d", &nLoop );
				else
					nLoop = 0;
				break;

			case 'x':
			case 'X':
				{
				}
				break;

			case 'y':
			case 'Y':
				{
				}
				break;

			default:
				break;
			}
		}

		{
		if ( argc == 1+i )
			{
        	strcpy( AnimName, argv[i] );
			if ( !strrchr( AnimName, '.' ) )
				strcat( AnimName, ".abm" );

			return;
		 	}

		{ // if here then we are not happy with the format, so just print help
		char szProgName[ _MAX_PATH ];

		_splitpath( argv[0], NULL, NULL, szProgName, NULL );
		strlwr( szProgName );

		printf(
"AbmShow %s  (c) 1993 Adept Creations.  All Rights Reserved.\n"
"IFF AnimBrush Display\n"
"By William B. Norris IV\n"
"Based on libraries by Kevin T. Seghetti and William B. Norris IV\n"
"(c) 1992, 1993 Developer Resources.  All Rights Reserved.\n\n"
"Usage: %s [<switches>] <inputAnim>[.abm]\n"
"", szVersion, szProgName );

//        1         2         3         4         5         6         7			  8
//2345678901234567890123456789012345678901234567890123456789012345678901234567890
	cout <<
"    -d:    Display off\n"
"    -p:    Pause and wait for keypress at each frame\n"
"    -l[#]: Loop # of times (no value = forever)\n"
		;
 		exit(0);
 		}
	}
	}

//==============================================================================

