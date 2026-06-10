/*===========================================================================*/
/*==   screen.cpp: open screen stuff                                        =*/
/*===========================================================================*/

#define INCL_BASE

#include <conio.h>
#ifdef __MSDOS__
#include <dos.h>
#endif

#ifdef DOSX286
#include <phapi.h>
#endif

#include "base.hpp"
#include "global.hpp"
#include "screen.hpp"
#include "display.hpp"

#if defined(__GNUC__)
#include <ncurses_io.h>
#endif

#ifdef __OS2__
#define INCL_BASE
#include <bse.h>
#endif

#ifdef __MSDOS__
#include "chargfx.hpp"
#endif

/*===========================================================================*/

#ifdef __OS2__
char _far16 *screen;
char _far16 *screen2;
#else
char far *screen,far *screen2;
#endif

#ifdef __BORLANDC__
text_info ti;
#endif

unsigned short screenWidth,screenHeight,screenSize;
unsigned short screenWidth2,screenHeight2,screenSize2;

unsigned char fillChar;
int cursorX,cursorY;
unsigned int displayMode;
FLAG twoScreen;
FLAG primaryDispCGA;
FLAG writeProtect;

FLAG curOn;
static int sStartLine, sStopLine;

//=============================================================================

void
SetSecondaryCursorPos(int x,int y)
{
#ifdef __MSDOS__
	int pos;
	pos = (screenWidth2*y)+x;                                               // calculate new cursor position
	outportb(0x3b4,15);                                     // select register 15
	outportb(0x3b5,pos & 0xff);                     // set cursor position, low byte
	outportb(0x3b4,14);                                     // select register 14
	outportb(0x3b5,(pos>>8) & 0xff);        // set cursor position, high byte
#else
#ifdef __OS2__
	{ // Need to set up "sessions" or get handles to other devices (or something) for OS/2s
	}
#endif
#endif
}

//=============================================================================

void
PositionCursor(int xPos,int yPos)
	{
	FLAG offScreen = boolean::FALSE;

	xPos -= virtualXOffset;
	yPos -= virtualYOffset;

	if(xPos >= (int)displayWidth)
		offScreen = boolean::TRUE;
	if(xPos < 0)
		offScreen = boolean::TRUE;
	if(yPos >= (int)screenHeight)
		offScreen = boolean::TRUE;
	if(yPos < 0)
		offScreen = boolean::TRUE;

	cursorX = xPos;
	cursorY = yPos;

	if (curOn )
		{
		if (offScreen)
			{
			CursorOff();                    // cheat, make invisible, but keep status on
			curOn = boolean::TRUE;
			}
		else
			{ // must be on second screen
			if (xPos >= (int)screenWidth)
				{
				CursorOff();                    // erase cursor from primary screen
				SetSecondaryCursorPos(xPos-screenWidth,yPos);
								// draw cursor on secondary screen
				curOn = boolean::TRUE;
				}
			else
				{ // erase cursor from secondary screen
				SetSecondaryCursorPos(0,screenHeight);

#ifdef __MSDOS__
				union REGS reg;

				reg.h.ah = 2;                   // function(set cursor position)
				reg.h.bh = 0;                   // page #

				reg.h.dl = xPos;
				reg.h.dh = yPos;
				int86(0x10,&reg,&reg);          // draw cursor on primary screen
#else
#ifdef __OS2__
				VioSetCurPos( yPos, xPos, 0 );  // draw cursor on primary screen
#endif
#endif
				}
			}
		}
	}

//=============================================================================

void
CursorOff(void)
{
#ifdef __OS2__
	VIOCURSORINFO ci;

	ci.yStart = sStartLine;
	ci.cEnd = sStopLine;
	ci.cx = 1;
	ci.attr = -1;

	VioSetCurType( &ci, 0 );
#else
#ifdef __MSDOS__
	union REGS reg;

	reg.h.bh = 0;                                   // page
    reg.h.ah = 2;
    reg.h.dh = screenHeight;                                    // row off of screen
    reg.h.dl = 0;
    int86(0x10, &reg, &reg);
#endif
#endif
	SetSecondaryCursorPos(0,screenHeight);
	curOn = boolean::FALSE;
}

//=============================================================================

void
CursorOn(void)
{
	curOn = boolean::TRUE;
	PositionCursor(cursorX,cursorY);
}

//=============================================================================

void
SetCursorStartStop(int startLine,int stopLine)
{
#ifdef __OS2__
	VIOCURSORINFO ci;

	sStartLine = ci.yStart = startLine;
	sStopLine = ci.cEnd = stopLine;
	ci.cx = 1;
	ci.attr = curOn ? 0 : -1;

	VioSetCurType( &ci, 0 );
#else
#ifdef __MSDOS__
    union REGS reg;
	reg.h.ah = 1;                   // function(set cursor start/stop)
	reg.h.ch = startLine;
	reg.h.cl = stopLine;

	int86(0x10,&reg,&reg);
#endif
#endif
}

//=============================================================================

void
InitScreen(void)
{
#if defined(__GNUC__)
	drmon_nc_init();
	screenWidth2 = 80;
	screenHeight2 = 25;
#endif
#ifdef __MSDOS__
	gettextinfo(&ti);
	screenWidth2 = 80;
	screenHeight2 = 25;
#endif
	SetupScreen();
}

//=============================================================================

#ifdef __MSDOS__
struct _dispInfo
{
	unsigned int activePage;
	unsigned int activeMode;
	unsigned int width;
	unsigned int height;
};

//=============================================================================

void
GetDisplayInfo(_dispInfo *di)
{
	asm {
		push    ds
		push    es

		push    bp
		les             di,[bp+6]

		mov             ah,0fh
		int             10h
		mov             dx,ax
		xor             ax,ax

		mov             al,bh
		stosw
		mov             al,dl
		stosw
		mov             al,dh
		stosw

		push    es
		mov             ax,1130h
		mov             bh,2
		int             10h
		xor             dh,dh
		inc             dx
		pop             es
		mov             es:[di],dx
		pop             bp

		pop         es
		pop             ds
	}
}
#endif

//=============================================================================

void
SetupScreen(void)
{
#ifdef __OS2__
	VIOMODEINFO vmi;

	clrscr();
	VioGetMode( &vmi, 0 );
	screenWidth = vmi.col;
	screenHeight = vmi.row;

	VioGetBuf( (PULONG16)&screen, &screenSize, 0 );
#else
#ifdef __MSDOS__
	union REGS in,out;
	_dispInfo di;
	text_info localTi;

	in.h.ah = 0x00;
	if(displayMode < 0x100)
		in.h.al = (unsigned char)displayMode;
	else
		in.h.al = 3;
	in.h.bl = 0;
	int86(0x10,&in,&out);

	if(displayMode == 0x100)                                // set to 43 line mode
	 {
		in.x.ax = 0x1112;
		in.h.bl = 0;
		int86(0x10,&in,&out);                           // download smaller font

//              *(char far *)0x00000487 |= 1;           // enable cursor emulation
		in.x.ax = 0x1200;
		in.h.bl = 0x34;
		int86( 0x10, &in, &out );

		in.h.ah = 0x01;
		in.x.cx = 0x0600;
		int86(0x10,&in,&out);                           // set cursor to a block

#if 0
		asm {

			push    ds
			push    si
			sub     ax,ax
			mov     ds,ax
			mov     si,487h
			or      byte ptr ds:[si],1

			mov     cx,0600h
			mov     ah,01
			int     10h

			mov dx,03d4h
			mov     al,14h
			out dx,al
			inc     dx
			mov al,07h
			out     dx,al

			sub     ax,ax
			mov     ds,ax
			mov     si,487h
			and     byte ptr ds:[si],0feh

			pop     si
			pop     ds
		}
#endif
	 }
	else if(displayMode == 0x101)                           // set to 50 line mode
	 {
		// someday maybe I will write this
	 }

	clrscr();
	GetDisplayInfo(&di);
    screenWidth = di.width;
    screenHeight = di.height;

	if(primaryDispCGA)
	 {
#ifdef DOSX286
		{
		unsigned short sel;

		if ( screen )
			DosFreeSeg( FP_SEG( screen ) );
		DosMapRealSeg( 0xB800, (long)di.width*di.height*charSize, &sel );
		screen = (char far*)MAKEP( sel, 0 );

		if ( screen2 )
			DosFreeSeg( FP_SEG( screen2 ) );
		DosMapRealSeg( 0xB000, (long)25*80*2, &sel );
		screen2 = (char far*)MAKEP( sel, 0 );
		}
#else
	    screen = (char far *)0xb8000000;             // primary is CGA/EGA/VGA
	screen2 = (char far *)0xb0000000;                // secondary is monochrome
#endif
	 }
	else
	 {
		screenHeight = 25;                                              // kludge, since getdisplayinfo doesn't work for monochrome
#ifdef DOSX286
		{
		unsigned short sel;

		if ( screen )
			DosFreeSeg( FP_SEG( screen ) );
		DosMapRealSeg( 0xB000, (long)80*25*charSize, &sel );
		screen = (char far*)MAKEP( sel, 0 );

		if ( screen2 )
			DosFreeSeg( FP_SEG( screen2 ) );
		DosMapRealSeg( 0xB800, (long)di.width*di.height*charSize, &sel );
		screen2 = (char far*)MAKEP( sel, 0 );
		}
#else
	    screen = (char far *)0xb0000000;
	screen2 = (char far *)0xb8000000;
#endif
	 }

    screenSize = screenWidth*screenHeight*charSize;
    screenSize2 = screenWidth2*screenHeight2*charSize;

	{ // Turn on 16-background colors
	_AX = (0x10 << 8) | 0x03;
	_BL = 0;                                                                        // Enable backgound intensities
	geninterrupt( 0x10 );
	}

#if 0
	{
	_AX = (0x11 << 8) | 0x03;
	_BL = (1<<2) | 0;
	geninterrupt( 0x10 );
	}

	WrFont( (void _far *)SetupDisplay, 0, di.height );
#endif

#elif defined(__GNUC__)
	// Linux: fixed 80x25 text screen rendered to the terminal by ncurses.
	{
		extern uint backgroundChar, backgroundAttr;
		backgroundChar = ' ';
		backgroundAttr = 0x17;          // white on blue (classic)
	}
	primaryDispCGA = boolean::TRUE;
	// Fill the terminal: size the CGA screen to the actual COLS x LINES (floored at
	// 80x25). Re-read on every call so ReSizeViewport() picks up live terminal resizes.
	{ int w = 80, h = 25; drmon_nc_size(&w, &h); screenWidth = w; screenHeight = h; }
	screenWidth2 = 80;  screenHeight2 = 25;   // secondary mono screen: only used if twoScreen
	if(screen)  free(screen);
	if(screen2) free(screen2);
	screen  = (char*)calloc((long)screenWidth *screenHeight *charSize, 1);
	screen2 = (char*)calloc((long)screenWidth2*screenHeight2*charSize, 1);
	screenSize  = screenWidth *screenHeight *charSize;
	screenSize2 = screenWidth2*screenHeight2*charSize;
#endif
#endif
	fillChar = ' ';
	SetCursorStartStop(6,7);
	CursorOff();
}

//=============================================================================

void
ClearScrBuff(char far *scrBuff,unsigned char fillChar,unsigned char charAttr,int screenSize)
{
    int i,max;
	unsigned char far *sPtr;
    sPtr = (unsigned char far*)scrBuff;
    max = screenSize/2;
    for(i=0;i<max;++i)
	{
	*sPtr++ = fillChar;
	*sPtr++ = charAttr;
	}
}

//=============================================================================

void
CloseScreen(void)
{
	clrscr();
	gotoxy(1,1);
	SetCursorStartStop(6,7);
#ifdef __MSDOS__
	textmode(ti.currmode);                  //restore original text mode
#endif
}

//=============================================================================
