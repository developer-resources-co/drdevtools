//==============================================================================
// coff.cpp:
//==============================================================================

#ifdef DEBUGCOFF

//==============================================================================

#include <stdlib.h>
//#include <alloc.h>
#include <string.h>
#include <assert.h>

#include <iostream.h>

#include	"moninc.hpp"
#include "coff.hpp"

///////////////////////////////////////////////////////////////////////////////

coff* coffBase = NULL;

///////////////////////////////////////////////////////////////////////////////

// THERE MUST BE BETTER VERSIONS OF THESE FUNCTIONS SOMEWHERE!!

unsigned short swaphilo( unsigned short i )
	{
	return (((i & 0xff)<<8) + (i >> 8));
	}

long swaphilo( long l )
	{
	unsigned short i1, i2;

	i1 = swaphilo( (unsigned short)l );
	i2 = swaphilo( (unsigned short)(l >> 16) );
	return( (long)i1<<16L | i2 );

//	return( swaphilo( (unsigned short)l, (unsigned short)(l>>16) ) );
	}

///////////////////////////////////////////////////////////////////////////////

coff::coff( FILE* fd , istream *newCoffStream) : sectionList()
	{
	assert( fd );
	coff_fd = fd;
	assert( newCoffStream );
	coffStream = newCoffStream;

	cbBinary = 0UL;
	read_headers();

	str_tab = NULL;

	if ( file_header.f_magic == COFF_SIERRA_MAGIC_HEADER )
		{
		read_sections();
		read_strings();
		read_symbols();
		read_binary();
		if ( str_tab ) farfree( str_tab ), str_tab = NULL;
		}
	else
		{ // I don't understand this COFF variation
			PrintMessageBar( "Not a Sierra COFF file" );
		}
	}

///////////////////////////////////////////////////////////////////////////////

coff::~coff( void )
	{
	if ( str_tab ) farfree( str_tab ), str_tab = NULL;
	delete coffStream;
	}

///////////////////////////////////////////////////////////////////////////////

void coff::read_headers( void )
	{
	fread( &file_header, sizeof( FILHDR ), 1, coff_fd );
	file_header.f_magic = swaphilo( file_header.f_magic );
	file_header.f_nscns = swaphilo( file_header.f_nscns );
	file_header.f_timdat = swaphilo( file_header.f_timdat );
	file_header.f_symptr = swaphilo( file_header.f_symptr );
	file_header.f_nsyms = swaphilo( file_header.f_nsyms );
	file_header.f_opthdr = swaphilo( file_header.f_opthdr );
	file_header.f_flags = swaphilo( file_header.f_flags );

#ifdef PRINT
	printf( "FILE HEADER VALUES\n" );
	printf( "f_magic  = 0%o (0x%x)\n", c.file_header.f_magic, c.file_header.f_magic );
	printf( "f_nscns  = %d\n", c.file_header.f_nscns );
	printf( "f_timdat = %s", ctime( &c.file_header.f_timdat ) );
	printf( "f_symptr = %ld\n", c.file_header.f_symptr );
	printf( "f_nsyms  = %ld\n", c.file_header.f_nsyms );
	printf( "f_opthdr = %d\n", c.file_header.f_opthdr );
	printf( "f_flags  = 0%o\n", c.file_header.f_flags );
#endif

	// Save the global values
	nSections = file_header.f_nscns;
	num_symbols = file_header.f_nsyms;
	symptr = file_header.f_symptr;

	strptr = symptr + sizeof( SYMENT ) * num_symbols;

	if ( file_header.f_opthdr )
		{
		fread( &optional_header, sizeof( AOUTHDR ), 1, coff_fd );

		}

	section_seek = FILHSZ + file_header.f_opthdr;
	}

///////////////////////////////////////////////////////////////////////////////

void coff::read_sections( void )
	{
	SCNHDR sh;
	RELOC re;
	LINENO le;
	char *raw_data;
	int i,j;
	coffSection *tcs,*tcsTemp;

	for ( i=0; i<nSections; ++i )
	 {
		fseek( coff_fd, section_seek, 0 );
		fread( &sh, sizeof( SCNHDR ), 1, coff_fd );
		section_seek += sizeof( SCNHDR );
		// Repair struct for 80x86 order
		sh.s_paddr = swaphilo( sh.s_paddr );
		sh.s_vaddr = swaphilo( sh.s_vaddr );
		sh.s_size = swaphilo( sh.s_size );
		sh.s_scnptr = swaphilo( sh.s_scnptr );
		sh.s_relptr = swaphilo( sh.s_relptr );
		sh.s_lnnoptr = swaphilo( sh.s_lnnoptr );
		sh.s_nreloc = swaphilo( sh.s_nreloc );
		sh.s_nlnno = swaphilo( sh.s_nlnno );
		sh.s_flags = swaphilo( sh.s_flags );

		// Add section to section header list

		tcs = new coffSection(&sh,coffStream);
		sectionList.Insert(tcs);
#if 0
		printf( "\n %s - SECTION HEADER - \n", sh.s_name );
		printf( "s_paddr   = 0x%x\n", sh.s_paddr );
		printf( "s_vaddr   = 0x%x\n", sh.s_vaddr );

		// Output raw data only for text and data sections
		if ( strcmp( sh.s_name, ".bss" ) )
			{
			assert( sh.s_size <= 65536 );
			raw_data = (char *)malloc( sh.s_size );
			fseek( coff_fd, sh.s_scnptr, 0 );
			fread( raw_data, sh.s_size, 1, coff_fd );

			printf( "RAW DATA\n" );
			j = 0;
			while ( j < sh.s_size )
				{
				printf( "%1x", (*raw_data>>4) & 0xF );
				printf( "%1x ", (*raw_data & 0xF ) );
				*raw_data++; j++;
				if ( j%16 == 0 ) printf( "\n" );
				}
			printf( "\n" );
			free( raw_data-j );
			}

		if ( sh.s_nreloc )
			{
			printf( "\n RELOCATION ENTRIES\n" );
			fseek( coff_fd, sh.s_relptr, 0 );
			j = 0;
			while ( j<sh.s_nreloc )
				{
				fread( &re, RELSZ, 1, coff_fd );
				re.r_vaddr = swaphilo( re.r_vaddr );
				re.r_symndx = swaphilo( re.r_symndx );
				re.r_type = swaphilo( re.r_type );

				printf( "r_vaddr = 0x%x", re.r_vaddr );
				printf( "r_symndx = %d\n", re.r_symndx );
				j++;
				}
			}

		if ( sh.s_nlnno )
			{
			printf( "\n LINE NUMBER ENTRIES \n" );
			fseek( coff_fd, sh.s_lnnoptr, 0 );

			for ( j=0; j<sh.s_nlnno; ++j )
				{
				fread( &le, LINESZ, 1, coff_fd );
				le.l_addr.l_symndx = swaphilo( le.l_addr.l_symndx );
				le.l_lnno = swaphilo( le.l_lnno );

				if ( le.l_lnno == 0 )
					printf( "function address 0x%x\n", le.l_addr.l_symndx );
				else
					printf( "line# %d at address 0x%x\n", le.l_lnno, le.l_addr.l_paddr );
				}
			}
#endif
		}
	}

///////////////////////////////////////////////////////////////////////////////

void coff::read_strings( void )
	{
	unsigned long strings;

	strings = symptr + sizeof( SYMENT ) * num_symbols;
	fseek( coff_fd, strings, 0 );
	fread( &str_length, 4, 1, coff_fd );
	str_length = swaphilo( str_length );

	if ( str_length )
		{
		str_length -= 4;
#ifndef DOSX286
		assert( str_length <= 65536 );
#endif
		str_tab = (char*)farmalloc( str_length );
		fseek( coff_fd, (strings+4), 0 );
		fread( str_tab, str_length, 1, coff_fd );

#ifdef PRINT
		char* str_ptr = str_tab;
		printf( "\n STRING TABLE DUMP\n" );
		do
			{
			printf( "%s\n", str_ptr );
			while ( *str_ptr++ != '\0' )
				;
			}
		while ( str_ptr < (str_tab+str_length) );
#endif
		}
	}

///////////////////////////////////////////////////////////////////////////////

void
coff::read_binary( void )
	{
	coffSection* csPtr;

	cbBinary = 0UL;

	for ( csPtr = GetSectionPtr(); csPtr; csPtr=csPtr->GetNext() )
		if ( csPtr->Flags() & (SH_TEXT|SH_DATA) )
			cbBinary  += csPtr->Size();
	PrintMessageBar( "Loading Binary (%lu bytes)", cbBinary );

	for ( csPtr = GetSectionPtr(); csPtr; csPtr=csPtr->GetNext() )
		{
		if ( csPtr->Flags() & (SH_TEXT|SH_DATA) )
			{
			fseek( coff_fd, csPtr->DataOffset(), 0 );
			LoadFileToSlave( coff_fd, csPtr->VAddr(), csPtr->Size() );
			}
		}

	ClearMessageBar();
	}

///////////////////////////////////////////////////////////////////////////////

// not used
void coff::LoadSymbols()
{
	char stringBuffer[100];
	PrintMessageBar( "Loading Symbols" );
	streampos pos,symPos;
	SYMENT se;
	AUXENT ae;
	char* symbolName;

	pos = coffStream->tellg();

	coffStream->seekg(coffBase->GetSymbolBase(),ios::beg);

	for ( int i=num_symbols; i; --i )
		{
		coffStream->read((char *)&se, sizeof(SYMENT));

		// Adjust for 80x86
		if ( se.n_zeroes == 0L )
			se.n_offset = swaphilo( se.n_offset );
		se.n_value = swaphilo( se.n_value );
		se.n_scnum = swaphilo( (unsigned short)se.n_scnum );
		se.n_type = swaphilo( se.n_type );

		if ( se.n_zeroes == 0L )	  // name longer than 8 characters --- use name from symbol table
		 {
			symPos = coffStream->tellg();
			coffStream->seekg(coffBase->GetStringBase()+se.n_offset-4,ios::beg);
			coffStream->get(stringBuffer,100,0);				// read symbol
			symbolName = stringBuffer;
			coffStream->seekg(symPos,ios::beg);
		 }
		else
		 {
			// !!! memory problem !!!
			strncpy( stringBuffer, &(se.n_name[0]), 8 );
			stringBuffer[8] = '\0';
			symbolName = stringBuffer;
		 }

		// Extended entries?
		for ( int j=0; j<se.n_numaux; ++j )
		 {
			AUXENT ae;

			coffStream->read((char *)&ae, sizeof(SYMENT));
			--i;
		 }

		switch ( se.n_type )
		 {
			case C_AUTO:
			case C_STAT:
			case C_LABEL:
			case C_REG:
			case C_FCN:

			case C_NULL:
			case C_EXTDEF:
			case C_ULABEL:
			case C_MOS:
			case C_ARG:
			case C_STRTAG:
			case C_MOU:
			case C_UNTAG:
			case C_TPDEF:
			case C_USTATIC:
			case C_ENTAG:
			case C_MOE:
			case C_REGPARM:
			case C_FIELD:
			case C_BLOCK:
			case C_EOS:
			case C_FILE:
			case C_ALIAS:
			case C_HIDDEN:

				AddSymbolQuick( (ULONG)se.n_value, symbolName, se.n_type, se.n_sclass );
				break;

			// Used internally by the compiler and assemblers
			case C_EFCN:
			case C_ARRAY:
			case C_SUE:
			case C_SKIP:

			default:
				break;
		 }
	 }

	coffStream->seekg(pos,ios::beg);
	ClearMessageBar();
}

//==============================================================================

void
	coff::symbols_stack( long* i, Scope* scope )
	{
	for ( ; *i; --(*i) )
		{
		SYMENT se;
		char shortSymbolName[ 9 ];
		char* symbolName;
		_symbolList* sPtr;

		// Read stored 680x0-style symbol
		fread( &se, sizeof( SYMENT ), 1, coff_fd );

		// Adjust for 80x86
		if ( se.n_zeroes == 0L )
			se.n_offset = swaphilo( se.n_offset );
		se.n_value = swaphilo( se.n_value );
		se.n_scnum = swaphilo( (unsigned short)se.n_scnum );
		se.n_type = swaphilo( se.n_type );

		if ( se.n_zeroes == 0L )
			{ // name longer than 8 characters --- use name from symbol table
			symbolName = &( str_tab[ se.n_offset-4 ] );
			}
		else
			{
			strncpy( shortSymbolName, &(se.n_name[0]), 8 );
			*( shortSymbolName+8 ) = '\0';
			symbolName = shortSymbolName;
			}

		// Extended entries?
		for ( int j=0; j<se.n_numaux; ++j, --(*i) )
			{
			AUXENT ae;

			fread( &ae, sizeof( AUXENT ), 1, coff_fd );
			}

		switch ( se.n_sclass )
			{
			case C_EXT:
				if ( se.n_scnum == N_UNDEF )
					break;

			case C_STAT:
				if ( *symbolName == '.' )
					break;

			case C_MOS:
			case C_MOE:
			case C_MOU:

			case C_STRTAG:
			case C_UNTAG:

//			case C_EOS:

			case C_AUTO:
			case C_LABEL:
			case C_REG:
			case C_EXTDEF:
			case C_ULABEL:
			case C_ARG:
			case C_USTATIC:
			case C_ENTAG:
			case C_REGPARM:
			case C_FIELD:
			case C_NULL:
				sPtr = AddSymbolQuick( (ULONG)se.n_value, symbolName, se.n_type, se.n_sclass );
				assert( sPtr );
				sPtr->scope = scope;
				break;

			case C_FCN:
			case C_BLOCK:
				if ( *( symbolName+1 ) == 'b' )
					{ // begin block/function
					symbols_stack( i, new Scope( se.n_value ) );
					}
				else if ( *( symbolName+1 ) == 'e' )
					{ // end block/function
					scope->End( se.n_value );
					--(*i);
					return;
					}
				break;

			case C_FILE:
			case C_TPDEF:


			case C_ALIAS:
			case C_HIDDEN:
			// Used internally by the compiler and assemblers
			case C_EFCN:
			case C_ARRAY:
			case C_SUE:
			case C_SKIP:

			default:
				break;
			}
		}
	}

//==============================================================================

void coff::read_symbols( void )
	{
	PrintMessageBar( "Loading Symbols" );

	fseek( coff_fd, symptr, SEEK_SET );

	Scope* scope = new Scope();				// create global scope
	long i = num_symbols;
	symbols_stack( &i, scope );

	SortSymbols( symbolObjPtr );
	if ( symbolOpen )
		ChangeListRect( (_window*)symbolObjPtr->layer,
			(_listRectDesc *)symbolObjPtr->dataPtr3,
			(_stringList *)&symbolListBase );
	ClearMessageBar();
	}

//==============================================================================

boolean
coffLineNo::FindSource(ULONG pc)
{
	boolean found = boolean::FALSE;
	unsigned short count = entries;
	streampos pos;

	ULONG addr;
	UWORD sLine,oldSLine = 0;

	if(count)
	 {
		coffFile->seekg(dataOffset,ios::beg);

		while(!found && count--)
	 	{
			addr = ReadLong(*coffFile);
			sLine = ReadWord(*coffFile);

			if(sLine == 0)					// handle function symndx
		 	 {
				pos = coffFile->tellg();
				coffFile->seekg(addr*sizeof(SYMENT)+coffBase->GetSymbolBase(),ios::beg);

				// kts very non-portable
				funcSymNdx = addr;
				coffSymbol *funcSym = new coffSymbol(*coffFile,coffBase->GetStringTable());				// create symbol for function
				coffFile->seekg(sizeof(AUXENT)+sizeof(SYMENT)+4,ios::cur);
				functionSourceLine = ReadWord(*coffFile);

				// put it back
				coffFile->seekg(pos,ios::beg);

				addr = funcSym->Addr();
				delete funcSym;
				currentSourceLine = functionSourceLine;
		 	 }
			else							// handle line number
		 	 {
				if(addr == pc || (addr > pc))
			 	 {
					found = boolean::TRUE;
					if(addr != pc)
					 {
						sLine = oldSLine;
						exactMatch = boolean::FALSE;
					 }
					else
					 {
						exactMatch = boolean::TRUE;
						// read next entry to make sure we are showing the last matching entry
						addr = ReadLong(*coffFile);
						if(addr == pc)						// if still matches pc, lets go with this line # instead
							sLine = ReadWord(*coffFile);
					 }
			 	 }
				currentSourceLine = functionSourceLine + (sLine-1);
		 	 }

			if(addr == pc)
				found = boolean::TRUE;
			oldSLine = sLine;
	 	}
	 }

	// ok, we have the line # in the file, now lets find out which file it is

	if(found)
	 {
		int lineFound,symNdx;
		coffAuxSymbol *auxEntPtr[50];
		pos = coffFile->tellg();
		coffFile->seekg(coffBase->GetSymbolBase(),ios::beg);

		coffSymbol *sym;
		coffAuxSymbol currentName;
		strcpy(currentName.Data(),"fuck.c");
		for(lineFound = 0,symNdx = 0; lineFound == 0 && symNdx < funcSymNdx;)
		 {
			sym = new coffSymbol(*coffFile,coffBase->GetStringTable());
			symNdx++;
			for(int i=0;i<sym->NumAuxEntries();i++)
			 {
				auxEntPtr[i] = new coffAuxSymbol(*coffFile, coffBase->GetStringTable());
				symNdx++;
			 }
			if(sym->SClass() == C_FILE)
				currentName = *auxEntPtr[0];

			// clean up
			for(int j=0;i<sym->NumAuxEntries();j++)
				delete auxEntPtr[j];
			delete sym;
		 }
		// put it back
		coffFile->seekg(pos,ios::beg);
		strncpy(&fileName[0],currentName.Data(),AUXESZ);
	 }
	return(found);
}

//==============================================================================

boolean
coffLineNo::FindAddress(char *fileName, UWORD line)
{
	boolean found = boolean::FALSE;
	unsigned short count = entries;
	streampos pos;

	ULONG addr;
	UWORD sLine;

	int lineFound,symNdx;
	coffAuxSymbol *auxEntPtr[50];
	pos = coffFile->tellg();
	coffFile->seekg(coffBase->GetSymbolBase(),ios::beg);

	found = boolean::TRUE;
#if 0
	// find filename
	coffSymbol *sym;
	coffAuxSymbol currentName;
	strcpy(currentName.Data(),"fuck.c");
	for(lineFound = 0,symNdx = 0; lineFound == 0 && symNdx < funcSymNdx;)
	 {
		sym = new coffSymbol(*coffFile,coffBase->GetStringTable());
		symNdx++;
		for(int i=0;i<sym->NumAuxEntries();i++)
			{
			auxEntPtr[i] = new coffAuxSymbol(*coffFile, coffBase->GetStringTable());
			symNdx++;
			}
		if(sym->SClass() == C_FILE)
			currentName = *auxEntPtr[0];

		// clean up
		for(int j=0;i<sym->NumAuxEntries();j++)
			delete auxEntPtr[j];
		delete sym;
	 }
	// put it back
	coffFile->seekg(pos,ios::beg);
	strncpy(&fileName[0],currentName.Data(),AUXESZ);
#endif

	// find line
	if(found && count)
	 {
		coffFile->seekg(dataOffset,ios::beg);

		while(!found && count--)
	 	{
			addr = ReadLong(*coffFile);
			sLine = ReadWord(*coffFile);

			if(sLine == 0)					// handle function symndx
		 	 {
				pos = coffFile->tellg();
				coffFile->seekg(addr*sizeof(SYMENT)+coffBase->GetSymbolBase(),ios::beg);

				// kts very non-portable
				funcSymNdx = addr;
				coffSymbol *funcSym = new coffSymbol(*coffFile,coffBase->GetStringTable());				// create symbol for function
				coffFile->seekg(sizeof(AUXENT)+sizeof(SYMENT)+4,ios::cur);
				functionSourceLine = ReadWord(*coffFile);

				// put it back
				coffFile->seekg(pos,ios::beg);

				addr = funcSym->Addr();
				delete funcSym;
				currentSourceLine = functionSourceLine;
		 	 }
			else							// handle line number
		 	 {
				currentSourceLine = functionSourceLine + (sLine-1);
				if(line == currentSourceLine)
				 {
					found = boolean::TRUE;
					currentPC = addr;
				 }
		 	 }
	 	}
	 }

	return(found);
}

//==============================================================================

#endif

//==============================================================================
