////////////////////////////////////////////////////////////////////////////////
// abmsplit.cpp

//== C++ =======================================================================
#include <iostream.h>
#include <fstream.h>

//== C =========================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>

//== Turbo C++ (IBM PC C libraries, but not standard C) ========================
#include <dir.h>
#include <dos.h>
#include <alloc.h>
#include <conio.h>

//== Adept Creations Library ===================================================
#include <pclib/general.hpp>
#include <pclib/iffilbm.hpp>
#include <pclib/error.hpp>
#include <pclib/mem.hpp>
#include <pclib/grphport.hpp>

//== Old Fucked (or Developer Resources) "Library" =============================
//#include <lib/display.h>

//== Application ===============================================================
#include "abmsplit.h"
#include "grabinpt.h"
#include "cloption.h"
#include "quit.h"

//==============================================================================

rastGraphPort *display;

//==============================================================================

extern unsigned char far *image[];

//==============================================================================

void
	check_ctrl_break( void )
	{
	kbhit();
	}

extern void ReadANIM( const char *szAnimName, char* szPictName );

extern void WritePict( char* );

	main (int argc,char *argv[])
	{
	GetInput( argc, argv );
	ctrlbrk( ctrlBreakHandler );
	printf( "Initializing...\r\n" );

	if ( bDisplay )
		display = new screenGraphPort( 320, 200, 256 );
	else
		{
		display = new rastGraphPort( 320, 200, 256 );
		cout << "Reading ANIM \"" << AnimName << "\"...\n";
		}
	ReadANIM( AnimName, szPictName );

	delete display;

	Quit();
	}

