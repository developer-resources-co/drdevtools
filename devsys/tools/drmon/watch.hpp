//=============================================================================
// watch.hpp
//=============================================================================

class _watchList : public _stringList
{
public:
	_watchList() : _stringList() { expr = NULL; }
//	_stringList list;
	char *expr;					// expression to evaluate
};

//=============================================================================

extern void WatchRoutine(_object *oPtr);

//============================================================================
// watch commands
enum
{
	WATCH_INSERT = 1,
	WATCH_DELETE,
	WATCH_CLEARALL,
};

//=============================================================================

class _watch : public _object
{

	boolean symbols;
public:

	_watch() : _object(WatchRoutine) { symbols = boolean::FALSE; }
	~_watch() { }

	boolean PrintSymbols(void) { return(symbols); }
	boolean PrintSymbols(boolean newPrint) { symbols = newPrint; return(symbols); }

};

//=============================================================================

