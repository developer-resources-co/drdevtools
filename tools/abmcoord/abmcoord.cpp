////////////////////////////////////////////////////////////////////////////////
// anmsplit.cpp

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
#include <pclib/mstream.hpp>
#include <pclib/strmnull.hpp>

//== Application ===============================================================
#include "anmsplit.hpp"
#include "grabinpt.hpp"
#include "cloption.hpp"
#include "quit.hpp"

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

extern void ReadANIM( const char *szAnimName, char* szSrcName );

	main (int argc,char *argv[])
	{
	mout << "AbmCoord";

	GetInput( argc, argv );
	ctrlbrk( ctrlBreakHandler );

	if ( bDisplay )
		display = new screenGraphPort( 320, 200, 256 );
	else
		{
		display = new rastGraphPort( 320, 200, 256 );
		cout << "Reading ANIM \"" << AnimName << "\"...\n";
		}
	ReadANIM( AnimName, szSrcName );

	delete display;

	Quit();
	}

