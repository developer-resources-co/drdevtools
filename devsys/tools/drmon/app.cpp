
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
//#include <dir.h>
//#include <mem.h>
//#include <alloc.h>
#include <dos.h>

#ifdef DOSX286
#include <phapi.h>
#endif

#include "compat.hpp"
#include "app.hpp"

App::App( int argc, char* argv[] )
	{
#ifdef __MSDOS__
	fn = new filename( argv[0] );

	unsigned short nVideoSeg;

	gettextinfo( &ti );

	nVideoSeg = ti.currmode==7 ? 0xB000 : 0xB800;

	int width = ti.winright-ti.winleft+1;
	int height = ti.winbottom-ti.wintop+1;
	cbScreen = width * height * 2;

#ifdef DOSX286
	unsigned short sel;

	DosMapRealSeg( nVideoSeg, cbScreen, &sel );
	pPhysicalScreenMem = MAKEP( sel, 0 );
#else
	pPhysicalScreenMem = MK_FP( nVideoSeg, 0 );
#endif

	// Get the current cursor position
	x = ti.curx;
	y = ti.cury;

	// Save screen
	if ( pScreenMem = farmalloc( cbScreen ) )
		_fmemcpy( pScreenMem, pPhysicalScreenMem, cbScreen );

	disk = getdisk();
	getcwd( szStartingDirectory, _MAX_PATH );
#endif
	}


App::~App()
	{
#ifdef __MSDOS__
	chdir( szStartingDirectory );
	setdisk( disk );

	_AH = 0;
	_AL = ti.currmode;
	_BL = 0;
	geninterrupt( 0x10 );

	if ( pScreenMem )
		{
		// Restore cursor
		gotoxy( x, y );

		// Restore screen
		_fmemcpy( pPhysicalScreenMem, pScreenMem, cbScreen );
		farfree( pScreenMem );

#ifdef DOSX286
		DosFreeSeg( FP_SEG( pPhysicalScreenMem ) );
#endif
		}
	else
		{
		gotoxy( 1, 1 );
		}
#endif
	}
