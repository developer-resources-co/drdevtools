//==============================================================================
// palgrab.cpp (c) 1992 Adept Creations
// By William B. Norris IV and Kevin T. Seghetti
// SNES and Genesis Palette Converter
//==============================================================================

//== C++ =======================================================================
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>

//== C =========================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//== Turbo C++ (IBM PC C libraries, but not standard C) ========================
#include <dir.h>
#include <dos.h>
#include <alloc.h>
#define _fnsplit fnsplit

//== Adept Creations (or Developer Resources) "Library" ========================
#include <pclib/general.hpp>
#include <pclib/iffilbm.hpp>
#include <pclib/iffpbm.hpp>
#include <pclib/iffanim.hpp>
#include <pclib/error.hpp>
#include <pclib/target.hpp>
#include <pclib/version.hpp>

//== Application ===============================================================

ilbmCMAP palgrabCMAP;

//==============================================================================

errorcode
	HandleCMAP( iffRead *readPtr )
	{
	return( palgrabCMAP.Read( readPtr ) );
	}

//============================================================================== Jeff is a weenie

int
	palgrab( char *szIlbm, char *szPal, int nStart=0, int nColors=-1, TargetSystem::systemType sysType = TargetSystem::SYS_AUTODETECT )
	{
	TargetSystem *ts = new TargetSystem( sysType );

	cout << "Extracting palette from ILBM file " << szIlbm <<
		" to " << ts->PlatformName() << " palette file " << szPal << "\n";

	ifstream input( (char *)szIlbm, ios::in|ios::binary );
	if ( input )
		{
		iffRead *readPtr = NULL;

		readPtr = new iffRead( input );
		readPtr->NextForm();
		if ( (readPtr->GetForm() == tagILBM) || (readPtr->GetForm() == tagPBM) ||
			 (readPtr->GetForm() == tagANIM) )
			{
			readPtr->RegisterChunk( tagCMAP, HandleCMAP );
			readPtr->ParseChunks();
			}
		else
			cout << "Error: file \"" << szIlbm << "\" not ILBM/PBM/ANIM.\n";

		delete readPtr;

		// Write out the palette in the target system format
		ofstream output( (char *)szPal, ios::out|ios::binary );
		if(!output)
			Error(ERROR_FILEIO,"cannot open output file %s",szPal);

		if ( nColors == -1 ) nColors = palgrabCMAP.GetNumColors() - nStart;

		for ( int i=nStart; i<nStart+nColors; ++i )
			{
			uint col;

			if ( ts->system() == TargetSystem::SYS_SNES )
				{
				col  = (palgrabCMAP.Blue( i ) >> (8-5)) << 10;
				col |= (palgrabCMAP.Green( i ) >> (8-5)) << 5;
				col |= (palgrabCMAP.Red( i ) >> (8-5));

				output.put( (char)(col&0xFF) );
				output.put( (char)(col>>8) );
				}
			else
				{
				col  = (palgrabCMAP.Blue( i ) >> (8-4)) << 8;
				col |= (palgrabCMAP.Green( i ) >> (8-4)) << 4;
				col |= (palgrabCMAP.Red( i ) >> (8-4));

				output.put( (char)(col>>8) );
				output.put( (char)(col&0xFF) );
				}

//			ts->write(output,col);			// write one color entry

			}

		cout << nStart << "," << nColors << " out of " << palgrabCMAP.GetNumColors() << endl;
		}
	return( 0 );
	}

//==============================================================================
// shouldn't this be derived from the command-line class?

void
	Usage( char *argv[], unsigned char *szUsageText )
	{
	unsigned char szProgName[_MAX_FNAME];

	_fnsplit( argv[0], NULL, NULL, szProgName, NULL );
	strlwr( szProgName );

	Error( ERROR_INVALID_OPTION, (char *)szUsageText, szVersion, szProgName );
	}

//==============================================================================

int
	main( int argc, char *argv[] )
	{
	unsigned char szPal[_MAX_PATH];

	if ( argc<2 )
		Usage( argv, "\nPalGrab: Palette Extractor %s (c) 1992,93 Adept Creations\n"
"IFF Graphic to Palette Converter\n"
"By William B. Norris IV and Kevin T. Seghetti\n"
"\n"
"Usage: %s [<switches>] <inputFile> [<outputFile>]\n\
	-s#:	starting color(default = 0)\n\
	-n#:	number of colors(default = all)\n\
	-P<s|g>	Platform: s=SNES, g=Genesis\n\
"
			);

	int startColor = 0;
	int numColors = -1;
	TargetSystem::systemType sysType = TargetSystem::SYS_AUTODETECT;

	int i = 1;
	while ( *argv[i] == '-' )
	 {
		switch(*(argv[i]+1))
		 {
			case 's':
			case 'S':
				sscanf( argv[i]+2, "%d", &startColor );
				break;
			case 'n':
			case 'N':
				sscanf( argv[i]+2, "%d", &numColors );
				break;

			case 'p':
			case 'P':
				switch(*(argv[i]+2))
				 {
					case 'g':
					case 'G':
						sysType = TargetSystem::SYS_GENESIS;
						break;
					case 's':
					case 'S':
						sysType = TargetSystem::SYS_SNES;
						break;
					default:
						sysType = TargetSystem::SYS_AUTODETECT;
						break;
				 }
				break;
			default:
				break;
		 }
		++i;
	 }
	if ( argc < 2+i )
		{
		unsigned char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFile[_MAX_FNAME];

		_fnsplit( argv[i], szDrive, szDir, szFile, NULL );
		_makepath( szPal, szDrive, szDir, szFile, ".pal" );
		}
	else
		strcpy( szPal, argv[i+1] );

	palgrab( argv[i], szPal, startColor, numColors, sysType );
	return(0);
	}

//==============================================================================
