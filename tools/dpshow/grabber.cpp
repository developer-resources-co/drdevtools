//===========================================================================
// grabber.cpp: IFF to SNES character grabber By Kevin T. Seghetti
// based on code by Lars Norphcen
// SNES additions and other modifications by William B. Norris IV
//===========================================================================

//== C++ ====================================================================
#include <iostream.h>
#include <fstream.h>

//== C ======================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>

//== Turbo C++ (IBM PC C libraries, but not standard C) =====================
#include <dir.h>
#include <dos.h>
//#include <alloc.h>
#include <conio.h>

//== Adept Creations Library ================================================
#include <pclib/general.hpp>
#include <pclib/iff.hpp>
#include <pclib/iffilbm.hpp>
#include <pclib/error.hpp>
#include <pclib/mem.hpp>
#include <pclib/grphport.hpp>

//== Application ============================================================
#include "grabber.hpp"
#include "ilbm.hpp"
#include "grabinpt.hpp"
#include "cloption.hpp"
#include "quit.hpp"

//===========================================================================

extern unsigned char far *image[];

//===========================================================================

void
	check_ctrl_break( void )
	{
	kbhit();
	}


colorMapIndex
	MixFunction( colorMapIndex col1, colorMapIndex col2 )
	{
	return( col1*16 + col2 );
	}


rastGraphPort*
	Mix( rastGraphPort* pict1, rastGraphPort* pict2 )
	{
	rastGraphPort* pict = new rastGraphPort( 320, 200, 256 );

#if 0
	for ( int i=0; i<256; ++i )
		pict->
#endif

	for ( int y=0; y<pict1->GetHeight(); ++y )
		{
		for ( int x=0; x<pict1->GetWidth(); ++x )
			{
			point p(x,y);
			colorMapIndex col = MixFunction( pict1->GetPixelIndex( p ), pict2->GetPixelIndex( p ) );
			pict->SetColorIndex( col );
			pict->PutPixel( p );
			}
		}

	// Create a new palette!
	return( pict );
	}


void WritePict(char* szPictName, rastGraphPort* );

	main (int argc,char *argv[])
	{
	rastGraphPort* pict1, *pict2, *pictOut;
	screenGraphPort* screen = new screenGraphPort( 320, 200, 256 );

	GetInput( argc, argv );
	ctrlbrk( ctrlBreakHandler );

	{
	pict1 = new rastGraphPort( 320, 200, 256 );
	ReadILBM( Input1Name, pict1 );
	for ( int y=0; y<200; ++y )
		{
		for ( int x=0; x<320; ++x )
			{
			point p(x,y);
			screen->SetColorIndex( pict1->GetPixelIndex( p ) );
			screen->PutPixel( p );
			}
		}
	}

	{
	pict2 = new rastGraphPort( 320, 200, 256 );
	ReadILBM( Input2Name, pict2 );
	for ( int y=0; y<200; ++y )
		{
		for ( int x=0; x<320; ++x )
			{
			point p(x,y);
			screen->SetColorIndex( pict2->GetPixelIndex( p ) );
			screen->PutPixel( p );
			}
		}
	}

	{
	pictOut = Mix( pict1, pict2 );
	for ( int y=0; y<200; ++y )
		{
		for ( int x=0; x<320; ++x )
			{
			point p(x,y);
			screen->SetColorIndex( pictOut->GetPixelIndex( p ) );
			screen->PutPixel( p );
			}
		}
	WritePict( OutputName, pictOut );
	}

	delete pictOut;
	delete pict2;
	delete pict1;

	delete screen;

	Quit();
	}








