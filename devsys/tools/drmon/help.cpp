//=============================================================================
//	help.cpp:
//=============================================================================

#include "moninc.hpp"
#include "help.hpp"
#include "textview.hpp"

//=============================================================================
// pass NULL to searchString if no search is desired

boolean
OpenRefFile(char *extension,char *title,char *searchString)
{
	char *t;
	_object *oPtr;
	errorcode error = NOERR;
	char fileName[_MAX_PATH+13];

	strcpy(fileName,progName);
	t = FindNull(fileName) - 3;
	strcpy(t,extension);

#ifdef SNES
	oPtr = OpenNamedTextFile(fileName,title,67,20);
#endif
#ifdef GENESIS
	oPtr = OpenNamedTextFile(fileName,title,60,20);
#endif

	if(oPtr && searchString)
	 {
		error = TextViewFindLabel(oPtr,searchString);
		if(error)
			PrintError(error);
	 }
	return(error);
}

//=============================================================================
