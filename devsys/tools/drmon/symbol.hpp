//=============================================================================
//  symbol.h: symbol header file
//=============================================================================

#ifndef DRMON_symbol_HPP
#define DRMON_symbol_HPP

//=============================================================================

#include "reg.hpp"
#include "menu.hpp"

//=============================================================================

#include "coff.hpp"

extern _object *symbolObjPtr;
extern struct _symbolList symbolListBase;

class _symbolList : public _stringList
{
public:
	_symbolList() : _stringList() { addr = 0; ivType = 0; ivStorageClass = 0;
#ifdef DEBUGCOFF
	scope = NULL;
#endif
	}

//	_stringList list;

	~_symbolList()
	 {
		if(symbolObjPtr)
			ChangeListRect((_window*)symbolObjPtr->layer,(_listRectDesc *)symbolObjPtr->dataPtr3,(_stringList *)&symbolListBase);
	 }

	uword Type() { return(ivType); }
	uword Type( uword newType ) { ivType = newType; return(ivType); }

	byte StorageClass() { return(ivStorageClass); }
	byte StorageClass(byte newStorageClass ) { ivStorageClass = newStorageClass; return(ivStorageClass); }

	ULONG *AddressAddr(void) { return(&addr); }

	ULONG Eval();
	ULONG Address(void) { return( Eval() ); }
	ULONG Address(ULONG newAddr) { addr = newAddr; return(addr); }

	boolean InScope( ULONG addr );
	boolean InScope()	{ return( InScope( GetReg(REG_PC) ) ); }

	void SymbolClear(void);

#ifdef DEBUGCOFF
	Scope* scope;
#endif

private:
	ULONG addr;						// address of symbol
	// New COFF stuff
	uword ivType;
	byte ivStorageClass;
};

//==============================================================================

extern FLAG symbolOpen;

//==============================================================================

_symbolList* AddSymbolQuick(ULONG addr, char *text, uword type=0, byte storageClass=C_LABEL );
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
