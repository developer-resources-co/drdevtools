// compress.cpp

#include <iostream.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <signal.h>

#include "version.h"

typedef unsigned char UBYTE;
typedef unsigned long ULONG;
typedef unsigned int BOOL;

ULONG LZSEncode( UBYTE* inbuffer, UBYTE* outbuffer, ULONG inlength );
ULONG LZSDecode( UBYTE* inbuffer, UBYTE* outbuffer, ULONG inlength );

ULONG MapEncode( UBYTE* inbuffer, UBYTE* outbuffer, ULONG inlength );


char* ctrlc_fnOut;
FILE* ctrlc_fpOut;
UBYTE* bufferOut;

void ctrlchandler( int )
	{
//	if ( ctrlc_fpOut ) fclose( ctrlc_fpOut );
//	if ( ctrlc_fnOut ) unlink( ctrlc_fnOut );
	exit( 0 );
	}


const short companySignature = 'CL';
const short compressionType = 1;

void
decompress( char* fnIn, char* fnOut )
	{
	short i;

	printf( "%s: ", fnIn );  fflush( stdout );

	FILE* fpIn = fopen( fnIn, "rb" );
	assert( fpIn );
	fseek( fpIn, 0L, SEEK_END );
	ULONG length = ftell( fpIn );
	fseek( fpIn, 0L, SEEK_SET );
	UBYTE* bufferIn = (UBYTE*)malloc( 65535 );
	assert( bufferIn );
	fread( bufferIn, 1, length, fpIn );
	fclose( fpIn );

	assert( *((short*)bufferIn) == companySignature );
	assert( *((short*)bufferIn+1) == compressionType );

	FILE* fpOut = fopen( fnOut, "wb" );
	assert( fpOut );

	ULONG lengthDecompressed = LZSDecode( bufferIn+4, bufferOut, 65535 );
	cout << endl;
	free( bufferIn );

	fwrite( bufferOut, 1, lengthDecompressed, fpOut );
	fclose( fpOut );
	}


void
statistics( char* fnIn )
	{
	short i;


	printf( "%s: ", fnIn );  fflush( stdout );

	FILE* fpIn = fopen( fnIn, "rb" );
	assert( fpIn );
	fseek( fpIn, 0L, SEEK_END );
	ULONG length = ftell( fpIn );
	fseek( fpIn, 0L, SEEK_SET );
	UBYTE* bufferIn = (UBYTE*)malloc( 65535 );
	assert( bufferIn );
	fread( bufferIn, 1, length, fpIn );
	fclose( fpIn );

	assert( *((short*)bufferIn) == companySignature );
	assert( *((short*)bufferIn+1) == compressionType );

	ULONG lengthDecompressed = LZSDecode( bufferIn+4, bufferOut, 65535 );

	cout << " compressed " << length << endl;

	free( bufferIn );
	}


unsigned short
compress( char* fnIn )
	{
	printf( "%s:\n", fnIn );

	FILE* fpIn = fopen( fnIn, "rb" );
	assert( fpIn );
	fseek( fpIn, 0L, SEEK_END );
	ULONG length = ftell( fpIn );
	fseek( fpIn, 0L, SEEK_SET );

	assert( length <= 65535 );

	UBYTE* bufferIn = (UBYTE*)malloc( 65535 );
	assert( bufferIn );
	fread( bufferIn, 1, length, fpIn );
	fclose( fpIn );

	//ULONG lengthCompressed = LZSEncode( bufferIn, bufferOut, length );
	ULONG lengthCompressed = MapEncode( bufferIn, bufferOut, length );
	free( bufferIn );

	printf( "(%ld/%ld=%ld%%)\n", lengthCompressed, length,
		(lengthCompressed*100/length) );

	return lengthCompressed;
	}


void
write_compressed_data( char* szOutName, unsigned short lengthCompressed )
	{
	// Write "CL" signature
	FILE* fpOut = fopen( szOutName, "wb" );
	assert( fpOut );

	fwrite( &companySignature, 1, 2, fpOut );
	fwrite( &compressionType, 1, 2, fpOut );

	fwrite( bufferOut, 1, lengthCompressed, fpOut );
	fclose( fpOut );
	}


int
main( int argc, char* argv[] )
	{
	if ( argc==1 )
		{
		cout << "Compress  Version " << rmj << '.' << rmm << '.' << rup << endl;
		cout << "Copyright 1994 Cave Logic Studios.  All Rights Reserved." << endl;
		cout << "By William B. Norris IV" << endl;
		cout << endl;
		cout << "Usage: compress [-d] <inFile> <outFile>" << endl;
		return 0;
		}

	bufferOut = (UBYTE*)malloc( 65535 );
	assert( bufferOut );

	// Install Ctrl+C handler
	signal( SIGINT, ctrlchandler );

	if ( stricmp( argv[1], "-d" ) == 0 )
		decompress( argv[2], argv[3] );
	else if ( stricmp( argv[1], "-v" ) == 0 )
		statistics( argv[2] );
	else
		{
		char* szOutName;

		if ( argc == 2 )
			{ // Generate output name based on input
			szOutName = strdup( argv[1] );
			*( szOutName + strlen( szOutName ) - 1 ) = '_';
			}
		else
			szOutName = strdup( argv[2] );

		unsigned short lengthCompressed = compress( argv[1] );

		write_compressed_data( szOutName, lengthCompressed );

		free( szOutName );
		}
	free( bufferOut );

	return 0;
	}
