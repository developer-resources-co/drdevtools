//==============================================================================
// filename.cpp:
//==============================================================================

#include <string.h>
#include <stdlib.h>

#include <pclib/filename.hpp>

//==============================================================================

void
	filename::Construct( const char* filename )
	{
	strcpy( fname, filename );
	_splitpath( fname, szDrive, szPath, szFile, szExt );
	}

	filename::filename( const char* filename, const char* szExt )
	{
	Construct( filename );
	if ( !strchr( filename, '.' ) )
		ext( szExt );
	}

//==============================================================================

const char*
	filename::file( const char* szNewFile )
	{
	strcpy( szFile, szNewFile );
	_makepath( fname, szDrive, szPath, szFile, szExt );		// Keep everyone in sync
	return( file() );
	}

//==============================================================================

const char*
	filename::ext( const char* szNewExt )
	{
	strcpy( szExt, szNewExt );
	_makepath( fname, szDrive, szPath, szFile, szExt );		// Keep everyone in sync
	return( ext() );
	}

//==============================================================================
