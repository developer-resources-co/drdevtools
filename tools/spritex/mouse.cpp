//
//          Mouse.C -- Mouse support for SQ
//
//
//      Russell Bornsch+
//      LEN
//		SLS
//      Feb, 1991
//		August, 1991
//

#include <dos.h>

#pragma in_line

short m_ax, m_bx, m_cx, m_dx;

#define MOUSE               mouse(&m_ax, &m_bx, &m_cx, &m_dx)
#define BUTTON_LEFT     0
#define BUTTON_RIGHT        1
#define BUTTON_UP           0
#define BUTTON_DOWN     1

void mouse(short *m1, short *m2, short *m3, short *m4)
{
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
}

//  get a number of buttons, or 0 if mouse not installed.

short CheckMouse( void )
{
    m_ax = 0;
    MOUSE;
    if (m_ax == 0)
        return(0);
    return(m_bx);
}

//  show & hide the mouse cursor

#if 1
void ShowMouse( void )
{
    m_ax = 1;
    MOUSE;
}
#endif

void HideMouse( void )
{
    m_ax = 2;
    MOUSE;
}

//      get mouse position & return buttons

short GetMouse( short *x, short *y )
{
    m_ax = 3;
    MOUSE;
    *x = m_cx;
    *y = m_dx;
    return( m_bx & 0x07 );
}

#if 0
short VGetMouse( short *x, short *y )			// kludge for vga 320x200 coord fix
{
    m_ax = 3;
    MOUSE;
    *x = m_cx>>1;
    *y = m_dx;
    return( m_bx & 0x07 );
}
#endif

//      set mouse position
void SetMouse( short x, short y )
{
    m_ax = 4;
    m_cx = x;
    m_dx = y;
    MOUSE;
}

//      get button info - state, count, x and y

short GetButton( short button, short *count, short *lastx, short *lasty )
{
    m_ax = 5;
    m_bx = button;
    MOUSE;
    *count = m_bx;
    *lastx = m_cx;
    *lasty = m_dx;
    return( m_ax & 0x07 );
}

//      get button release info - state, count, x/y

short GetButtonRelease( short button, short *count, short *lastx, short *lasty )
{
    m_ax = 6;
    m_bx = button;
    MOUSE;
    *count = m_bx;
    *lastx = m_cx;
    *lasty = m_dx;
    return( m_ax & 0x07 );
}

//      set mouse bounds

void SetMouseBounds( short minx, short maxx, short miny, short maxy )
{
    m_ax = 7;
    m_cx = minx;
    m_dx = maxx;
    MOUSE;
    m_ax = 8;
    m_cx = miny;
    m_dx = maxy;
    MOUSE;
}

//      set the text mode cursor masks

#if 0
void SetTextAttributeCursor( short andmask, short xormask )
{
    m_ax = 10;
    m_bx = 0;
    m_cx = andmask;
    m_dx = xormask;
    MOUSE;
}
#endif

#if 1
//      set the x and y motion-control ratios

void SetMouseRatio( short xratio, short yratio )
{
    m_ax = 15;
    m_cx = xratio;
    m_dx = yratio;
    MOUSE;
}

void SetGraphicPointer( char far *image, short hotx, short hoty )
{
    asm{
        PUSH    ES
        LES DX, image
        MOV BX, hotx
        MOV CX, hoty
        MOV AX, 09H
        INT 33H
        POP ES
    }
}

void SetExclusion( short left, short right, short top, short bottom )
{
    asm{
        PUSH    SI
        PUSH    DI
        MOV CX, left
        MOV SI, right
        MOV DX, top
        MOV DI, bottom
        MOV AX, 010H
        INT 33H
        POP DI
        POP SI
    }
}

unsigned char far p1[] = {
    0xFF, 0x7F, 0xFF, 0x9F, 0xFF, 0x87, 0xFF, 0xC1,
    0x7F, 0xC0, 0x1F, 0xE0, 0x07, 0xE0, 0x01, 0xF0,
    0x03, 0xF0, 0x0F, 0xF8, 0x1F, 0xF8, 0x3F, 0xFC,
    0x7F, 0xFC, 0x7F, 0xFE, 0xFF, 0xFE, 0xFF, 0xFF,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
    0x00, 0x0C, 0x00, 0x0F, 0xC0, 0x07, 0xF0, 0x07,
    0xE0, 0x03, 0xC0, 0x03, 0x80, 0x01, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned short far p2[] = {
    0x7FFF, 0x9FFF, 0x87FF, 0xC1FF,         0xC0FF, 0xE0FF, 0xE0FF, 0xF0FF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,     0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,

    0x0000, 0x0000, 0x2000, 0x1000,     0x0C00, 0x0E00, 0x0600, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000,     0x0000, 0x0000, 0x0000, 0x0000
};


#endif
