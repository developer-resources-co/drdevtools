/////////////////////////////////////////////////////////////////////////////
//
// File:	romopt.cpp
//
// Project: ROM Layout Organizer/Optimizer
//
//	This program takes a list of files and squeezes them as best it can into
// x segments of size y.
//
// Current restrictions:
// ---------------------
// 	No error checking
//		Included file can't be larger than the defined segment size
//		Limited to MAXFILES (200) files total
//		Limited to MAX_EXTENSIONS (50) extensions total
//		Redefinitions of extensions aren't dealt with
//  *	No error messages (esp. if file doesn't exist)
//
// 	09 Aug 92	WBNIV	Created
//  15 Oct 93	WBNIV	Added COFF support
//
/////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dir.h>
#include <ctype.h>
#include <time.h>

#include <pclib/boolean.hpp>
#include <pclib/libtypes.hpp>
#include <pclib/regexp.hpp>
#include "version.h"

#include "coff.hpp"

boolean bCoff = boolean::FALSE;
char szCommentChar[ 80 ];


const long nDefSegmentSize = 1024L*64;

/////////////////////////////////////////////////////////////////////////////

void
	UnPadString(unsigned char *buffer)
	{
	while (*buffer)
		buffer++;

	buffer--;
	while ( (*buffer == ' ') || (*buffer == '\t') )
		buffer--;

	buffer++;
	*buffer = '\0';
	}

/////////////////////////////////////////////////////////////////////////////

const int MAX_FILES = 1000;

void IncludeProc( char* szLine )
	{
	printf( szLine );
	}

/////////////////////////////////////////////////////////////////////////////

const int MAX_EXTENSIONS = 50;

typedef struct extensionEntry
	{
	char *szFilespec;
	char *szMacroName;
	} EXTENSIONENTRY;

EXTENSIONENTRY extensionList[ MAX_EXTENSIONS ];

int numExtensions;


void
	ExtensionsInit( void )
	{
	numExtensions = 0;
	}


void
	ExtensionsProc( char *szLine )
	{
	char szFilespec[132], szMacroName[132];

	if ( numExtensions < MAX_EXTENSIONS )
		{
		*szFilespec = *szMacroName = '\0';
		sscanf( szLine, "%s %s", szFilespec, szMacroName );

		if ( *szFilespec && *szMacroName )
			{ // Valid inputs, create a new entry
			extensionList[numExtensions].szFilespec = strdup( szFilespec );
			extensionList[numExtensions].szMacroName = strdup( szMacroName );
			++numExtensions;
			}
		}
	else
		cout << "Error: too many associations defined.\n";
	}


void
	ExtensionsExit( void )
	{
#ifdef DEBUG
	for ( int i=0; i<numExtensions; ++i )
		{
		cout << extensionList[i].szFilespec << "  " <<
				  extensionList[i].szMacroName << "\n";
		}
#endif
	}

char* ExtensionMatch( char* szFilename )
	{
	regexp *r = NULL;

	// For each filespec in the list, check for match
	for ( int i=0; i<numExtensions; ++i )
		{
		if ( r ) free( r );
		r = regcomp( extensionList[i].szFilespec );
		if ( regexec( r, szFilename ) )
			break;
		}
	if ( r ) free( r );
	return( i < numExtensions ? extensionList[i].szMacroName : "(Undefined)" );
	}

/////////////////////////////////////////////////////////////////////////////

int nTabSize = 4;

void
	TabSizeProc( char* szLine )
	{
	int nTempTabSize;

	UnPadString( szLine );
	if ( (*szLine!='\n') && (sscanf( szLine, "%d", &nTempTabSize ) ) )
		{
		nTabSize = nTempTabSize;
		}
	}

/////////////////////////////////////////////////////////////////////////////

long nSegmentSize = nDefSegmentSize;

void
	SegmentSizeProc( char* szLine )
	{
	long nTempSegmentSize;

	UnPadString( szLine );
	if ( (*szLine!='\n') && (sscanf( szLine, "%ld", &nTempSegmentSize ) ) )
		{
		nSegmentSize = nTempSegmentSize;
		}
	}

/////////////////////////////////////////////////////////////////////////////

typedef struct fileEntry
	{
	char *szFilename;
	long nSize;
	char *szMacroOverride;
	} FILEENTRY;


FILEENTRY fileList[ MAX_FILES ];

int nFiles;

void
	FilenamesInit( )
	{
	nFiles = 0;
	}


FILHDR* read_headers( FILE* );
long read_sections( FILE* );

long ffilesize( char* szFilename )
	{
	FILE* fpTemp;
	long size = -1;

	if ( fpTemp = fopen( szFilename, "rb" ) )
		{
		if ( bCoff )
			{
			FILHDR* file_header = read_headers( fpTemp );
			if ( file_header->f_magic != 0x150 )
				cerr << "Not Sierra COFF file" << endl;
			else
				size = read_sections( fpTemp );
			}
		else
			{
			fseek( fpTemp, 0L, SEEK_END );
			size = ftell( fpTemp );
			}
		fclose( fpTemp );
		}
	return size;
	}


void
	FilenamesProc( char *szLine )
	{
	FILE* fpTemp;
	char* sz;

	// Strip LF
	if ( sz = strrchr( szLine, '\n' ) )
		*sz = '\0';

	if ( *szLine )
		{
		char szFilename[132], szMacroName[132];

		*szFilename = *szMacroName = '\0';

		sscanf( szLine, "%s %s", szFilename, szMacroName );

		if ( nFiles >= MAX_FILES )
			{
			printf( "ERROR: Out of filename spaces\n" );
			exit( 10 );
			}
		// Copy filename
		fileList[ nFiles ].szFilename = strdup( szFilename );
		strlwr( fileList[ nFiles ].szFilename );

		if ( *szMacroName )
			{ // Copy macro override
			fileList[ nFiles ].szMacroOverride = strdup( szMacroName );
			}

		// Calculate size of file
		fileList[ nFiles ].nSize = ffilesize( fileList[ nFiles ].szFilename );
		if ( fileList[ nFiles ].nSize == 0 )
			{ // Print warning
			cerr << "Warning: \"" <<
				fileList[ nFiles ].szFilename << "\" empty" << endl;
			}
		else if ( fileList[ nFiles ].nSize < 0 )
			{ // Print error -- file not found
			cerr << "Warning: unable to open input file \"" <<
				fileList[ nFiles ].szFilename << "\"" << endl;
			fileList[ nFiles ].nSize = 0;
			}

		++nFiles;		// Increment here so continue's don't affect count
		}
	}


int
	sort_fn( const void *fe1, const void *fe2 )
	{
	//return( ((FILEENTRY *)fe2)->nSize - ((FILEENTRY *)fe1)->nSize );
	if ( ((FILEENTRY *)fe2)->nSize == ((FILEENTRY *)fe1)->nSize )
		return 0;
	else if ( ((FILEENTRY *)fe2)->nSize > ((FILEENTRY *)fe1)->nSize )
		return 1;
	else
		return -1;
	}


output_file( int i, long& nSizeAvail )
	{
	char szDrive[MAXDRIVE], szDir[MAXPATH],
		szFile[MAXFILE], szExt[MAXEXT];
	char szPath[MAXPATH];
	int len;

	nSizeAvail -= fileList[i].nSize;

	fnsplit( fileList[i].szFilename, szDrive, szDir, szFile, szExt );

	fnmerge( szPath, szDrive, szDir, NULL, NULL );	// Directory

	if ( bCoff )
		printf( " %s", fileList[i].szFilename );
	else
		{
		len = printf( "\t%s\t%s,%s,%s",
			fileList[i].szMacroOverride ?
				fileList[i].szMacroOverride :
				ExtensionMatch( fileList[i].szFilename ),
			szFile, szExt+1, szPath
			);

		printf( "%*s\t;ÃÄ %3ld%% Ä¯ %ld bytes\n", 40-len, "",
			100 - ( (nSizeAvail*100L)/nSegmentSize ),
			fileList[i].nSize
			);
		}

	free( fileList[i].szFilename ), fileList[i].szFilename = NULL;

	if ( fileList[i].szMacroOverride )
		free( fileList[i].szMacroOverride), fileList[i].szMacroOverride = NULL;
	}


void FilenamesExit( )
	{
	int nSegment;
	int nFilesLeft;
	long nSizeAvail;

	// Sort the array of filenames according to size
	qsort( (void *)fileList, nFiles, sizeof(FILEENTRY), sort_fn );

	for ( nSegment=1, nFilesLeft=nFiles; nFilesLeft; ++nSegment )
		{
		int i;

		if ( bCoff )
			printf( "GROUP%d =", nSegment );
		else
			{
			printf( "\tNEWSEG	%d\n", nSegment );
			printf( ";Ä Segment #%2d ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n",
				nSegment );
			}
		nSizeAvail = nSegmentSize;

		// Find first file in the list which can
		// fit in the remaining available space
		for ( i=0; i<nFiles; ++i )
			if ( fileList[i].szFilename )  			// not a deleted entry
				{
				if ( fileList[i].nSize > nSegmentSize )
					{
					cerr << "Warning: file \"" << fileList[i].szFilename
						<< "\" exceeds defined segment size" << endl;
					output_file( i, nSizeAvail );
					//nSizeAvail = 0;
					--nFilesLeft;
					break;
					}
				else if ( fileList[i].nSize <= nSizeAvail )
					{
					output_file( i, nSizeAvail );
					--nFilesLeft;
					continue;
					}
				}

		if ( bCoff )
			printf( "\n" );

		// If we got here, there must not be enough space.
		printf( "%sÄÄ Size = %6ld ÄÄÄ Avail = %6ld ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n\n",
			szCommentChar, nSegmentSize - nSizeAvail, nSizeAvail );
		}
	}

/////////////////////////////////////////////////////////////////////////////

typedef struct sectionEntry
	{
	char *szSectionName;
	void (*fnSectionInit)( void );
	void (*fnSectionProc)( char *szLine );
	void (*fnSectionExit)( void );
	} SECTIONENTRY;

void NoOp( void ) { }
void NoOp( char * ) { }
SECTIONENTRY nothingEntry = { NULL, NoOp, NoOp, NoOp };

SECTIONENTRY sectionList[] = {
	{ "INCLUDE", NoOp, IncludeProc, NoOp },
	{ "FILENAMES", FilenamesInit, FilenamesProc, FilenamesExit },
	{ "ASSOCIATIONS", ExtensionsInit, ExtensionsProc, ExtensionsExit },
	{ "TABSIZE", NoOp, TabSizeProc, NoOp },
	{ "SEGMENTSIZE", NoOp, SegmentSizeProc, NoOp },
	{ NULL, NoOp, NoOp, NoOp }
	};

/////////////////////////////////////////////////////////////////////////////

char szProgName[ _MAX_FNAME ];

/////////////////////////////////////////////////////////////////////////////

void
	Usage( int argc, char *argv[] )
	{
	static char usageText[] =
		{
"RomOpt Version %d.%d.%d\n\
Copyright 1992-1994 Cave Logic Studios.  All Rights Reserved.\n\
ROM Layout Organizer/Optimizer\n\
By William B. Norris IV\n\n\
Usage: %s <inFile>\n\
\n"
		};
	char szBuffer[ 512 ];

	fnsplit( argv[0], NULL, NULL, szProgName, NULL );
	strlwr( szProgName );

	if ( argc == 1 )
		{
		sprintf( szBuffer, usageText, rmj, rmm, rup, szProgName, nDefSegmentSize );
		fprintf( stderr, szBuffer );
		exit( EXIT_FAILURE );
		}
	}


//=============================================================================
// remove padding from string

int romopt( char *szInput, char *szOutput );

int
	main( int argc, char *argv[] )
	{
 	Usage( argc, argv );

	// Parse command-line switches

	if ( bCoff )
		strcpy( szCommentChar, "#" );
	else
		strcpy( szCommentChar, ";" );

	romopt( argv[1], argv[2] );

	exit( 0 );
	}


const char COMMAND_SWITCH = '*';

int
	romopt( char *szInput, char *szOutput )
	{
	FILE *fpIn, *fpOut;
	static SECTIONENTRY *se = &nothingEntry;

	{ // Print output file header
	time_t t = time( NULL );

	printf( "%sÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n", szCommentChar );
	printf( "%s³\n", szCommentChar );
	printf( "%s³ ROM allocation file \"%s\" %s", szCommentChar, szInput, ctime( &t ) );
	printf( "%s³\n", szCommentChar );
	printf( "%s³ Generated by %s Version %d.%d.%d\n", szCommentChar, szProgName, rmj, rmm, rup );
	printf( "%s³  Copyright 1992-94 by Cave Logic Studios.  All Rights Reserved.\n", szCommentChar );
	printf( "%s³\n", szCommentChar );
	printf( "%sÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n", szCommentChar );
	}


	fpIn = fopen( szInput, "rt" );

	while ( !feof( fpIn ) )
		{
		char szLine[132];

		fgets( szLine, 132, fpIn );
		UnPadString( szLine );

		if ( *szLine == '#' || *szLine == ';' )			// Comment
			continue;
		else if ( (*szLine == COMMAND_SWITCH) )			// Command
			{
			char szSectionName[80];
			int i;

			// Leave previous section
			(*se->fnSectionExit)();

			// Find command in command array
			sscanf( szLine, "* %s", szSectionName );		//!!!
			for ( i=0; sectionList[i].szSectionName; ++i )
				{
				if ( stricmp( sectionList[i].szSectionName, szSectionName ) == 0 )
					{
					se = &sectionList[i];
					break;
					}
				se = &nothingEntry;
				}
			(*se->fnSectionInit)();
			continue;
			}
		else
			(*se->fnSectionProc)( szLine );
		}
	(*se->fnSectionExit)();
	fclose( fpIn );

	if ( !bCoff )
		printf( "\tEND\n" );
	}



#if 0
{
	{ "INCLUDE", fnIncludeEntry, fnIncludeProc, fnIncludeExit },
	{ "FILENAMES", fnFilesnamesEntry, fnFilenamesProc, fnFIlesnamesExit },
}
#endif
