// strmnull.cpp

#include <iostream.h>

#include "compat.hpp"

#include "strmnull.hpp"

nullstreambuf::nullstreambuf( char* s, int n ) : streambuf( s, n )
	{
	setp( base(), ebuf() );
	}


nullstreambuf::~nullstreambuf()
	{
	sync();
	}


int
nullstreambuf::overflow( int nChar )
	{
	sync();
	if ( nChar != EOF )
		return sputc( nChar );
	else
		return nChar;
	}


int
nullstreambuf::sync()
	{
	if ( out_waiting() )
		setp( base(), ebuf() );

	return __NOT_EOF;
	}


nullstream::nullstream() : ostream()
	{
	strbuf = new nullstreambuf( msgs, bsize );
	ostream::init( strbuf );
	setf( ios::unitbuf );
	}


nullstream::~nullstream()
	{
	delete strbuf;
	}


nullstream cnull;				// Create a global instance of the output
