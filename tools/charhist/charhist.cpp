// charhist.cpp -- Character Histogram

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>


int nUsage[ 256 ];
char chPrint[ 256 ];


int
	main( int argc, char *argv[] )
	{
	FILE *fpIn;
	int f;

	if ( argc == 1 )
		{
		printf( "Usage: usage <inputFile>...\n" );
		exit( 0 );
		}

	{ // Initialize printing table
	int i;

	for ( i=0; i<256; ++i )
		chPrint[i] = i;

	// Replace unprintables with "."
	chPrint[7] = chPrint[8] = chPrint[9] =
		chPrint[10] = chPrint[13] = chPrint[26] = '.';
	}


	{ // Initialize histogram usage table
	int i;

	for ( i=0; i<256; ++i )
		nUsage[i] = 0;
	}


	for ( f=1; f<argc; ++f )
		{
		fpIn = fopen( argv[f], "rt" );

		while ( !feof( fpIn ) )
			{
			++nUsage[ fgetc( fpIn ) ];
			}
		}

	{ // Display histogram usage chart
	int i;
	int nLongest, nLargest;

	// Find largest value
	for ( i=0; i<256; ++i )
		if ( nUsage[i] > nLargest )
			nLargest = nUsage[i];

		{ // Find length of largest number
		char szBuffer[10];

		nLongest = sprintf( szBuffer, "%d", nLargest );
		}

	for ( i=0; i<256; ++i )
		{
		int x;

		printf( "%c %3d %02X: [%*d] ", chPrint[i], i, i, nLongest, nUsage[i] );
		for ( x=1; x<62L*nUsage[i]/nLargest; ++x )
			//putchar( 'Û' );
			putchar( 'Ü' );
		putchar( '\n' );
		}
	}

	return( 0 );
	}




