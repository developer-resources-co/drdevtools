//==============================================================================
// filename.h:
//==============================================================================
/*

Documentation:

	Abstract:

	History:
			Created	? William B. Norris IV
			First documented 10-19-92 02:35pm

	Class Hierarchy:
		none
	Dependancies:
		iostream
		fstream
		stdlib
	Restrictions:
		?
	Example:

*/

//==============================================================================
// use only once insurance

#ifndef PCLIB_FILENAME_H
#define PCLIB_FILENAME_H

//==============================================================================
// dependencies

#include <iostream.h>
#include <fstream.h>

#include <stdlib.h>

//==============================================================================
// class declaration

class filename
	{
private:
	filename();

	char fname[_MAX_PATH];

	char szDrive[_MAX_DRIVE];
	char szPath[_MAX_PATH];
	char szFile[_MAX_FNAME];
	char szExt[_MAX_EXT];

public:
	// Constructors
	void Construct( const char* fname );
	filename( const char* fname )				{ Construct( fname ); }
	filename( const char* fname, const char* szExt );

	// Converters
//	operator ifstream() { return( new ifstream( (const char*)fname ) ); }
//																// Binary switch thang???

	// Methods
	const char* const operator()()     			{ return( fname ); }

	const char* const file( void )				{ return( szFile ); }
	const char* file( const char* szNewFile );

	const char* const ext( void )					{ return( szExt ); }
	const char* ext( const char* szNewExt );
	};

//==============================================================================

#endif

//==============================================================================
