//==============================================================================
// ini.cpp:
//==============================================================================

#include "ini.hpp"

//==============================================================================

	Ini::Ini( char* progName )
	{
	_splitpath( progName, szDrive, szDir, szFile, szExt );

	_makepath( szToolIni, szDrive, szDir, szFile, "ini" );
	strcpy( szToolName, szFile );
	}

//==============================================================================

boolean
	Ini::Boolean( char* szItem, boolean bDefault )
	{
	boolean bReturn;

	bReturn = boolean( GetPrivateProfileInt( szToolName, szItem, int(bDefault), szToolIni ) );

	return( bReturn );
	}

//==============================================================================

int
	Ini::Int( char* szItem, int nDefault )
	{
	int nReturn;

	nReturn = GetPrivateProfileInt( szToolName, szItem, nDefault, szToolIni );

	return( nReturn );
	}

//==============================================================================
