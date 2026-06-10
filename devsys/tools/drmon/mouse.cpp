//==============================================================================
//
//          Mouse.C -- Mouse support for SQ
//
//
//      Russell Bornsch+
//      LEN
//      Feb, 1991
//
//==============================================================================

#ifndef __OS2__
#define DO_MOUSE
#endif

#ifdef __WATCOMC__
#undef DO_MOUSE
#endif

#include <assert.h>

#include <dos.h>

#include "moninc.hpp"

#if defined(__GNUC__)
#include <ncurses_io.h>     // Linux: mouse comes from the ncurses backend
#endif

#pragma in_line

//==============================================================================

static short m_ax, m_bx, m_cx, m_dx;

#define MOUSE               mouse(&m_ax, &m_bx, &m_cx, &m_dx)
#define BUTTON_LEFT     0
#define BUTTON_RIGHT        1
#define BUTTON_UP           0
#define BUTTON_DOWN     1

//==============================================================================

void mouse(short *m1, short *m2, short *m3, short *m4)
{
#ifdef DO_MOUSE
    union REGS reg;

    reg.x.ax = *m1;
    reg.x.bx = *m2;
    reg.x.cx = *m3;
    reg.x.dx = *m4;
    int86(0x33, &reg, &reg);
    *m1 = reg.x.ax;
    *m2 = reg.x.bx;
    *m3 = reg.x.cx;
    *m4 = reg.x.dx;
#endif
}

//==============================================================================
//  get a number of buttons, or 0 if mouse not installed.

short CheckMouse( void )
{
#if defined(__GNUC__)
	return( drmon_nc_havemouse() ? 2 : 0 );   // left + right
#elif defined(DO_MOUSE)
    m_ax = 0;
    MOUSE;
    if (m_ax == 0)
        return(0);
    return(m_bx);
#else
	return( 0 );
#endif
}

//==============================================================================

void HideMouse( void )
{
#ifdef DO_MOUSE
    m_ax = 2;
    MOUSE;
#endif
}

//==============================================================================
//      get mouse position & return buttons

short GetMouse( short *x, short *y )
{
#if defined(__GNUC__)
	return( drmon_nc_getmouse( x, y ) );
#elif defined(DO_MOUSE)
    m_ax = 3;
    MOUSE;
    *x = m_cx;
    *y = m_dx;
    return( m_bx & 0x07 );
#else
	return( 0 );
#endif
}

//==============================================================================
//      get button info - state, count, x and y

short GetButton( short button, short *count, short *lastx, short *lasty )
{
#ifdef DO_MOUSE
    m_ax = 5;
    m_bx = button;
    MOUSE;
    *count = m_bx;
    *lastx = m_cx;
    *lasty = m_dx;
    return( m_ax & 0x07 );
#else
	return( 0 );
#endif
}

//==============================================================================
//      get button release info - state, count, x/y

short GetButtonRelease( short button, short *count, short *lastx, short *lasty )
{
#ifdef DO_MOUSE
    m_ax = 6;
    m_bx = button;
    MOUSE;
    *count = m_bx;
    *lastx = m_cx;
    *lasty = m_dx;
    return( m_ax & 0x07 );
#else
	return( 0 );
#endif
}

//==============================================================================
//      set mouse bounds

void SetMouseBounds( short minx, short maxx, short miny, short maxy )
{
#ifdef DO_MOUSE
    m_ax = 7;
    m_cx = minx;
    m_dx = maxx;
    MOUSE;
    m_ax = 8;
    m_cx = miny;
    m_dx = maxy;
    MOUSE;
#endif
}

//==============================================================================
