
#include <stdio.h>
#include <conio.h>

#include <bios.h>

#include <pclib/general.hpp>
#include <pclib/grphport.hpp>
#include <pclib/iffilbm.hpp>

#include "keys.hpp"
#include "display.hpp"
#include "grabber.hpp"

extern int charX, charY;
extern unsigned char far *image[];
extern ubyte mapBad[MAXGRID_Y][MAXGRID_X];
extern int pixelWidth;

extern ilbmCMAP myCMAP;
extern ilbmBMHD myBMHD;

void
	drawPage( int xMapOffset, int yMapOffset )
	{
	int x,y;
	int ixmo;

	xMapOffset *= charX;
	yMapOffset *= charY;

	for ( y=0; y<SCRYSIZE; ++y )
		for ( ixmo=xMapOffset, x=0; x<SCRXSIZE; ++x, ++ixmo )
			{
			int i = GetPixel( ixmo, y );

			if ( ixmo < myBMHD.w )
				display->SetColorIndex( i );
			else
				display->SetColor( color(0,0,0) );

			display->PutPixel( point(x,y) );

//			VGAPutPixel( x, y,
//				ixmo<pixelWidth ? *(image[y+yMapOffset]+ixmo) : 0
//				);
			}
	}


#if 0
static char szBuffer[ 1024 ];

static const char szBrowseHelp[] =
{
PLATFORMNAME " Character Grabber %s Help\n"
"\n"
"   Esc/"
"   Alt-X  Exit browse mode\n"
};

void
	ShowHelp( const char *szHelpText )
	{
	ViewPort subScreen( 0,0 );

	MessageBox( &subScreen, -1,-1, szHelpText, szVersion );
	}
#endif



void
	ScrollAroundPicture( void )
	{
	int c;

	do
		{
		int xMap = 0;
		int yMap = 0;
		int xInc, yInc;

		// Draw at current (xMap,yMap) location
		drawPage( xMap, yMap );
		{ // Erase the valid characters
		display->SetColor( color(0,0,0) );
		for ( int y=0; y<SCRYSIZE/charY; ++y )
			for ( int x=0; x<SCRXSIZE/charX; ++x )
				{
				if ( mapBad[y+yMap][x+xMap] )
					; // Leave on screen
				else
					{
					int x1 = x*charX;					// Top of box
					int y1 = y*charY;

					display->Box( point(x1,y1), point(x1+charX-1,y1+charY-1) );
					}
				}
		}

		while ( !kbhit() )
			;
		c = getch();
		if ( c==0 ) c = KEY_EXT | getch();		// Retrieve extended code;

		xInc = yInc = 0;
		switch ( c )
			{
			case KEY_HOME:
				xInc = -xMap;
				break;

			case KEY_END:
				xInc = 0xFFFF;				// won't work...
				break;

			case KEY_LEFT:
				xInc = -1;
				break;

			case KEY_RIGHT:
				xInc = +1;
				break;

			case KEY_PGUP:
				yInc = -yMap;
				break;


			case KEY_PGDN:
				yInc = 0xFFFF;				// won't work...
				break;

			case KEY_UP:
				yInc = -1;
				break;

			case KEY_DOWN:
				yInc = +1;
				break;

			case KEY_HELP:
//				ShowHelp( szBrowseHelp );
				break;
			}

		if ( bioskey(2) & 3 )						// SHIFT == Turbo Key
			{
			xInc *= (SCRXSIZE/charX)/2;		//	1/2 screen
			yInc *= (SCRYSIZE/charY)/2;		// 1/2 screen
			}

		xMap += xInc;
		if ( xMap < 0 ) xMap = 0;
//		if ( xMap > ...

		yMap += yInc;
		if ( yMap < 0 ) yMap = 0;
//		if ( yMap >	...
		}
	while ( (c != KEY_ESC) && (c != KEY_ALTX) );
	}
