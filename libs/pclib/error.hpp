//=============================================================================
// error.h: header file for error handling code
//=============================================================================
/*

Documentation:

	Abstract:
		global error handler, this is just until we get throw/catch

	History:
			Created	? Kevin T. Seghetti & William B. Norris IV
			First Documented 10-19-92 02:13pm

	Class Hierarchy:
		none
	Dependancies:
		none
	Restrictions:

	Example:


*/
//==============================================================================
// use only once insurance

#ifndef PCLIB_error_H
#define PCLIB_error_H

//==============================================================================
// global declarations

typedef int errorcode;

enum
{
	ERROR_UNDEFINED=-1,
	NOERR,
	ERROR_NOMEM,
	ERROR_FILEIO,
	ERROR_INVALID_OPTION,				// invalid command line option
	ERROR_GFXMODE_UNAVAILABLE
};

//==============================================================================

void Error(errorcode error);
void Error( errorcode error, const char *fmt, ... );

//==============================================================================

#endif

//==============================================================================
