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

#include "abmscale.hpp"
#include "anim.hpp"
#include "cloption.hpp"

//==============================================================================

char AnimName[ _MAX_PATH ];
char szOutputName[ _MAX_PATH ];

//==============================================================================
// global switches

boolean bPause;								// = boolean::FALSE;
boolean bDisplay;							// = boolean::TRUE;
fixed xScale(1,0);
fixed yScale(1,0);

boolean fScale( char* szScale, fixed& newScale )
	{
	int i = atoi( szScale );

	if ( *( szScale + strlen( szScale ) - 1 ) == '%' )
		{ // Handle percentage
		newScale = fixed(i) / fixed(100);
		return boolean::TRUE;
		}
	else
		return boolean::FALSE;
	}

//==============================================================================

void GetInput (int argc,char *argv[])
{
	{ // Read all options in before local (command-line) overrides
	// LoadPrefs()
	Ini* prefs = new Ini( argv[0] );

	bDisplay = prefs->Boolean( "Display", boolean::TRUE );
	bPause = prefs->Boolean( "Pause", boolean::FALSE );

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

			case 'o':
			case 'O':
				strcpy( szOutputName, argv[i]+sizeof("-o") );
				if ( !strchr( szOutputName, '.' ) )
					strcat( szOutputName, ".abm" );
				break;

			case 'p':
			case 'P':
				bPause = boolean::TRUE;
				break;

			case 's':
			case 'S':
				{ // Can appear more than once -- can specify x or y, pixel or %
				fixed newScale;

				if ( fScale( argv[i]+sizeof( "-?" )-1, newScale ) )
					xScale = yScale = newScale;
				}
				break;

			case 'x':
			case 'X':
				{
				fixed newScale;

				if ( fScale( argv[i]+sizeof( "-?" )-1, newScale ) )
					xScale = newScale;
				}
				break;

			case 'y':
			case 'Y':
				{
				fixed newScale;

				if ( fScale( argv[i]+sizeof( "-?" )-1, newScale ) )
					yScale = newScale;
				}
				break;

			default:
				break;
			}
		}

		{
		if ( argc == 2+i )
			{
        	strcpy( AnimName, argv[i] );
			if ( !strrchr( AnimName, '.' ) )
				strcat( AnimName, ".abm" );

			strcpy( szOutputName, argv[i+1] );
			if ( !strrchr( szOutputName, '.' ) )
				strcat( szOutputName, ".abm" );

			return;
		 	}

		{ // if here then we are not happy with the format, so just print help
		char szProgName[ _MAX_PATH ];

		_splitpath( argv[0], NULL, NULL, szProgName, NULL );
		strlwr( szProgName );

		printf(
"AbmScale %s  (c) 1993 Adept Creations.  All Rights Reserved.\n"
"IFF animation scale\n"
"By William B. Norris IV\n"
"Based on libraries by Kevin T. Seghetti and William B. Norris IV\n"
"(c) 1992, 1993 Developer Resources.  All Rights Reserved.\n\n"
"Usage: %s [<switches>] <inputAnim>[.abm] <outputAnim>[.abm]\n"
"", szVersion, szProgName );

//        1         2         3         4         5         6         7			  8
//2345678901234567890123456789012345678901234567890123456789012345678901234567890
	cout <<
"    -d:   Display off\n"
"    -p:   Pause and wait for keypress at each frame\n"
"    -x#\%: Scale picture by #\% in x direction\n"
"    -y#\%: Scale picture by #\% in y direction\n"
"    -s#\%: Scale picture by #\% in both directions (default: 100\%)\n"
//"    -o<filename>[.dat]: Set output filename\n"

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

