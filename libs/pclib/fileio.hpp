//==============================================================================
// fileio.h:
//==============================================================================
/*

Documentation:

	Abstract:
		old library interface, should soon be obsolete
	History:
			Created	? William B. Norris IV
			First documented 10-19-92 02:18pm

	Class Hierarchy:

	Dependancies:

	Restrictions:

	Example:

*/
//==============================================================================
// use only once insurance

#if !defined(GFXTOOLS_fileio_H)
#define GFXTOOLS_fileio_H

//==============================================================================
// dependancies

#include <stdio.h>
#include <pclib/general.hpp>

//==============================================================================
// 

long ffilesize( FILE * );
//boolean LoadFile( void **ptr, LPCSTR lpszFilename, long lOffset );
char *ChangeExtension( char *, const char * );

//==============================================================================

#endif

//==============================================================================
