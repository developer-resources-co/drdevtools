// mstream.cpp

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <dos.h>

#include "compat.hpp"

#include "mstream.hpp"

const CPL = 80;
const LPP = 25;
const ATTR = 0x700;

mstreambuf::mstreambuf( char* s, int n ) : streambuf( s, n )
	{
	setp( base(), ebuf() );

	if ( !screen )
		screen = (unsigned short*)MK_FP( 0xB000, 0 );

	for ( int i=0; i<CPL*LPP; ++i )
		*(screen + i) = ' ' + ATTR;

	column = row = 0;
	}


mstreambuf::~mstreambuf()
	{
	sync();
	}


void
mstreambuf::newline()
	{
	column = 0;
	++row;

	assert( row <= LPP );

	if ( row == LPP )
		{
		memmove( screen, &screen[CPL], (CPL*(LPP-1))*2 );
		for ( int i=CPL*(LPP-1); i < CPL*LPP; ++i )
			screen[ i ] = ATTR + ' ';
		--row;
		}
	}


int
mstreambuf::overflow( int nChar )
	{
	sync();
	if ( nChar != EOF )
		return sputc( nChar );
	else
		return nChar;
	}


int
mstreambuf::sync()
	{
	int count = out_waiting();

	if ( count )
		{
		for ( int i=0; i<count; ++i )
			{
			char c = pbase()[i];

			if ( column == CPL )
				newline();

			if ( c == '\n' )
				newline();
			else
				screen[ (CPL*row) + column++ ] = c + ATTR;
			}
		setp( base(), ebuf() );
		}

	return 0;
	}


mstream::mstream() : ostream()
	{
	strbuf = new mstreambuf( msgs, bsize );
	ostream::init( strbuf );
	setf( ios::unitbuf );
	}


mstream::~mstream()
	{
	delete strbuf;
	}


unsigned short* mstreambuf::screen = NULL;
int mstreambuf::column = 0;
int mstreambuf::row = 0;


	mstream mout;

