//==============================================================================
// grabinpt.c
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

#include "abmsplit.h"

//==============================================================================

char AnimName[_MAX_PATH];
char szPictName[_MAX_PATH];

//==============================================================================
// global switches
boolean bPause = boolean::FALSE;

boolean bDisplay;			// = boolean::TRUE;
boolean bPbm;				// = boolean::FALSE;		// Write output as PBM rather than ILBM
boolean bMultipleLbm;	// = boolean::FALSE;
boolean bClipLbmSize;	// = boolean::FALSE;
int nSpacing;				// = 1;						// Spacing between frames
int xLbmSize;				//=320;
int yLbmSize;				//=200;

//==============================================================================

#include "ini.h"

	Ini::Ini( char* progName )
	{
	fnsplit( progName, szDrive, szDir, szFile, szExt );

	fnmerge( szToolIni, szDrive, szDir, szFile, "ini" );
	strcpy( szToolName, szFile );
	}


boolean
	Ini::Boolean( char* szItem, boolean bDefault )
	{
	boolean bReturn;

	bReturn = boolean( GetPrivateProfileInt( szToolName, szItem, int(bDefault), szToolIni ) );

	return( bReturn );
	}


int
	Ini::Int( char* szItem, int nDefault )
	{
	int nReturn;

	nReturn = GetPrivateProfileInt( szToolName, szItem, nDefault, szToolIni );

	return( nReturn );
	}


void GetInput (int argc,char *argv[])
{
	int i = 0;

	// Read all options in before local (command-line) overrides
	// LoadPrefs()
	Ini *prefs = new Ini( argv[0] );

	bDisplay = prefs->Boolean( "Display", boolean::TRUE );
	xLbmSize = prefs->Int( "xOutputSize", 320 );
	yLbmSize = prefs->Int( "yOutputSize", 200 );
	nSpacing = prefs->Int( "Spacing", 1 );
	bClipLbmSize = prefs->Boolean( "Clip", boolean::FALSE );
	bMultipleLbm = prefs->Boolean( "Multiple", boolean::FALSE );
	bPbm         = prefs->Boolean( "PBM", boolean::FALSE );

	delete prefs;


	while ( *argv[i+1] == '-' )
		{
		switch(*(argv[i+1]+1))
			{
			case 'p':
			case 'P':
				bPbm = boolean::TRUE;
				break;

			case 's':
			case 'S':
				sscanf( argv[i+1]+2, "%d", &nSpacing );
				break;

			case 'c':
			case 'C':
				bClipLbmSize = boolean::TRUE;
				break;

			case 'x':
			case 'X':
				sscanf( argv[i+1]+2, "%d", &xLbmSize );
				break;

			case 'y':
			case 'Y':
				sscanf( argv[i+1]+2, "%d", &yLbmSize );
				break;

			case 'm':
			case 'M':
				bMultipleLbm = boolean::TRUE;
				break;

			case 'o':
			case 'O':
				sscanf( argv[i+1]+2, "%s", &szPictName );
				// If no extension, add ".lbm"
				break;

			case 'd':
			case 'D':
				bDisplay = boolean::FALSE;
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

			if ( !*szPictName )
				{
#if 0
				if ( bMultipleLbm )
					sprintf( szPictName, "%4s%%d.lbm", AnimName );
				else
#endif
		        	strcpy( szPictName, AnimName );
				ChangeExtension( szPictName, ".lbm" );
				}

			return;
		 	}

		{ // if here then we are not happy with the format, so just print help
		char szProgName[ _MAX_PATH ];

		_splitpath( argv[0], NULL, NULL, szProgName, NULL );
		strlwr( szProgName );

		printf(
"AnmSplit %s  (c) 1992 Adept Creations.  All Rights Reserved.\n"
"IFF animation to picture converter\n"
"By William B. Norris IV\n"
"Based on libraries by Kevin T. Seghetti and William B. Norris IV\n\n"
//"Usage: %s -x -y -c -m -ofilePattern[.lbm] -p -d <inputAnim>[.abm]\n"
"Usage: %s -x -y -c -m -ofilePattern[.lbm] -p -s# <inputAnim>[.abm]\n"
"", szVersion, szProgName );

//        1         2         3         4         5         6         7			  8
//2345678901234567890123456789012345678901234567890123456789012345678901234567890
	cout <<
//"    -x:  x-size of output .lbm         -y:  y-size of output .lbm\n"
"    -x:  x-size of output .lbm\n"
"    -y:  y-size of output .lbm\n"
"    -c:  clip output .lbm to input size\n"
"    -m:  write output to multiple files\n"
"    -o:  write output to specified name\n"
"    -p:  write output as IFF PBM\n"
"    -s#: spacing (in pixels) between brushes\n"
//"    -d:  display off\n"
#if 0
	d: disable display
	p: output FORM PBM
	m: multiple .lbm's
	?: wait for keypress (p/w/k??)
#endif
		;
 		exit(0);
 		}
	}
	}

//==============================================================================

