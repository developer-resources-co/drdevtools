
#include <iostream.h>
#include <iomanip.h>

#include <stdlib.h>
#include <conio.h>

#include "gfxcomm.hpp"

void
DownloadPicture( char* szChr, char* szMap, int nBackground )
	{
	int i;

//	cout << "Map " << flush;
	PutCommand( CmdGetMap );					// Send map
	PutByte( nBackground );				// Screen #

	ifstream inputMap( szMap, ios::in|ios::binary );
	SendFile( inputMap );
//	cout << endl;

//	cout << "Tiles " << flush;
	PutCommand( CmdGetTiles );					// Send tiles
	PutByte( nBackground );				// Screen #

	ifstream inputChr( szChr, ios::in|ios::binary );
	SendFile( inputChr );
//	cout << endl;

	PutCommand( CmdUpdateDisplay );
	}


main( int argc, char* argv[] )
	{
	if ( argc == 1 )
		{
		cout << "Usage: gfxshow <chrFile> <mapFile> [<BG>=1|2]" << endl;
		return( 10 );
		}

	outportb( 0x37A, 0xFF );

	DownloadPicture( argv[1], argv[2], argc==4 ? atoi( argv[3] )-1 : 0 );

	return( 0 );
	}
