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

#include "abmdummy.hpp"

//==============================================================================
// global switches
boolean bDisplay = boolean::TRUE;
char szMessage[ 256+1 ];
char szOutputName[_MAX_PATH];
int nFrames = 1;
int nPalette = 0;

//==============================================================================

void GetInput (int argc,char **argv)
{
	for ( ++argv; *argv && **argv == '-'; ++argv, --argc )
		{
		char chSwitch = *( *argv + 1 );
		switch ( chSwitch )
			{
			case 'd':
			case 'D':
				bDisplay = boolean::FALSE;
				break;

			case 'f':
			case 'F':
				nFrames = atoi( *argv+2 );
				break;

			case 'o':
			case 'O':
				strcpy( szOutputName, *argv+2 );
				break;

			case 'p':
			case 'P':
				nPalette = atoi( *argv+2 );
				break;

			default:
				cerr << "Unrecognized command line option \"" << chSwitch << "\"" << endl;
			}
		}

#if 0
	if ( argc != 1 )
		{ // if here then we are not happy with the format, so just print help
		char szProgName[ _MAX_PATH ];

		_splitpath( argv[0], NULL, NULL, szProgName, NULL );
		strlwr( szProgName );

		cout <<
szProgName << szVersion << "  Copyright 1994 Cave Logic Studios.  All Rights Reserved.\n" <<
"Creates placeholder AnimBrushes\n" <<
"By William B. Norris IV\n" <<
" Based on libraries by Kevin T. Seghetti and William B. Norris IV\n" <<
" Copyright 1992,93,94 Developer Resources.  All Rights Reserved.\n\n" <<
"Usage: " << szProgName << " -f<# of frames> -p<palette> \"Animation text\"\n";
//        1         2         3         4         5         6         7			  8
//2345678901234567890123456789012345678901234567890123456789012345678901234567890
 		exit(0);
 		}
#endif

	strcpy( szMessage, *argv );
	if ( !*szOutputName )
		strcpy( szOutputName, "test.abm" );

	cout << "szMessage: " << szMessage << endl;
	cout << "szOutputName: " << szOutputName << endl;
	cout << "nFrames: " << nFrames << endl;
	}

//==============================================================================

