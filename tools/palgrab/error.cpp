//==============================================================================
// error.cpp: bad error handling code
//==============================================================================

#include <iostream.h>
#include <stdio.h>
#include <stdarg.h>

#include <pclib/general.hpp>
#include <pclib/error.hpp>

//==============================================================================

uchar * errorText[] =
{
	"No Error",
	"Out of memory",
	"File I/O error",
	"Invalid command-line option",
	"Graphics mode unavailable"
};

//==============================================================================

void Error(errorcode error)
{
	cerr << "Error: " << errorText[error] << '\n';
	Quit(error);
}

//==============================================================================

void
	Error( errorcode error, const char *fmt, ... )
	{
	if ( fmt )
		{
		char szBuffer[ 500 ];
		va_list argptr;

		va_start(argptr, fmt);
		vsprintf(szBuffer, fmt, argptr);
		va_end(argptr);

		cerr << "Error: " << errorText[error];
		cerr << szBuffer;				// actually print it to stderr
		cerr << '\n';
		Beep();
		}

	Quit(error);
	}

//==============================================================================
