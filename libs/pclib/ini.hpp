//==============================================================================
// ini.hpp:
//==============================================================================

#ifndef PCLIB_INI_H
#define PCLIB_INI_H

#include <stdlib.h>
#include <string.h>

#include <pclib/boolean.hpp>
#include <pclib/profile.hpp>

//==============================================================================

class Ini
	{
public:
	Ini( char* progName );
//	Ini( char* progName, char*  );

	boolean Boolean( char* szItem, boolean bDefault );
	int Int( char* szItem, int nDefault );
	char* String( char* szItem, char* szDefault );

private:
	// --- Need to use filename class!
	char szToolIni[ 80 ];
	char szToolName[ 50 ];

	char szDrive[10];
	char szDir[60];
	char szFile[10];
	char szExt[4];
	};

//==============================================================================

#endif

//==============================================================================

