//
//          Mouse.H -- mouse stuff for SQ
//
//
//      Russell Bornsch+
//      LEN
//      Feb, 1991
//
//==============================================================================

#ifndef _BAD_MOUSE_CODE_
#define _BAD_MOUSE_CODE_

//==============================================================================

const int BUTTON_LEFT = 0;
const int BUTTON_RIGHT = 1;
const int BUTTON_UP = 0;
const int BUTTON_DOWN = 1;

short  CheckMouse( void );
void ShowMouse( void );
void HideMouse( void );
short  GetMouse( short *x, short *y );
short  VGetMouse( short *x, short *y );
void SetMouse( short x, short y );
short  GetButton( short button, short *count, short *lastx, short *lasty );
short  GetButtonRelease( short button, short *count, short *lastx, short *lasty );
void SetMouseBounds( short minx, short maxx, short miny, short maxy );
void SetTextAttributeCursor( short andmask, short xormask );
void SetMouseRatio( short xratio, short yratio );
void SetGraphicPointer( char far *image, short hotx, short hoty );
void SetExclusion( short left, short right, short top, short bottom );
extern unsigned char far p1[];
extern unsigned short far p2[];

//==============================================================================

#endif

//==============================================================================
