//=============================================================================
// display.hpp:
//=============================================================================

#ifndef DRMON_display_HPP
#define DRMON_display_HPP

#include "input.hpp"

//=============================================================================

extern char *scrBuffer;

extern unsigned int displayWidth,displayHeight;
extern unsigned int virtualXOffset,virtualYOffset,virtualXSize,virtualYSize;

void
SetupDisplay(void);

void
InitDisplay(void);

void
UpdateScreen(void);

#if defined(__GNUC__)
void
ReSizeViewport(void);		// Linux: re-fill the screen after a terminal resize
#endif

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
