

//
//      SQ.C -- graphics routines for BUGSQUAD
//
//      Russell Bornsch+
//      January, 1991
//

//
//      The following graphics routines are designed for VGA/MCGA
//      320x200 256-color mode (13h). G_set_mode() should be called
//      with an argument of 0x13 to enter this mode, and 0x03 to
//      return to 80-column color text mode. UNLESS OTHERWISE STATED,
//      NONE OF THE OTHER FUNCTIONS WILL WORK PROPERLY, IF AT ALL,
//      IN ANY OTHER MODES.
//

#pragma in_line

// pclib
#include <pclib\general.h>
#include <pclib\iff.h>
#include <pclib\grphport.h>

// local
#include "menu.hpp"
#include "sprite.hpp"

#include "utypes.hpp"
#include "global.hpp"
#include "spmain.hpp"

#include "sq.hpp"

//      SQ_set_RGB_color() uses the BIOS set 256-color palette entry
//      call to set entry number "entry" to the RGB color specified.
//      The color pointed to by "rgb" should be in specified with
//      red, green, and blue values being 0-63 (not 0-255).
//      This and the following three functions should work in any
//      256-out-of-256K color mode.

void SQ_set_RGB_color( int entry, RGBcolor far *rgb )
{
    asm{
         mov dx,3dah
        }
    wait4retrace:
    asm{
        in al,dx
        test al,08h
        jz wait4retrace
        PUSH    ES
        LES DI, rgb
        MOV AX, 1010H
        MOV BX, entry
        MOV DH, ES:[DI].r
        MOV CH, ES:[DI].g
        MOV CL, ES:[DI].b
        INT 10H
        POP ES
    }
}

//      SQ_get_RGB_color() uses a BIOS call to retrieve an RGB color.
//      The parameters have the same meanings as for SQ_set_RGB_color(),
//      but "rgb" is the pointer to the destination, rather than source.

void SQ_get_RGB_color( int entry, RGBcolor far *rgb )
{
    asm{
        PUSH    ES
        LES DI, rgb
        MOV AX, 1015H
        MOV BX, entry
        INT 10H
        MOV ES:[DI].r, DH
        MOV ES:[DI].g, CH
        MOV ES:[DI].b, CL
        POP ES
    }
}

//      SQ_set_palette() sets a continuous set of palette entries
//      in a way similar to SQ_set_RGBcolor(). "start" is the first entry
//      to set, "count" is the number of entries to set, and "rgb"
//      is now the base of an array of RGBcolor structures.

void SQ_set_palette( int start, int count, RGBcolor far *rgb )
{
    asm{
        PUSH    ES
        MOV AX, 1012H
        MOV BX, start
        MOV CX, count
        LES DX, rgb
        INT 10H
        POP ES
    }
}

//      SQ_get_palette() is the complement of SQ_set_palette().

void SQ_get_palette( int start, int count, RGBcolor far *rgb )
{
    asm{
        PUSH    ES
        MOV AX, 1017H
        MOV BX, start
        MOV CX, count
        LES DX, rgb
        INT 10H
        POP ES
    }
}

//
//      SQ_get_screen_address() returns a pointer to the screen memory
//      at coordinates ("x", "y"). Useful to get source and destination
//      pointers for copy operations.
//

char far *SQ_get_screen_address( int x, int y )
{
    asm{
        MOV DX, 0A000H
        MOV BX, y               // multiplies y by 320
        MOV AX, BX
        MOV CX, 2
        SHL AX, CL
        ADD AX, BX
        MOV CX, 6
        SHL AX, CL          // ax = y * 320
        ADD AX, x
    }
}

//
//      SQ_hline() draws a horizontal line from "x1" to "x2" at
//      y-coord "y" in the specified color.
//

void SQ_hline( int x1, int x2, int y, uchar color )
{
    asm{
        PUSH    ES
        MOV AX, 0A000H      //segment of screen
        MOV ES, AX

        MOV BX, x1
        MOV CX, x2
        SUB CX, BX         //x2-x1 = length
        JNS lineok         //is x2 > x1 ?

        NEG CX             //no, fix length
        MOV BX, x2          //use x2 as start
    }
    lineok:                     //CX = line length
    asm{
        INC CX                  //if (x2-x1)=1, draw 2 pixels

        PUSH    CX
        MOV AX, y          //base + (y*320) + x-start
        MOV DI, AX
        MOV CX, 2
        SHL DI, CL
        ADD DI, AX
        MOV CX, 6
        SHL DI, CL
        ADD DI, BX          // di <- y*320 + x1
        POP CX

        MOV AL, color
        CLD                     //count forward
        REP STOSB          //draw line
        POP ES
    }
}


//
//      SQ_vline() draws a horizontal line from "y1" to "y2" at
//      x-coord "x" in the specified color.
//

void    SQ_vline( int x, int y1, int y2, uchar color )
{
    asm{
        PUSH    ES
        MOV AX, 0A000H      //segment of screen
        MOV ES, AX
        MOV SI, 320     //distance ro next scan line

        MOV BX, y2
        MOV CX, y1
        SUB CX, BX          //x2-x1 = length
        JNS vlineok         //is x2 > x1 ?

        NEG CX              //no, fix length
        MOV BX, y1      //use x2 as start
    }
    vlineok:            //CX = line length
    asm{
        INC CX      //if (y2-y1)=1, draw 2 pixels

        PUSH    CX
        MOV AX, BX          //base + (y*320) + x-start
        MOV DI, AX
        MOV CX, 2
        SHL DI, CL
        ADD DI, AX
        MOV CX, 6
        SHL DI, CL
        ADD DI, x           // di <- y*320 + x1
        POP CX

        MOV AL, color
    }
    lnLP1:
    asm{
        MOV ES:[DI], AL //store 1 byte
        ADD DI, SI          //move to next scan line
        LOOP    lnLP1           //do it CX number of lines
        POP ES
    }
}

void
ClipBox( int x, int y, int xs, int ys, struct VIEWPORT *vp, uchar color )
{
	int x1, y1;
	boolean leftClip = boolean::FALSE;
	boolean rightClip = boolean::FALSE;
	boolean topClip = boolean::FALSE;
	boolean bottomClip = boolean::FALSE;

	x1 = x + xs;
	y1 = y + ys;
	if ( leftClip = (x < vp->left) )
		 x= vp->left;

	if ( rightClip = ( x1 > vp->right) )
		x1 = vp->right;

	if ( topClip = ( y < vp->top) )
		y = vp->top;

	if ( bottomClip = ( y1 > vp->bottom) )
		y1 = vp->bottom;


    if (!topClip)
		SQ_hline( x,  x1, y,  color );

	if (!bottomClip)
		SQ_hline( x,  x1, y1, color );

	if (!leftClip)
    	SQ_vline( x,   y, y1, color );

	if (!rightClip)
    	SQ_vline( x1,  y, y1, color );
}

void SQ_hxline( int x1, int x2, int y )
{
    asm{
        PUSH    ES
        MOV AX, 0A000H      //segment of screen
        MOV ES, AX

        MOV BX, x1
        MOV CX, x2
        SUB CX, BX         //x2-x1 = length
        JNS lineok         //is x2 > x1 ?

        NEG CX             //no, fix length
        MOV BX, x2          //use x2 as start
    }
    lineok:                     //CX = line length
    asm{
        INC CX                  //if (x2-x1)=1, draw 2 pixels

        PUSH    CX
        MOV AX, y          //base + (y*320) + x-start
        MOV DI, AX
        MOV CX, 2
        SHL DI, CL
        ADD DI, AX
        MOV CX, 6
        SHL DI, CL
        ADD DI, BX          // di <- y*320 + x1
        POP CX
		}
	lnLP1:
    asm{
		MOV AL, ES:[DI]
		XOR AL,0FFH
        MOV ES:[DI], AL //store 1 byte
		INC DI
        LOOP    lnLP1           //do it CX number of lines
        POP ES
    }
}

// vertical xor line

void    SQ_vxline( int x, int y1, int y2)
{
    asm{
        PUSH    ES
        MOV AX, 0A000H      //segment of screen
        MOV ES, AX
        MOV SI, 320     //distance ro next scan line

        MOV BX, y2
        MOV CX, y1
        SUB CX, BX          //x2-x1 = length
        JNS vlineok         //is x2 > x1 ?

        NEG CX              //no, fix length
        MOV BX, y1      //use x2 as start
    }
    vlineok:            //CX = line length
    asm{
        INC CX      //if (y2-y1)=1, draw 2 pixels

        PUSH    CX
        MOV AX, BX          //base + (y*320) + x-start
        MOV DI, AX
        MOV CX, 2
        SHL DI, CL
        ADD DI, AX
        MOV CX, 6
        SHL DI, CL
        ADD DI, x           // di <- y*320 + x1
        POP CX

    }
    lnLP1:
    asm{
		MOV AL, ES:[DI]
		XOR AL,0FFH
        MOV ES:[DI], AL //store 1 byte
        ADD DI, SI          //move to next scan line
        LOOP    lnLP1           //do it CX number of lines
        POP ES
    }
}
void
ClipXorBox( int x, int y, int xs, int ys, struct VIEWPORT *vp )
{
	int x1, y1;
	boolean leftClip = boolean::FALSE;
	boolean rightClip = boolean::FALSE;
	boolean topClip = boolean::FALSE;
	boolean bottomClip = boolean::FALSE;

	x1 = x + xs;
	y1 = y + ys;
	if ( leftClip = (x < vp->left) )
		 x= vp->left;

	if ( rightClip = ( x1 > vp->right) )
		x1 = vp->right;

	if ( topClip = ( y < vp->top) )
		y = vp->top;

	if ( bottomClip = ( y1 > vp->bottom) )
		y1 = vp->bottom;


    if (!topClip)
		SQ_hxline( x,  x1, y );

	if (!bottomClip)
		SQ_hxline( x,  x1, y1 );

	if (!leftClip)
    	SQ_vxline( x,   y, y1 );

	if (!rightClip)
    	SQ_vxline( x1,  y, y1 );
}
