/////////////////////////////////////////////////////////////////////////////
//
// File:			map.cpp
//
// Project:		Adept Creations Tools Class Libraries
//
// 	19 Aug 92	WBNIV		Created
//
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include <lib/general.hpp>

#include <pclib/tile.hpp>
#include <pclib/map.hpp>


void
	map::Construct( int xs, int ys )
	{
	map::xs = xs;
	map::ys = ys;
	xOffset = yOffset = 0;

//	buffer = (tile *)SafeFarMalloc( xSize() * ySize() * sizeof(tile) );

	buffer = new tile[ xSize() * ySize() ];

	szFilename[0] = '\0';
	}


	map::map( istream& istr )
	{
	uint xMapSize, yMapSize;

	istr.read( (ubyte*)&xMapSize, sizeof(uint) );
	istr.read( (ubyte*)&yMapSize, sizeof(uint) );

	Construct( xMapSize, yMapSize );

	for ( uint y=0; y<yMapSize; ++y )
		{
		for ( uint x=0; x<xMapSize; ++x )
			{
			tile t( istr );
			data( x, y, t );
			}
		}
	}


	map::~map()
	{
	delete[] buffer;
	}


// Same stuff as above, figure out way to "merge"
boolean
	map::read( istream& istr )
	{
	istr.read( (ubyte*)&xs, sizeof(uint) );
	istr.read( (ubyte*)&ys, sizeof(uint) );

	for ( uint y=0; y<ySize(); ++y )
		{
		for ( uint x=0; x<xSize(); ++x )
			{
			tile t( istr );
			data(x,y,t);
			}
		}

	return( boolean::TRUE );
	}


boolean const
	map::write( ostream& ostr )
	{
	int xMapSize = xSize();
	int yMapSize = ySize();

	ostr.write( (ubyte*)&xMapSize, sizeof(uint) );
	ostr.write( (ubyte*)&yMapSize, sizeof(uint) );

	for ( uint y=0; y<ySize(); ++y )
		{
		for ( uint x=0; x<xSize(); ++x )
			{
			tile t = data(x,y);
			t.write( ostr );
			}
		}

	return( boolean::TRUE );
	}


tile const
	map::data( uint x, uint y )
	{
	tile t;

	t = *(buffer + y*xs + x);
	return( t );
	}


tile
	map::data( uint x, uint y, tile& t )
	{
	*(buffer + y*xs + x) = t;
	return( t );
	}




