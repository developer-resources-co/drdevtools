//============================================================================
//	cardsave.cpp: save binary card data to disk
//============================================================================

#include <iostream.h>
#include <fstream.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <alloc.h>
#include <dos.h>

#include <
#include "version.h"


typedef struct {
	char OutputName[ _MAX_FNAME ];
	} CardSaveOptions;

void GetInput( int argc, char* argv[], CardSaveOptions* );


void
	main( int argc, char *argv[] )
	{
	CardSaveOptions csOpt;

	GetInput( argc, argv, &csOpt );

	cout << "OutputName: " << csOpt.OutputName << endl;

//	DevelopmentSystemBoard devSys();

	ofstream* output = new ofstream( csOpt.OutputName, ios::out|ios::binary );
	if ( !output )
		{
		cerr << "Unable to open output file \"" << csOpt.OutputName << "\"" << endl;
		exit( 10 );
		}

	for ( ulong i=1024*1024L; i; i -= 32768 )
		{

		}

	delete output;

	unlink( csOpt.OutputName );
	}


void GetInput (int argc,char *argv[], CardSaveOptions* csOpt )
{
	for ( int i=1; argv[i] && (*argv[i] == '-'); ++i )
		{
		switch(*(argv[i]+1))
			{
#if 0
			case 's':
				bGenesis = boolean::FALSE;
				break;

			case 'F':
				sscanf( argv[i]+2, "%d", &nFrame );
				break;
#endif

			default:
				cerr << "Unrecognized command line switch \"" <<
					*(argv[i]+1) << "\"" << endl;
				break;
			}
		}

	if ( argc >= 1+i )
		{
		strcpy( csOpt->OutputName, argv[i] );
		if ( !strrchr( csOpt->OutputName, '.' ) )
			strcat( csOpt->OutputName, ".bin" );
		}

	if ( argc == i )
		{ // if here then we are not happy with the format, so just print help
		char szProgName[ _MAX_FNAME ];

		_splitpath( argv[0], NULL, NULL, szProgName, NULL );
		strlwr( szProgName );

		cout <<
/*ts->PlatformName() <<*/ "cardsave  v" << rmj << '.' << rmm << '.' << rup << " Copyright 1994 Cave Logic Studios.  All Rights Reserved.\n"
"Save card binary image to disk\n"
"By William B. Norris IV\n\n"
"Usage: " << szProgName << " [-switches] <filename[.bin]>\n";

//        1         2         3         4         5         6         7			  8
//2345678901234567890123456789012345678901234567890123456789012345678901234567890
		cout << "";

 		exit(0);
 		}
	}
