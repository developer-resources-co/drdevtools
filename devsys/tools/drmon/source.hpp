//==============================================================================
// source.hpp: source debug window header
//==============================================================================

//==============================================================================
// include only once insurance

#ifndef DRMON_source_H
#define DRMON_source_H

extern char *sourcePath;
extern void SourceRoutine(_object *oPtr);

//==============================================================================

class _source : public _object
{
public:
	char currentFileName[80];

		_source() : _object( SourceRoutine )
		 {
			currentFileName[0] = NULL;
			_object* o2Ptr = GetLastObject();
			Prev( o2Ptr );
			o2Ptr->Next( this );
		 }
};

//============================================================================
// source commands
enum
{
	SOURCE_LOAD = 1,
	SOURCE_SEARCH,
	SOURCE_SEARCHLABEL,
	SOURCE_NEXT,
	SOURCE_GOTO,

	SOURCE_SETBREAK,
	SOURCE_SETBREAKONCE,
	SOURCE_CLEARBREAK,
	SOURCE_CLEARALLBREAKS,
	SOURCE_RUNTOHERE,
	SOURCE_GOTOSYMBOL
};


//==============================================================================

#endif

//==============================================================================
