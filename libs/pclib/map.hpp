/////////////////////////////////////////////////////////////////////////////
//
// File:			map.h
//
// Project:		Developer Resources Tools Class Libraries
//
// 19 Aug 92	WBNIV		Created
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(CLASSLIB_MAP_H)
#define CLASSLIB_MAP_H

#include <fstream.h>

#include <stdlib.h>
#include <string.h>

#include <pclib/general.hpp>
#include <pclib/tile.hpp>

class map
	{
private:
	uint xs, ys;
	uint xOffset, yOffset;
	tile *buffer;								// far
	char szFilename[_MAX_PATH];				// filename

	map();

public:
	void Construct( int xs, int ys );
	~map();

	map( int xs, int ys )      	{ Construct(xs,ys); }
	map( istream& istr );

	boolean read( istream& istr );
	boolean const write( ostream& ostr );

	inline tile const data(uint x, uint y);
	inline tile data(uint x, uint y, tile& t);

	const uint xSize( void )     { return( xs ); }
	const uint xSize( uint xs )  { return( map::xs=xs ); }

	const uint ySize( void )     { return( ys ); }
	const uint ySize( uint ys )  { return( map::ys=ys ); }

	const char* name( void )     { return( szFilename ); }
	const char* name( const char* szFilename )
	                 { return( strcpy( map::szFilename, szFilename ) ); }
	};

#endif

