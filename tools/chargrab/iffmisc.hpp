//==============================================================================
// iffmisc.hpp:  header for misc chunks
//==============================================================================
/*

Documentation:

	Abstract:

	History:
			Created	30 Nov 93 William B. Norris IV

	Class Hierarchy:

	Dependancies:
			iff

	Restrictions:

	Example:

*/
//==============================================================================

#ifndef PCLIB_misc_H
#define PCLIB_misc_H

//==============================================================================
// dependancies

#include <string.h>
#include <assert.h>
#include <pclib/iff.hpp>

//==============================================================================
// this should be inside the class

const iffTag tagANNO = MakeTag('A','N','N','O');

//==============================================================================
// class declaration

class iffANNO
	{
private:
	char* annotation;

	errorcode Write( iffWrite* pWrite );
	errorcode Read(iffRead *readPtr);

public:
	// File I/O
	friend ostream& operator<<( ostream& s, iffANNO& anno );
	friend iffWrite& operator<<( iffWrite& s, iffANNO& anno )
		{ anno.Write( &s );  return s; }
	friend iffRead& operator>>( iffRead& s, iffANNO& anno )
		{ anno.Read( &s );  return s; }

	iffANNO() { annotation = NULL; }
	iffANNO( char* s ) { annotation = strdup( s ); assert( annotation ); }
	iffANNO( iffRead* pRead ) { *pRead >> *this; }
	~iffANNO()	{ free( annotation ); }
	};

//==============================================================================

#endif

//==============================================================================
