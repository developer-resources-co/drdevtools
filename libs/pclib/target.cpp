//============================================================================
// target.cpp (c) 1992 Adept Creations.  All Rights Reserved.
// by William B. Norris IV
//============================================================================

#include <stdlib.h>
#include <ctype.h>

#include <pclib/target.hpp>
#include <pclib/profile.hpp>

	TargetSystem::TargetSystem( systemType st )
	{
	char *szSystemDir;

	if ( st == SYS_AUTODETECT )
		{
		char szDefaultSystem[80+1];

		st = SYS_DEFAULT;			// in case getenv() doesn't work or no match found

		if ( szSystemDir = getenv( "DR_SYSTEM" ) )
			GetPrivateProfileString( "System", "System", "Genesis", szDefaultSystem, 80, szSystemDir );

		switch ( tolower(*szDefaultSystem) )
			{
			case 's':
				st = SYS_SNES;
				break;

			case 'g':
				st = SYS_GENESIS;
				break;
			}
		}

	// Initialize system based on passed system value or calculated system
	switch ( _system=st )
		{
		case SYS_SNES:
			szPlatformName = "SNES";
			break;

		case SYS_GENESIS:
			szPlatformName = "Genesis";
			break;
		}
	}


////////////////////////////////////////////////////////////////////////////////
//
// File I/O Routines
//

size_t
	TargetSystem::write( ostream& os, ubyte b, size_t n )
	{
	os.write( (char*)&b, n );
	return( n );
	}


size_t
	TargetSystem::write( ostream& os, uword w, size_t n )
	{
	os.write( (char*)&w, n*sizeof(uword) );
	return( n );
	}


size_t
	TargetSystem::write( ostream& os, ulong l, size_t n )
	{
	os.write( (char*)&l, n*sizeof(ulong) );
	return( n );
	}


size_t
	TargetSystem::read( istream& is, ubyte b, size_t n )
	{
		return(0);
	}


size_t
	TargetSystem::read( istream& is, uword w, size_t n )
	{
		return(0);
	}


size_t
	TargetSystem::read( istream& is, ulong l, size_t n )
	{
		return(0);
	}


