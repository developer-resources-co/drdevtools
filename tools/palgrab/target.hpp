//============================================================================
// target.cpp (c) 1992,93 Adept Creations.  All Rights Reserved.
// by William B. Norris IV
//============================================================================
/*

Documentation:

	Abstract:

	History:
			Created	? William B. Norris IV
			First documented 10-19-92 02:37pm

	Class Hierarchy:
		none

	Dependencies:
		general

	Restrictions:
		?
	Example:

*/

//==============================================================================
// use only once insurance

#if !defined(PCLIB_target_H)
#define PCLIB_target_H

//==============================================================================
// dependencies

#include <pclib/general.hpp>

//==============================================================================
// class declaration

class TargetSystem
	{
public:
	enum systemType {
		SYS_AUTODETECT,
		SYS_GENESIS,
		SYS_SNES,

		SYS_DEFAULT = SYS_GENESIS
		};

	TargetSystem( systemType st = SYS_AUTODETECT );

	enum systemType system()  { return( _system ); }

	// Name of development system platform (suitable for displaying to user)
	const char *PlatformName( void )  { return( szPlatformName ); }

	// File I/O routines
	size_t write( ostream& os, ubyte b, size_t n=1 );
	size_t write( ostream& os, uword w, size_t n=1 );
	size_t write( ostream& os, ulong l, size_t n=1 );

	size_t read( istream& is, ubyte b, size_t n=1 );
	size_t read( istream& is, uword w, size_t n=1 );
	size_t read( istream& is, ulong l, size_t n=1 );

	// palette I/O routines
		// read from stream
		// write to stream
		// validate/restrict
private:
	enum systemType _system;
	char *szPlatformName;
	boolean bByteSwap;
	};

//==============================================================================

#endif

//==============================================================================
