/*===========================================================================*/
/*==   screen.cpp: open screen stuff                                        =*/
/*===========================================================================*/

#define INCL_BASE

#include <conio.h>

#include "base.hpp"
#include "global.hpp"
#include "screen.hpp"
#include "display.hpp"

#if defined(__GNUC__)
#include <ncurses_io.h>
#endif

/*===========================================================================*/

char *screen,*screen2;

unsigned short screenWidth,screenHeight,screenSize;
unsigned short screenWidth2,screenHeight2,screenSize2;

unsigned char fillChar;
int cursorX,cursorY;
unsigned int displayMode;
FLAG twoScreen;
FLAG primaryDispCGA;
FLAG writeProtect;

FLAG curOn;
int cursorInsert;		// active string-gadget caret: 1=insert(underline) 0=overwrite(block)
static int sStartLine, sStopLine;

//=============================================================================

void
SetSecondaryCursorPos(int x,int y)
{
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
				}
			}
		}
	}

//=============================================================================

void
CursorOff(void)
{
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
InitScreen(void)
{
#if defined(__GNUC__)
	drmon_nc_init();
	screenWidth2 = 80;
	screenHeight2 = 25;
#endif
	SetupScreen();
}

//=============================================================================


//=============================================================================

void
SetupScreen(void)
{
#if defined(__GNUC__)
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
	fillChar = ' ';
	CursorOff();
}

//=============================================================================

void
ClearScrBuff(char *scrBuff,unsigned char fillChar,unsigned char charAttr,int screenSize)
{
    int i,max;
	unsigned char *sPtr;
    sPtr = (unsigned char *)scrBuff;
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
}

//=============================================================================
