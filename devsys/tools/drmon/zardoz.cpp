//==============================================================================
// zardoz.cpp:
//==============================================================================

#ifdef DEBUGZARDOZ

//==============================================================================

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream.h>

#include	"moninc.hpp"
#include "zardoz.hpp"

///////////////////////////////////////////////////////////////////////////////

zardoz* zardozBase = NULL;

///////////////////////////////////////////////////////////////////////////////

zardoz::zardoz( FILE* fd )
	{
	assert( fd );
	zardoz_fd = fd;

//	str_tab = NULL;

	if ( get_word( fd ) == 0x1234 )
		{
		//read_sections();
		//read_symbols();
		}
	else
		{
		PrintMessageBar( "Not a valid Zardoz symbolic debug file" );
		}
	}


zardoz::~zardoz()
	{
	}


//zardoz::read_headers() 

//zardoz::read_sections()

void
zardoz::read_symbols()
	{
	// save and restore file position if necessary
	LoadSymbol( zardoz_fd );
	}

#if 0
	FILE* fp;
	long addr, size;
	unsigned short snum, len, line, mnum = 0, curName;
	unsigned short nsecs, nmods, nrecs, nfiles, nsyms, type, ps;
	char buf[ 128 ], *cp;
	char doSyms = 0, doLines = 0;
	unsigned long val;
	short i;
	LinRec lr;

	while ( argc > 2 )	
		{
		if ( strcmp( argv[1], "-l" ) == 0 )
			doLines = 1;
		else if ( strcmp( argv[1], "-s" ) == 0 )
			doSyms = 1;
		else
			return 1;
		--argc;
		++argv;
		}

	if ( argc != 2 )
		return 1;

	if ( ( fp = fopen( argv[1], "rb" ) ) == NULL )
		return 1;

	if ( get_word( fp ) != 0x1234 )
		{
		printf( " bad symbol file!\n" );
		return 1;
		}

	nsecs = get_word( fp );
	nmods = get_word( fp );
	printf( "%d sections - %d modules\n", nsecs, nmods );
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
			printf( "\tsnum = %d start=%lx size=%ld\n", snum, addr, size );
			}
		nrecs = get_word( fp );
		printf( "\t\tnlinrecs - %u", nrecs );
		if ( doLines )
			{
			while ( nrecs-- )
				{
				fread( &lr, 1, sizeof( LinRec ), fp );
				printf( "\t\tfile %d line %d @ %lx ps-%02X len-%d\n",
					lr.filNum, lr.linNum, lr.addr, lr,ps, lr.nBytes );
				}
			}
		else
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
			printf( "\t%d: nLines=%d <%s>\n", snum, line, buf );
			++snum;
			}
		}

	nsyms = get_word( fp );
	if ( doSyms )
		{
		while ( nsyms-- )
			{
			val = get_long( fp );
			type = getc( fp );
			len = getc( fp );
			fread( buf, 1, len, fp );
			buf[ len ] = 0;
			printf( "%02X %08lX <%s>\n", type, val, buf );
			}
		}

	fclose( fp );
#endif

#endif

//==============================================================================
