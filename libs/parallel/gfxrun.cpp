
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>

#include <stdlib.h>
#include <conio.h>

#include "gfxcomm.hpp"


void
DownloadCode( char* szInput, unsigned long address )
	{
	ifstream input( szInput, ios::in|ios::binary );

	PutCommand( CmdDownloadCode );						// "Download code"
	PutLong( address );

	SendFile( input );

	PutCommand( CmdExecuteCode );						// "Execute code"
	PutLong( address );
	}


main( int argc, char* argv[] )
	{
	if ( ! ( argc == 2 || argc == 3 ) )
		{
		cout << "Usage: gfxrun <binaryfile> [<address>=0]" << endl;
		exit( 10 );
		}

	outportb( 0x37A, 0xFF );

	DownloadCode( argv[1], argc==3 ? atoi( argv[2] ) : 0 );

	return( 0 );
	}
