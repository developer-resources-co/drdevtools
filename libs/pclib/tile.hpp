/////////////////////////////////////////////////////////////////////////////
//
// File:			tile.h
//
// Project:		Adept Creations Tools Class Libraries
//
// 19 Aug 92	WBNIV		Created 
//
/////////////////////////////////////////////////////////////////////////////

#ifndef CLASSLIB_TILE_H
#define CLASSLIB_TILE_H

#include <iostream.h>

#include <pclib/general.hpp>

class tile
	{
private:
	union {
		int data;
		struct {
			int nName : 10;
			int nPalette : 3;
			int bBgPri : 1;
			int bHflip : 1;
			int bVflip : 1;
			} bits;
		};

public:
	tile( istream& istr )	{ read( istr ); }
	tile( )  					{ data=0; };

	void read( istream &istr )		{
		int i;
		istr.read( (ubyte*)&i, sizeof(int) );
		data=i;						//?	istr.read( (char*)&data, sizeof(int) );
		};
	void write( ostream& ostr )	{ 
		int i = data;
		ostr.write( (char*)&i, sizeof(int) );
		};

	const int operator()()	{ return( data ); };

	// "bit" manipulations
	int vflip( )					{ return( bits.bVflip ); }
	int vflip( int onOff )		{ bits.bVflip = onOff;  return( vflip() ); }

	int hflip( )					{ return( bits.bHflip ); }
	int hflip( int onOff )		{ bits.bHflip = onOff;  return( hflip() ); }

	int bgpri( )					{ return( bits.bBgPri ); }
	int bgpri( int onOff )		{ bits.bBgPri = onOff;  return( vflip() ); }

	int pal( )						{ return( bits.nPalette ); }
	int pal( int nPalette )		{ bits.nPalette = nPalette;  return( pal() ); }

	int name( )						{ return( bits.nName ); }
	int name( int nName )		{ bits.nName = nName;  return( name() ); }
	};

#endif

