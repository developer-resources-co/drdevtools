//==============================================================================
// iffanim.hpp: header for anim parser built on iff parser
//==============================================================================
/*

Documentation:

	Abstract:

	History:
			Created	? William B. Norris IV
			First documented 11-18-92 02:08am

	Class Hierarchy:

	Dependancies:
			iff
			iffilbm

	Restrictions:

	Example:

*/
//==============================================================================
// use only once insurance

#ifndef PCLIB_ANIM_H
#define PCLIB_ANIM_H

//==============================================================================
// dependancies

#include <pclib/iff.hpp>
#include "iffilbm.hpp"

//==============================================================================
// this should be inside the class

const iffTag tagANIM = MakeTag('A','N','I','M');

const iffTag tagANHD = MakeTag('A','N','H','D');
const iffTag tagDLTA = MakeTag('D','L','T','A');

//==============================================================================
// class declaration

class animANHD
	{
public:
	ubyte operation;
	ubyte mask;
	uword w, h;
	word x, y;
	ulong abstime;
	ulong reltime;
	ubyte interleave;
	ubyte pad0;
	ulong bits;
	ubyte pad[16];

	// Constructors
	animANHD()	{ }
	animANHD( iffRead* input )	{ Read( input ); }

	errorcode Read( iffRead* readPtr );
	errorcode Write( iffWrite* pWrite );

	// Printing
	friend ostream& operator<<(ostream& s, animANHD& anhd );
	};


class animDLTA
	{
	animANHD *anhd;
	animDLTA() { };
public:
	animDLTA( animANHD* newANHD, iffRead* readPtr, rastGraphPort* rp );

//	errorcode Read( iffRead* readPtr, errorcode parseLine( ubyte* line ) );
//	errorcode Write( iffWrite* pWrite, errorcode parseLine( ubyte* line ) );
	};

#endif

//==============================================================================
