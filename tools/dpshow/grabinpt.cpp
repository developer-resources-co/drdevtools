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

char Input1Name[_MAX_PATH], Input2Name[_MAX_PATH];
char OutputName[_MAX_PATH];

//==============================================================================
// global switches
boolean bDisplay = boolean::TRUE;
boolean bPause = boolean::FALSE;

//==============================================================================

void GetInput (int argc,char *argv[])
{
	int i = 0;

	while ( *argv[i+1] == '-' )
		{
		switch(*(argv[i+1]+1))
			{
			default:
				break;
			}
		}

		{
		if(argc==4+i)
			{
     	strcpy( Input1Name, argv[1+i] );
			strcpy( Input2Name, argv[2+i] );
			strcpy( OutputName, argv[3+i] );
			return;
		 	}

		{ // if here then we are not happy with the format, so just print help
		char szProgName[ _MAX_PATH ];

		_splitpath( argv[0], NULL, NULL, szProgName, NULL );
		strlwr( szProgName );

		printf(
"DPShow %s  (c) 1992,93 Developer Resources.  All Rights Reserved.\n"
"Deluxe Paint Viewer\n"
"By William B. Norris IV\n"
" Based on libraries by Kevin T. Seghetti and William B. Norris IV\n"
" (c) 1992,93 Developer Resources.  All Rights Reserved.\n\n"
"Usage: %s <inputAnim>\n"
"", szVersion, szProgName );

//        1         2         3         4         5         6         7			  8
//2345678901234567890123456789012345678901234567890123456789012345678901234567890
//	cout <<
//"    -x:  x-size of output .lbm         -y:  y-size of output .lbm\n"
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

