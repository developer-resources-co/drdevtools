//==============================================================================
// iffmisc.cpp: funcions for misc parser built on iff parser
//==============================================================================

#include <iomanip.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <pclib/error.hpp>
#include "iffmisc.hpp"

//==============================================================================

ostream& operator<<( ostream& s, iffANNO& anno )
	{
	s << "[ ANNO ]" << endl;
	s << anno.annotation << endl;
	return s;
	}


errorcode
iffANNO::Read( iffRead* pRead )
	{
	return NOERR;
	}


errorcode
iffANNO::Write( iffWrite* pWrite )
	{
	pWrite->NewChunk( tagANNO );
	pWrite->WriteChunkData( annotation, strlen( annotation ) );
	}

//==============================================================================
