//=============================================================================
//  debug.h: status display window header
//=============================================================================

//extern FLAG statusOpen;
//extern int statusLinesSaved;
//extern int statLineCount;

struct _statText
	{
	_list link;
	char *textPtr;
	unsigned char charAttr;
	};

extern _statText statTextBase;

//=============================================================================

void
StatusRoutine(_object *oPtr);

void
AddStatusGadgets(_object *oPtr);

FLAG
StatusInput(_input *in,_object *oPtr);

_statText *
NewStatLine( char* text, unsigned char attr );

void
DeleteStatLine(void);

void PrintToConsoleWindow( char* text, ... );
#define PrintToStatWindow	PrintToConsoleWindow

//=============================================================================
