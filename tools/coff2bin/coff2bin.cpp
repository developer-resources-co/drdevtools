//==============================================================================
// coff2bin.cpp: lame coff code from the coff book, used to understand the format
//==============================================================================

#include <stdlib.h>
#include <alloc.h>
#include <string.h>

#include "coff2bin.hpp"

//==============================================================================

long num_sections;
long section_seek;

long symptr;
long num_symbols;

FILE *fd;								// input
FILE* fpOut;							// output

//==============================================================================

void Exit( int nReturnCode )
	{
	if ( fd ) fclose( fd );
	if ( fpOut ) fclose( fpOut );

	exit( nReturnCode );
	}


unsigned short swaphilo( unsigned short i )
{
	return (((i & 0xff)<<8) + (i >> 8));
}

//-----------------------------------------------------------------------------

long swaphilo( long l )
{
	unsigned short i1, i2;

	i1 = swaphilo( (unsigned short)l );
	i2 = swaphilo( (unsigned short)(l >> 16) );
	return( (long)i1<<16L | i2 );

//	return( swaphilo( (unsigned short)l, (unsigned short)(l>>16) ) );
}

//=============================================================================

void read_headers( char* szProgName )
{
	FILHDR file_header;
	AOUTHDR optional_header;

	fread( &file_header, sizeof(FILHDR), 1, fd );
	file_header.f_magic = swaphilo( file_header.f_magic );
	file_header.f_nscns = swaphilo( file_header.f_nscns );
	file_header.f_timdat = swaphilo( file_header.f_timdat );
	file_header.f_symptr = swaphilo( file_header.f_symptr );
	file_header.f_nsyms = swaphilo( file_header.f_nsyms );
	file_header.f_opthdr = swaphilo( file_header.f_opthdr );
	file_header.f_flags = swaphilo( file_header.f_flags );

	printf( "%s: %d sections  %s",
		szProgName,
		file_header.f_nscns,
		ctime( &file_header.f_timdat ) );
#if 0
	printf( "FILE HEADER VALUES\n" );
	printf( "f_magic  = 0%o (0x%x)\n", file_header.f_magic, file_header.f_magic );
	printf( "f_nscns  = %d\n", file_header.f_nscns );
	printf( "f_timdat = %s", ctime( &file_header.f_timdat ) );
	printf( "f_symptr = %ld\n", file_header.f_symptr );
	printf( "f_nsyms  = %ld\n", file_header.f_nsyms );
	printf( "f_opthdr = %d\n", file_header.f_opthdr );
	printf( "f_flags  = 0%o\n", file_header.f_flags );
#endif

	// Save the global values
	num_sections = file_header.f_nscns;
	num_symbols = file_header.f_nsyms;
	symptr = file_header.f_symptr;

	if ( file_header.f_opthdr )
		{
		fread( &optional_header, sizeof(AOUTHDR), 1, fd );
		}

	section_seek = FILHSZ + file_header.f_opthdr;
}

//==============================================================================

void read_sections( void )
	{
	SCNHDR sh;
	SYMENT se,se2;
	AUXENT ae,ae2;
	RELOC re;
	LINENO le;
	int raw_data;
	int i,j;
	unsigned long pos;
	long bfIndex;

	for ( i=0; i<num_sections; ++i )
		{
		fseek( fd, section_seek, 0 );
		fread( &sh, sizeof( SCNHDR ), 1, fd );
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

		printf( "[%2d]  %-8s  addr=0x%08lx  size=0x%lx",
			i, sh.s_name, sh.s_paddr, sh.s_size );

#if 0
		printf( "\n %s - SECTION HEADER - \n", sh.s_name );
		printf( "s_paddr   = 0x%x\n", sh.s_paddr );
		printf( "s_vaddr   = 0x%x\n", sh.s_vaddr );
		printf( "s_size    = 0x%x\n", sh.s_size );
		printf( "flags     = 0x%x\n", sh.s_flags);
#endif

		// Output raw data only for text and data sections
		if ( strcmp( sh.s_name, ".bss" ) )
			{
			fseek( fd, sh.s_scnptr, 0 );
			fseek( fpOut, sh.s_paddr, 0 );

			for ( j=0; j<sh.s_size && ((raw_data = fgetc(fd)) != EOF); ++j )
				{
				if ( fputc( raw_data, fpOut ) == EOF )
					{
					fprintf( stderr, "\nError writing data (out of disk space?)\n" );
					Exit( 10 );
					}
				}
			}
		printf( "\n" );
		}
	}

//==============================================================================

void
main( int argc, char *argv[] )
	{
	if ( argc != 3 )
		{
		printf(
				"coff2bin v1.0.3  (c) 1993 Alexandria, Inc.  All Rights Reserved.\n"
				"Convert COFF file to binary data file\n"
				"By William B. Norris IV\n"
				"\n"
				"Usage: coff2bin <inFile> <outFile>\n"
			);
		exit(0);
	 	}

	if ( ( fd = fopen( argv[1], "rb" ) ) && ( fpOut = fopen( argv[2], "wb" ) ) )
		{
		read_headers( argv[1] );
		read_sections();
		}

	Exit( 0 );
	}

//==============================================================================
