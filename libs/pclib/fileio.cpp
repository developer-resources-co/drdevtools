//=============================================================================
// fileio.cpp:
//=============================================================================

#include <stdlib.h>
#include <stdio.h>

//=============================================================================

long
	ffilesize( FILE *fp )
	{
	long lSavedPos, lFileSize;

	lSavedPos = ftell( fp );
	fseek( fp, 0, SEEK_END );
	lFileSize = ftell( fp );
	fseek( fp, lSavedPos, SEEK_SET );

	return( lFileSize );
	}

//=============================================================================

char *
	ChangeExtension(char *string,const char *newExt)
	{
	char szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFile[_MAX_FNAME];

	_splitpath( string, szDrive, szDir, szFile, NULL );
	_makepath( string, szDrive, szDir, szFile, newExt );
	return( string );
	}

//=============================================================================
