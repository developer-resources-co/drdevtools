//=============================================================================
//  symbol.cpp: symbol handling
//=============================================================================
// symbolsSortBy = sort by, 0 = none, 1 = alpha, 2  = numerical

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include	"moninc.hpp"
#include	"word.hpp"	/* allow reserved words */
#include	"display.hpp"

#include "coff.hpp"

//=============================================================================

#define	SYGAD_UP	GAD_USER+1
#define SYGAD_DOWN SYGAD_UP+1
#define SYGAD_SCROLL SYGAD_DOWN+1
#define SYGAD_SBOX SYGAD_SCROLL+1
#define SYGAD_LIST SYGAD_SBOX+1
#define BWIN_YSIZE 10

//=============================================================================

_symbolList symbolListBase;

char symbolName[] = "Symbol file";   // produced by SPASM/65816";

FLAG symbolOpen = boolean::FALSE;
_object *symbolObjPtr;
int symbolSortBy = 0;

//=============================================================================

void
InitSymbols()
{
	int i;
	_symbolList *listNode;
	InitListBase((_list *)&symbolListBase);
}

//=============================================================================

////////////////////////////////////////////////////////////////////////////
//
// Functions which work on a single symbol entry
//
////////////////////////////////////////////////////////////////////////////

boolean
_symbolList::InScope( ULONG addr )
	{
#ifdef DEBUGCOFF
	if ( scope )
		return( scope->InScope( addr ) );
	else
		return( boolean::TRUE );
//	return( scope ? scope->InScope( addr ) : boolean::TRUE );
#else
	return( boolean::TRUE );
#endif
	}

//=============================================================================

ULONG
_symbolList::Eval()
	{
#ifdef DEBUGCOFF
	static ulong mask[ 4 ] =
		{
		0xFF,
		0xFFFF,
		0xFFFFFF,
		0xFFFFFFFF
		};

	static int tblnBytes[ 16 ] =
		{
		0,		// NULL
		0,		// long double
		1,		// character
		2,		// short
		4,		// integer
		4,		// long integer
		0,		// float
		0,		// double
		4,		// structure (address?)
		4,		// union (address?)
		4,		// enumeration (?)
		4,		// moe (?)
		1,		// unsigned character
		2,		// unsigned short
		4,		// unsigned int
		4		// unsigned long int
		};

	ULONG v = addr;

	//PrintToStatWindow( "type = %d", ivType );
	switch ( ivStorageClass )
		{
		case C_ARG:
			v = MemRead( GetReg( REG_A6 )+addr, 4 );
			break;

		case C_AUTO:
			v = MemRead( GetSP()+addr, 4 );
			break;

		case C_REG:
		case C_REGPARM:
			v = GetReg( addr );
			break;

		case C_MOS:
		case C_LABEL:
		case C_MOE:
			break;

		// Don't know if about this section (for sure)
		case C_BLOCK:
			PrintToStatWindow( "C_BLOCK" );
			break;

		case C_FCN:
			PrintToStatWindow( "C_FCN" );
			break;

		// Don't really know if these work correctly
		case C_EXT:
		case C_STAT:
			v = addr;
			break;

		case C_FIELD:
			PrintToStatWindow( "Bit fields not currently supported" );
			v = 0;
			break;

		case C_FILE:
			PrintToStatWindow( "C_FILE" );
			break;

		case C_ALIAS:
			PrintToStatWindow( "C_ALIAS" );
			break;

		case C_HIDDEN:
			PrintToStatWindow( "C_HIDDEN" );
			break;

		case C_STRTAG:
		case C_MOU:
		case C_UNTAG:
		case C_TPDEF:
		case C_ENTAG:
			v = addr;
			break;

		// Used internally by the compiler and assemblers
		case C_EFCN:
		case C_ARRAY:
		case C_SUE:
		case C_SKIP:
			PrintToStatWindow( "Internal storage class %d used", ivStorageClass );

		default:
			v = 0;
			break;
		}

	unsigned int type = ivType & 0x0F;
	unsigned int nBytes = tblnBytes[ type ];

	if ( nBytes == 0 )
		{
		PrintToStatWindow( "Don't know how to deal with type %X", type );
		return( v );
		}
	else
		{
		v &= mask[nBytes-1];

		switch ( type )
			{
			case T_UCHAR:
			case T_USHORT:
			case T_UINT:
			case T_ULONG:
				break;

			case T_NULL:
			case T_LDOUBLE:
			case T_CHAR:
			case T_SHORT:
			case T_INT:
			case T_LONG:
			case T_FLOAT:
			case T_DOUBLE:
			case T_STRUCT:
			case T_UNION:
			case T_ENUM:
			case T_MOE:
				// sign extend
				unsigned int nBits = 32-8*nBytes;
				v <<= nBits, v >>= nBits;
				break;
			}
		}
	return( v );
#else
	return( addr );
#endif
	}


////////////////////////////////////////////////////////////////////////////
//
// Functions which work on the symbol list
//
////////////////////////////////////////////////////////////////////////////

void
SortSymbolAlpha(void)
{
	_symbolList tempBase;
	_list *list,*list2;

	if ( ! symbolListBase.Next() )
		return;

	InitListBase( (_list *)&tempBase);
	tempBase.Address(0);

	while(symbolListBase.Next() != 0)
	 {
		list = RemoveListNode((_list *)symbolListBase.Next());
		list2 = (_list *)&tempBase;
		while(list2->next && strcmp( ((_symbolList *)list)->Name(), ((_symbolList *)list2)->Name()) > 0)
			list2 = list2->next;
		if(list2->Next() && list2->Prev())
			list2 = list2->Prev();
		InsertListNode(list2,list);
	 }
	symbolListBase = tempBase;			// structure to structure copy!
	symbolListBase.Next()->Prev(&symbolListBase);
	tempBase.Next(NULL);				// no nodes are attached here any longer
}

//=============================================================================

void
SortSymbolNum(void)
{
	_symbolList tempBase;
	_list *list,*list2;

	if ( ! symbolListBase.Next() )
		return;

	InitListBase( (_list *)&tempBase );
	tempBase.Address(0);

	while(symbolListBase.Next() != 0)
	 {
		list = RemoveListNode((_list *)symbolListBase.Next());
		list2 = (_list *)&tempBase;
		while(list2->Next() && ((_symbolList *)list)->Address() > (((_symbolList *)list2)->Address()))
			list2 = list2->Next();
		if(list2->Next() && list2->Prev())
			list2 = list2->Prev();
		InsertListNode(list2,list);
	 }
	symbolListBase = tempBase;			// structure to structure copy!
	symbolListBase.Next()->Prev(&symbolListBase);
	tempBase.Next(NULL);				// no nodes are attached here any longer
}

//=============================================================================

errorcode
SymbolNameClear(char *name)
{
	_symbolList *sPtr;
	if ((sPtr = FindSymbol(name)) != 0)
	 {
		delete sPtr;
//		sPtr->SymbolClear();
		return(NOERR);
	 }
	return(ERROR_NOSUCHVAR);
}

//=============================================================================

// 5 Apr 93  0104  WBN  FindSymbol() now only finds symbols in scope
// 6 Apr 93  0026  WBN  If symbol isn't found, FindSymbol() tries _symbol
_symbolList*
FindSymbol(char *text)
	{
	_symbolList* sPtr;

#ifndef SYMBOL_CASE_SENSITIVE
	strupr( text );
#endif

	for ( sPtr = (_symbolList*)symbolListBase.Next(); sPtr; sPtr = (_symbolList*)sPtr->Next() )
		{
		if ( ( strcmp(sPtr->Name(),text) == 0 ) && ( sPtr->InScope() ) )
			break;
		}

#ifdef DEBUGCOFF
	if ( !sPtr )
		{
		char* buffer = (char*)alloca( strlen(text)+1+1 );
		assert( buffer );
		*buffer = '_';
		strcpy( buffer+1, text );

		for ( sPtr = (_symbolList*)symbolListBase.Next(); sPtr; sPtr = (_symbolList*)sPtr->Next() )
			{
			if ( ( strcmp(sPtr->Name(),buffer) == 0 ) && ( sPtr->InScope() ) )
				break;
			}
		}
#endif

#ifdef DEBUGZARDOZ
	if ( !sPtr )
		{
		char* buffer = (char*)alloca( strlen(text)+2+1 );
		assert( buffer );
		*buffer = '~';
		*(buffer+1)  = '~';
		strcpy( buffer+2, text );

		for ( sPtr = (_symbolList*)symbolListBase.Next(); sPtr; sPtr = (_symbolList*)sPtr->Next() )
			{
			if ( ( strcmp(sPtr->Name(),buffer) == 0 ) && ( sPtr->InScope() ) )
				break;
			}
		}
#endif

	return( sPtr );
	}

//=============================================================================

// 5 Apr 93  2230  WBN  FindHexSymbol() now only finds symbols in scope
_symbolList*
FindHexSymbol(ULONG value)
	{
	_symbolList* sPtr;

	for ( sPtr = (_symbolList*)symbolListBase.Next(); sPtr; sPtr = (_symbolList*)sPtr->Next() )
		{
		if ( value == sPtr->Address() && ( sPtr->InScope() ) )
			break;
		}

	return( sPtr );
	}

//=============================================================================

_symbolList*
AddSymbolQuick(ULONG addr, char *text, uword type, byte storageClass )
{
	_symbolList *sPtr;

	if ( sPtr = new _symbolList )
		{
		char* string = DupString(text);
		assert( string );
		if ( string )
			{
#ifndef SYMBOL_CASE_SENSITIVE
			strupr( string );
#endif
			InsertListNode((_list *)&symbolListBase,(_list *)sPtr);
			sPtr->Name( string );
			sPtr->Address( addr );

			sPtr->Type( type );
			sPtr->StorageClass( storageClass );
	 	 }
		else
			{
			delete sPtr;
			sPtr = NULL;
			}
		}

	return( sPtr );
	}

//=============================================================================

FLAG
SymbolGUIClear(_object *oPtr)
{
	_listRectDesc *lrPtr;
	_symbolList *slPtr,*sPtr;
	int selected;

	slPtr = &symbolListBase;
	lrPtr = (_listRectDesc *)oPtr->dataPtr3;
	selected = lrPtr->selItem;

	if(slPtr->Next())
	 {
		sPtr = (_symbolList *)SkipNodes((_list *)slPtr,selected+1);
//		sPtr->SymbolClear();
		delete sPtr;
	 }
	return(boolean::TRUE);
}


////////////////////////////////////////////////////////////////////////////
//
// Functions which work on the symbol list window
//
////////////////////////////////////////////////////////////////////////////

#include "zardoz.hpp"

errorcode
LoadSymbol( FILE* inFile )
	{
	char tempBuff[0x81];
	int	i;
	ULONG addr;
	errorcode error = NOERR;

	{
		// Validate symbol file
#if defined( DEBUGDR )
		for(i=0;i<0x80;i++)
			tempBuff[i] = getc(inFile);
		tempBuff[13] = 0;
		if(strcmp(symbolName,&tempBuff[2]))
#elif defined( DEBUGZARDOZ )
		if ( get_word( inFile ) != 0x1234 )
#endif
			{
			fclose( inFile );
			return( ERROR_NOTSYMBOLFILE );
			}

		PrintMessageBar( "Loading Symbols" );
		i = 0;

#if defined( DEBUGDR )
		tempBuff[i] = getc(inFile);			// read first character
		while(!feof(inFile))
		{
			while(tempBuff[i++])
				tempBuff[i] = getc(inFile);
			addr = getc(inFile);
			addr |= ((ULONG)getc(inFile)) << 8;
			addr |= ((ULONG)getc(inFile)) << 16;
			addr |= ((ULONG)getc(inFile)) << 24;

			if ( !AddSymbolQuick(addr,tempBuff) )
			 {
				fclose(inFile);
				ClearMessageBar();
				return(ERROR_NOMEM);
			 }
			i = 0;
			tempBuff[i] = getc(inFile);			// read first character
		 }
#elif defined( DEBUGZARDOZ )
	long addr, size;
	unsigned short snum, len, line, mnum = 0, curName;
	unsigned short nsecs, nmods, nrecs, nfiles, nsyms, type, ps;
	char buf[ 128 ], *cp;
	char doSyms = 0, doLines = 0;
	unsigned long val;
	short i;
	LinRec lr;
	FILE* fp = inFile;

	nsecs = get_word( fp );
	nmods = get_word( fp );
	while ( nmods-- )
		{
		len = getc( fp );
		fread( buf, 1, len, fp );
		buf[ len ] = 0;
		printf( "\nmodule %d: %s\n", mnum++, buf );
		nsecs = getc( fp );
		while ( nsecs-- )
			{
			snum = getc( fp );
			addr = get_long( fp );
			size = get_long( fp );
//			printf( "\tsnum = %d start=%lx size=%ld\n", snum, addr, size );
			}
		nrecs = get_word( fp );

		fseek( fp, ((long)nrecs) * sizeof( LinRec ), 1 );

		snum = 0;
		nfiles = getc( fp );
		while ( nfiles-- )
			{
			len = getc( fp );
			cp = buf;
			while ( len-- )
				*cp++ = getc( fp );
			*cp = 0;
			line = get_word( fp );
//			printf( "\t%d: nLines=%d <%s>\n", snum, line, buf );
			++snum;
			}
		}

	nsyms = get_word( fp );

	while ( nsyms-- )
		{
		val = get_long( fp );
		type = getc( fp );
		len = getc( fp );
		fread( buf, 1, len, fp );
		buf[ len ] = 0;
//		printf( "%02X %08lX <%s>\n", type, val, buf );
		PrintToStatWindow( "%02X %08lX <%s>\n", type, val, buf );
		if ( !AddSymbolQuick( val, buf ) )
			{
			fclose( inFile );
			ClearMessageBar();
			return( ERROR_NOMEM );
			}
		}

#endif

		fclose( inFile );
		SortSymbols( symbolObjPtr );
		if ( symbolOpen )
			ChangeListRect( (_window*)symbolObjPtr->layer,(_listRectDesc *)symbolObjPtr->dataPtr3,(_stringList *)&symbolListBase );
		ClearMessageBar();
		}

	return( error );
	}

errorcode
LoadSymbol( char* fileName )
	{
	FILE *inFile;
	errorcode error = NOERR;

	inFile = fopen(fileName,"rb");
	if ( inFile )
		{
		error = LoadSymbol( inFile );
		fclose( inFile );
		}
	else
		error = ERROR_FILENOTFOUND;

	return( error );
	}

void
SaveSymbols(void *data,char *path,char *fileName)
{
	FILE *inFile;
}


void
SymbolClearAll( void )
{
	_symbolList *sPtr,*slPtr;
	_object *oPtr;

	sPtr = &symbolListBase;
	oPtr = symbolObjPtr;
	slPtr = &symbolListBase;

	while(slPtr->Next())
	 {
		sPtr = (_symbolList *)slPtr->Next();
		free(sPtr->Name());
		DeleteListNode((_list *)sPtr);
	 }
	if(oPtr)
		ChangeListRect((_window*)oPtr->layer,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)slPtr);
}

//=============================================================================

errorcode
AddSymbol(ULONG addr,char *text)
{
	_symbolList* sPtr;
	errorcode error = NOERR;

	if ( sPtr = FindSymbol(text) )
		delete sPtr;

	if ( AddSymbolQuick(addr,text) && symbolOpen)
		ChangeListRect((_window*)symbolObjPtr->layer,(_listRectDesc *)symbolObjPtr->dataPtr3,(_stringList *)&symbolListBase);
	else
		error = ERROR_NOMEM;

	return( error );
}

//=============================================================================

char *
SymbolPrintListEntry(_stringList *stPtr, unsigned char *attr, unsigned* nLines )
{
	_symbolList *sPtr = (_symbolList *)stPtr;

	// Don't print in list if out of scope
	if ( !(sPtr->InScope()) )
		return( NULL );

	char *string = textBuffer;

	*attr = ATTR_NORMAL;

	string = PrintRaw32Bits( string, sPtr->Address() );
	*string++ = ' ';
	*string++ = ' ';
	string = PrintString( (char*)string, (char*)sPtr->Name() );
#ifdef DEBUGCOFF
	*string++ = ' ';
	*string++ = ' ';
	*string++ = '(';

	char* szStorageClass;

	switch ( sPtr->StorageClass() )
		{
		case C_EFCN:
			szStorageClass = "C_EFCN";
			break;

		case C_NULL:
			szStorageClass = "C_NULL";
			break;

		case C_AUTO:
			szStorageClass = "Automatic";
			break;

		case C_EXT:
			szStorageClass = "External";
			break;

		case C_STAT:
			szStorageClass = "Static";
			break;

		case C_REG:
			szStorageClass = "Register ";
			string = PrintString( (char*)string, szStorageClass );
			szStorageClass = regNameArray[ sPtr->Address() ];
			break;

		case C_EXTDEF:
			szStorageClass = "External Definition";
			break;

		case C_LABEL:
			szStorageClass = "Label";
			break;

		case C_ULABEL:
			szStorageClass = "Undefined Label";
			break;

		case C_MOS:
			szStorageClass = "Member of Structure";
			break;

		case C_ARG:
			szStorageClass = "Function Argument";
			break;

		case C_STRTAG:
			szStorageClass = "Structure Tag";
			break;

		case C_MOU:
			szStorageClass = "Member of Union";
			break;

		case C_UNTAG:
			szStorageClass = "Union Tag";
			break;

		case C_TPDEF:
			szStorageClass = "Type Definition";
			break;

		case C_USTATIC:
			szStorageClass = "Unitialized Static";
			break;

		case C_ENTAG:
			szStorageClass = "Enumeration Tag";
			break;

		case C_MOE:
			szStorageClass = "Member of Enumeration";
			break;

		case C_REGPARM:
			szStorageClass = "Register Parameter";
			break;

		case C_FIELD:
			szStorageClass = "Bit Field";
			break;

		case C_ARRAY:
			szStorageClass = "Array Dimension Information";
			break;

		case C_SUE:
			szStorageClass = "Structure, Union, or Enumeration";
			break;

		case C_SKIP:
			szStorageClass = "C_SKIP: Should not be output";
			break;

		case C_BLOCK:
			szStorageClass = "Beginning and end of Block";
			break;

		case C_FCN:
			szStorageClass = "Beginning and end of Function";
			break;

		case C_EOS:
			szStorageClass = "End of Structure";
			break;

		case C_FILE:
			szStorageClass = "Filename";
			break;

		case C_ALIAS:
			szStorageClass = "Duplicate Tag";
			break;

		case C_HIDDEN:
			szStorageClass = "Like static, used to avoid name conflicts";
			break;

		default:
			szStorageClass = (char*)calloc( 20, 1 );
			assert( szStorageClass );
			sprintf( szStorageClass, "%d", sPtr->StorageClass() );
			break;
		}
	string = PrintString( (char*)string, szStorageClass );

#if 0
	{ // Print the scope
	char buffer[80];

	if ( sPtr->scope )
		{
		sprintf( buffer, " %lX-%lX", sPtr->scope->Start(), sPtr->scope->End() );
		string = PrintString( (char*)string, buffer );
		}
	}
#endif

	*string++ = ')';
#endif

	*string = '\0';

	*nLines = 1;

	return( textBuffer );
}

//=============================================================================

void
SymbolRoutine(_object *oPtr)
{
	assert( oPtr );
	ChangeListRect((_window*)oPtr->layer,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)&symbolListBase);
}

////////////////////////////////////////////////////////////////////////////
//
// Functions which work with the *user* (GUI) and the symbol list window
//
////////////////////////////////////////////////////////////////////////////

void
LoadGUISymbols(void *data,char *path,char *fileName)
{
	errorcode error;
	error = LoadSymbol(fileName);
	if(error)
		PrintError(error);
}

//=============================================================================

void
SymbolSetGUI2(ULONG SymbolAddr,void *dataPtr)
{
	_object *oPtr;
	errorcode error;
	oPtr = (_object *)dataPtr;
	assert( oPtr );

	error = AddSymbol(SymbolAddr,strString);
	if(error)
		PrintError(error);
	else
	 {
		SortSymbols(oPtr);
		ChangeListRect((_window*)oPtr->layer,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)&symbolListBase);
	 }
}

//=============================================================================

void
SymbolSetGUI(char *string,void *dataPtr)
{
	_object *oPtr;
	oPtr = (_object *)dataPtr;

	UnPadString(strString);
	if(!FindSymbol(string))
		GetExpr(SymbolSetGUI2,oPtr,"Enter Symbol Address",oPtr->layer);
}

//=============================================================================

void
SortSymbols(_object *oPtr)
{
	switch(symbolSortBy)
	 {
		case 0:             		// don't sort
			break;
		case 1:
			SortSymbolAlpha();
			assert( oPtr );
			ChangeListRect((_window*)oPtr->layer,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)&symbolListBase);
			break;
		case 2:
			SortSymbolNum();
			assert( oPtr );
			ChangeListRect((_window*)oPtr->layer,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)&symbolListBase);
			break;
	 }
}

//=============================================================================

FLAG
SymbolSetSort(_menuItem *iPtr,_object *oPtr,int choice)
{
	symbolSortBy = choice;
	SortSymbols(oPtr);
	return(boolean::TRUE);
}

//=============================================================================

menuItems symbolSortByMenu[] =
{
	{"Sort &Off         ",SymbolSetSort,0},
	{"Sort By &Name     ",SymbolSetSort,1},
	{"Sort By &Constant ",SymbolSetSort,2},
	{0,0,0}
};

//=============================================================================

FLAG
MenuSymbolSortBy(_menuItem *iPtr,_object *oPtr,int choice)
{
	DoSubMenu(iPtr,oPtr,symbolSortByMenu,"Sort By");
	return(boolean::FALSE);
}

//=============================================================================

menuItems symbolMenu[] =
{
	{"&Load Symbols...   Ctrl-L",SendWindowMessage,SYMBOL_LOAD},
	{"&Save Symbols...         ",SendWindowMessage,SYMBOL_SAVE},
	{"Se&t Symbol...     Ctrl-S",SendWindowMessage,SYMBOL_SET},
	{"&Clear Symbol      Ctrl-C",SendWindowMessage,SYMBOL_CLEAR},
	{"Clear &All Symbols Ctrl-A",SendWindowMessage,SYMBOL_CLEARALL},
	{"Sort &By               >>",MenuSymbolSortBy,0},
	{0,0,0}
};

//=============================================================================

void
SymbolCommand(char **s,_object *oPtr, errorcode *error)
{
	ULONG dummy;
	unsigned int word,wordType;

	*error = NOERR;
	while (*s && **s && !*error)
	 {
		*s = ParseWord(*s, &word);
		switch (word & 0xff)
		 {
			case RW_VALUE:
				symbolSortBy = 2;
				SortSymbols(oPtr);
				break;

			case RW_ALPHA:
				symbolSortBy = 1;
				SortSymbols(oPtr);
				break;

			case RW_OFF:
				symbolSortBy = 0;
				SortSymbols(oPtr);
				break;

			case RW_WLOAD:
			default:
				*error = LoadSymbol(*s);
				*s=NULL;
				//*error = ERROR_SYNTAX;
				break;
		 }
	 }
	return;
}

//=============================================================================

FLAG
AddSymbolGadgets(_window* pWindow, _object *pObject)
{
	_gadget *gPtr;
	_listRectDesc *lrPtr;
	lrPtr = AddListRect(pWindow,pWindow->xSize-2,pWindow->ySize-2,(_stringList *)&symbolListBase,SYGAD_LIST);
	pObject->dataPtr3 = (char*)lrPtr;
	if(lrPtr)
	 {
		lrPtr->drawListEntry = SymbolPrintListEntry;
		return(boolean::TRUE);
	 }
	return(boolean::FALSE);
}

//=============================================================================

FLAG
SymbolInput(_input *in,_object *oPtr)
{
    _window *pWindow;
	_menu *mPtr;
	_listRectDesc *lrPtr;
	_symbolList *sPtr;

	int selected;
    FLAG inputUsed;
	FLAG fileIn;
	_gadget *gPtr;

    pWindow = (_window *)oPtr->layer;
	assert( pWindow );
    inputUsed = boolean::TRUE;

	fileIn = ListRectInput(in,oPtr,(_listRectDesc *)oPtr->dataPtr3,SYGAD_LIST);
	if((fileIn & LRIF_INPUTUSED) == boolean::FALSE)
		switch(in->inputType)
	 	{
			case INP_WINDOW_MESSAGE:
				switch(in->message)
				 {
					case SYMBOL_SETBREAK:
						lrPtr = (_listRectDesc *)oPtr->dataPtr3;
						selected = lrPtr->selItem;
						sPtr = (_symbolList *)SkipNodes((_list *)lrPtr->listPtr,selected+1);
						AddBrkPt(sPtr->Address(),0,NULL);
						break;
					case SYMBOL_SETBREAKONCE:
						lrPtr = (_listRectDesc *)oPtr->dataPtr3;
						selected = lrPtr->selItem;
						sPtr = (_symbolList *)SkipNodes((_list *)lrPtr->listPtr,selected+1);
						AddBrkPt(sPtr->Address(),1,NULL);
						break;
					case SYMBOL_LOAD:
						strcpy(patternString,"*.sym       ");
						DoFileReq("Load Symbols",10,1,LoadGUISymbols,oPtr);
						break;
					case SYMBOL_SAVE:
						strcpy(patternString,"*.sym       ");
						DoFileReq("Save Symbols",10,1,SaveSymbols,oPtr);
						break;
					case SYMBOL_SET:
						GetString(SymbolSetGUI,oPtr,"Enter Symbol Label",oPtr->layer);
						break;
					case SYMBOL_CLEAR:
						SymbolGUIClear(oPtr);
						break;
					case SYMBOL_CLEARALL:
						SymbolClearAll();
						break;
				 }
				break;
			case INP_KEY:
				switch(in->fullKey)
	 	 	 	{
					case CMD_LOCALMENU:
						mPtr = CreateMenuWithItems(symbolMenu,"Symbols");
						mPtr->xPos = AutoPosition(pWindow->xPos,pWindow->xSize,mPtr->xSize,displayWidth);
						mPtr->yPos = AutoPosition(pWindow->yPos,pWindow->ySize,mPtr->ySize,displayHeight);
						DoMenu(mPtr,oPtr);
						break;
					case CMD_CLOSEWINDOW:
						delete pWindow;
						delete (_listRectDesc *)oPtr->dataPtr3;
						delete oPtr;
						oPtr = NULL;
						inputUsed = boolean::TRUE;
						symbolOpen = boolean::FALSE;
						symbolObjPtr = NULL;
						break;
					default:
						inputUsed = boolean::FALSE;
	 	 	 	}
				break;

			case INP_MOUSEMOVE:
				inputUsed = boolean::FALSE;
				break;

			case INP_MOUSE_LEFTBUTTON_UP:
				break;

			case INP_MOUSE_LEFTBUTTON_DOWN:
				if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
			 	{
					switch(gPtr->gNum)
				 	{
						case GGAD_CLOSE:
							delete pWindow;
							delete (_listRectDesc *)oPtr->dataPtr3;
							delete oPtr;
							ActivateFrontWindow();
							inputUsed = boolean::TRUE;
							oPtr = NULL;
							symbolOpen = boolean::FALSE;
							symbolObjPtr = NULL;
							break;
						default:
							inputUsed = boolean::FALSE;
							break;
				 	}
			 	}
				break;

			case INP_MOUSE_RIGHTBUTTON_DOWN:
				mPtr = CreateMenuWithItems(symbolMenu,"Symbols");
				DoLocalMenu(mPtr,oPtr,in);
				break;

			case INP_RESIZE:
				delete (_listRectDesc *)oPtr->dataPtr3;
				if(!AddSymbolGadgets(pWindow,oPtr))
					PrintError(ERROR_NOMEM);
				break;

			case INP_COMMAND:
				SymbolCommand(in->cmdText,oPtr,in->errPtr);
				break;

			default:
				inputUsed = boolean::FALSE;
	 	}

	if(inputUsed && oPtr)
	 {
		UpdateListRect(pWindow,(_listRectDesc *)oPtr->dataPtr3,SYGAD_LIST);
		DrawGadgets(pWindow);
	 }
    return(inputUsed);
}

//=============================================================================

struct _inputKeyRemap symbolKeys[] =
{
	{KEY_CTRLB,SYMBOL_SETBREAK},
	{KEY_CTRLO,SYMBOL_SETBREAKONCE},
	{KEY_CTRLL,SYMBOL_LOAD},
	{KEY_INSERT,SYMBOL_SET},
	{KEY_DELETE,SYMBOL_CLEAR},
	{KEY_CTRLS,SYMBOL_SET},
	{KEY_CTRLC,SYMBOL_CLEAR},
	{KEY_CTRLA,SYMBOL_CLEARALL},
	{0,0}
};

//=============================================================================

FLAG
OpenSymbol(void)
{
	if(!symbolOpen)
	 {
		symbolOpen = boolean::TRUE;
    	_object * oPtr = AddObject(SymbolRoutine);
		assert( oPtr );

		symbolObjPtr =oPtr;
    	_window * pWindow = new _window(54,1,26,BWIN_YSIZE,"Symbol",(unsigned char)symbolAttr,(char)symbolChar);

		pWindow->xMin = 12;
		pWindow->xMax = displayWidth;
		pWindow->yMin = 4;					// leave enough room for up/down gadgets
		pWindow->yMax = displayHeight;

		pWindow->data = (void *)oPtr;
    	oPtr->inputRoutine = SymbolInput;
		oPtr->layer = pWindow;
		oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;
		oPtr->remap.keyArray = symbolKeys;

		AddSysGadgets(pWindow);
		if(!AddSymbolGadgets(pWindow,oPtr))
			PrintError(ERROR_NOMEM);
		ActivateFrontWindow();
		DrawGadgets(pWindow);
		ChangeListRect((_window*)oPtr->layer,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)&symbolListBase);
		return(boolean::TRUE);
	 }
	else
	 {
		symbolObjPtr->layer->ToFront();
		ActivateFrontWindow();
		return(boolean::TRUE);
	 }
}

//=============================================================================
