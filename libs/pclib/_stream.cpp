// _stream.cpp

#include <iostream.h>
#include <stdio.h>

#include "compat.hpp"
#include "_stream.hpp"

_streambuf::_streambuf( char* s, int n ) : streambuf( s, n )
	{
	setp( base(), ebuf() );
	}


_streambuf::~_streambuf()
	{
	sync();
	}


void
mstreambuf::cr()
	{
	}


void
_streambuf::newline()
	{
	cr();
	putchar( '\n' );
	}


int
_streambuf::overflow( int nChar )
	{
	sync();
	if ( nChar != EOF )
		return sputc( nChar );
	else
		return nChar;
	}


int
_streambuf::sync()
	{
	int count = out_waiting();

	if ( count )
		{
		for ( int i=0; i<count; ++i )
			{
			char c = pbase()[i];
			if ( c == '\n' )
				newline();
			else if ( c == '\r' )
				cr();
			else
				putchar( c );
			}
		setp( base(), ebuf() );
		}

	return __NOT_EOF;
	}


_stream::_stream() : ostream()
	{
	strbuf = new _streambuf( msgs, bsize );
	ostream::init( strbuf );
	setf( ios::unitbuf );
	}


_stream::~_stream()
	{
	delete strbuf;
	}


#if COMMENT
	_stream _out;				// Create a global instance of the output
#endif
