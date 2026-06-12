//=============================================================================
//  symbol.h: symbol header file
//=============================================================================

#ifndef DRMON_symbol_HPP
#define DRMON_symbol_HPP

//=============================================================================

#include "reg.hpp"
#include "menu.hpp"

//=============================================================================

extern _object *symbolObjPtr;
extern struct _symbolList symbolListBase;

class _symbolList : public _stringList
{
public:
	_symbolList() : _stringList() { addr = 0;
	}

//	_stringList list;

	~_symbolList()
	 {
		if(symbolObjPtr)
			ChangeListRect((_window*)symbolObjPtr->layer,(_listRectDesc *)symbolObjPtr->dataPtr3,(_stringList *)&symbolListBase);
	 }

	ULONG *AddressAddr(void) { return(&addr); }

	ULONG Eval();
	ULONG Address(void) { return( Eval() ); }
	ULONG Address(ULONG newAddr) { addr = newAddr; return(addr); }

	boolean InScope( ULONG addr );
	boolean InScope()	{ return( InScope( GetReg(REG_PC) ) ); }

	void SymbolClear(void);

private:
	ULONG addr;						// address of symbol
};

//==============================================================================

extern FLAG symbolOpen;

//==============================================================================

_symbolList* AddSymbolQuick(ULONG addr, char *text);
errorcode AddSymbol(ULONG addr,char *string);
errorcode AssignSymbol(char *name, ULONG value);
errorcode SymbolNameClear(char *name);

void SymbolClearAll( void );
void SymbolClearAll(_menuItem *iPtr,_object *oPtr,int choice);

void InitSymbols(void);

_symbolList *FindSymbol(char *text);

_symbolList *FindHexSymbol(ULONG value);

errorcode LoadSymbol( FILE* );
errorcode LoadSymbol( char* fileName );

void SortSymbols(_object *oPtr);

//============================================================================
// symbol commands
enum
{
	SYMBOL_SETBREAK = 1,
	SYMBOL_SETBREAKONCE,
	SYMBOL_LOAD,
	SYMBOL_SAVE,
	SYMBOL_SET,
	SYMBOL_CLEAR,
	SYMBOL_CLEARALL
};

//=============================================================================

#endif

//============================================================================
