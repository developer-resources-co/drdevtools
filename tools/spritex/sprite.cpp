//==============================================================================
/*
 * Sprite Converter.  (c) 1991 Developer Resources, by Scott Statton
 * Based heavily on CharGrab by Lars Norpchen, K. T. Seghetti
 */
//==============================================================================
//==============================================================================
// sprite.cpp: new improved sprite tool
// By Kevin T. Seghetti (c) 1993 Developer Resources.  All Rights Reserved.
//==============================================================================

// pclib
#include <pclib/general.h>
#include <pclib/boolean.h>
#include <pclib/iff.h>
#include <pclib/grphport.h>

// local
#include "menu.hpp"
#include "sprite.hpp"

//==============================================================================

#include <conio.h>
#include <mem.h>
#include <dos.h>
#include <stdio.h>
#include <alloc.h>

#include "utypes.hpp"
#include "global.hpp"
#include "general.hpp"
#include "sq.hpp"
#include "sprinpt.hpp"
#include	"input.hpp"
#include "sprite.hpp"
#include "spmain.hpp"
#include	"compilbm.hpp"
#include	"filereq.hpp"
#include	"image.hpp"
#include	"mouse.hpp"

//==============================================================================

FILE *fp;

extern struct PANE pane;
extern boolean spriteBorder;
extern boolean chrNameboolean;
extern char far PanelBitmap;
extern char far PanelPalette[];
struct VIEWPORT mainscreen = { 320 , 255, 319, 8, 199 };

//==============================================================================

screenGraphPort* display;
gfxText* font;

struct PANE spriteBase;	// = { boolean::FALSE, 0, 0, 0, 0, 0, 0, NULL, NULL } ;
struct PANE pane;			// = { boolean::FALSE, 0,0,0,0,0,0, NULL, NULL };
struct GRID grid;			// = { boolean::FALSE, 8, 8, 0, 0 } ;
struct GRAPHIC graphic;	// = { boolean::FALSE , 0, 0 } ;

Animation myPicture;


main (int argc,char *argv[])
{
	spriteBase.active = boolean::FALSE;
	spriteBase.x =
	spriteBase.y =
	spriteBase.sizex =
	spriteBase.sizey =
	spriteBase.prevx =
	spriteBase.prevy = 0;
	spriteBase.gPtr = NULL;
	spriteBase.next =
	spriteBase.prev = NULL;

	pane.active = boolean::FALSE;
	pane.x =
	pane.y =
	pane.sizex =
	pane.sizey =
	pane.prevx =
	pane.prevy = 0;
	pane.gPtr = NULL;
	pane.next =
	pane.prev = NULL;

	grid.flag = boolean::FALSE;
	grid.sizeX =
	grid.sizeY = 8;
	grid.offsetX =
	grid.offsetY = 0;

	graphic.flag = boolean::FALSE;
	graphic.offsetX =
	graphic.offsetY =
	graphic.sizeX =
	graphic.sizeY =
	graphic.upLeftX =
	graphic.upLeftY =
	graphic.lowRightX =
	graphic.lowRightY = 0;
	graphic.midFlag = boolean::FALSE;


	GetInput (argc,argv);
	clrscr ();
//	T_setup_font();
//	printf( "Initializing...\n" );

	display = new screenGraphPort(320,200,256,color(0,0,0));		// setup screen
extern ubyte far systemFont[];
	font = new gfxText( display, systemFont, 64, 8,8 );

	colorPalette *sprPal = new colorPalette();
	for(int i=0;i<256;i++)
	 {
		sprPal->Create(color(PanelPalette[i*3], PanelPalette[(i*3)+2], PanelPalette[(i*3)+2]));
	 }
	display->Palette(*sprPal);					// force our own 256 color palette
//	menuBar spriteMenuBar(display);

	InitInput();
	graphic.imagePtr = &PanelBitmap;
 	graphic.sizeX = 80;
 	graphic.sizeY = 193;
	spriteBorder= boolean::FALSE;
	chrNameboolean = boolean::FALSE;
	NewDrawClippedBitmap(&myPicture, &mainscreen, 0,0);

//	GetPal();					// what does this do?

	fileActive = pane.active = boolean::FALSE;
	ShowMouse();
	while ( DoMainLoop() )
		;

	delete display;

	return( 0 );
}

//==============================================================================

