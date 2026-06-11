//=============================================================================
// search.hpp
//=============================================================================

// A search hit: a list node (mirrors _breakList / _symbolList) carrying the
// matched address.  Inherits _stringList so it drops straight into the list-rect
// widget the symbol/break windows use.
class _searchList : public _stringList
{
public:
	_searchList() : _stringList() { addr = 0; }
	ULONG addr;						// address of a search hit
	ULONG Address() { return(addr); }
	ULONG Address(ULONG newAddr) { addr = newAddr; return(addr); }
};

// Search-window messages (menuItems userData -> SendWindowMessage -> in->message).
enum
{
	SEARCH_GOTO = 1,
	SEARCH_CLEAR
};

//=============================================================================

FLAG OpenSearchWindow(void);

extern _object *searchObjPtr;

//=============================================================================
