//=============================================================================
// display.hpp:
//=============================================================================

#ifndef DRMON_display_HPP
#define DRMON_display_HPP

#include "input.hpp"

//=============================================================================

#ifdef __OS2__
extern char _far16 *scrBuffer;
#include <os2.h>
#else
extern char far *scrBuffer;
#endif

extern unsigned int displayWidth,displayHeight;
extern unsigned int virtualXOffset,virtualYOffset,virtualXSize,virtualYSize;

void
SetupDisplay(void);

void
InitDisplay(void);

void
UpdateScreen(void);

void
XorDisplayRect(int startX, int startY, int endX, int endY);

void
ReadDisplayRect(char *buffer,int startX, int startY, int endX, int endY);

void ScrollDisplayRight(int distance);
void ScrollDisplayLeft(int distance);
void ScrollDisplayUp(int distance);
void ScrollDisplayDown(int distance);

void ScrollToMouse(_input *in);

//=============================================================================

#endif

//=============================================================================
