//==============================================================================
// pbm.h: header for fucked pbm parser built on iff parser
//==============================================================================
/*

Documentation:

	Abstract:
		IFF PBM reader
	History:
			Created	? Kevin T. Seghetti
			First documented 10-19-92 02:16pm

	Class Hierarchy:

	Dependancies:
		general
		iff
		ilbm
	Restrictions:

	Example:

*/
//==============================================================================
// use only once insurance

#ifndef PCLIB_pbm_H
#define PCLIB_pbm_H

//==============================================================================
// Dependancies

#include <pclib/general.hpp>
#include <pclib/iff.hpp>
#include <pclib/iffilbm.hpp>

//==============================================================================
// globals

const iffTag tagPBM = MakeTag('P','B','M',' ');

//==============================================================================
// class declaration

class pbmBODY
{
	ilbmBMHD *bmhd;
	pbmBODY() { };					// cannot create without bmhd pointer
public:
	pbmBODY(ilbmBMHD *newBMHD) {  bmhd = newBMHD; }
	errorcode Read(iffRead *readPtr, errorcode parseLine(ubyte *line));
};

//==============================================================================

#endif

//==============================================================================
