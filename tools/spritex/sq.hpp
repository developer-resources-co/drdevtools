//
//      SQ.hpp -- prototypes of public graphic routines for BUGSQUAD
//
//      Russell Bornsch+
//      January, 1991
//

#ifndef SQ_H
#define SQ_H

#include "utypes.hpp"



#define VGA_HOME SQ_get_screen_address(0,0)
#define CLEARSCREEN SQ_fill(VGA_HOME,64000,0)

//  an 18-bit or 24-bit color.

typedef struct _rgbcolor
{
    uchar r;                        /* herein 0-63 */
    uchar g;                        /* herein 0-63 */
    uchar b;                        /* herein 0-63 */
} RGBcolor;

// an HSV color

#define NO_HUE  255

typedef struct _hsvcolor
{
    uchar h;                        /* herein 0-179, 0=r, 60=g, 120=b, 255=none */
    uchar s;                        /* herein 0-63 */
    uchar v;                        /* herein 0-63 */
} HSVcolor;

#define COL_FIELD       255 // light grey
#define COL_TEXT        254 // black
#define COL_SELECT  253 // white
#define COL_LOCK        252 // red

void     SQ_set_mode( int mode_num );
void     SQ_set_RGB_color( int entry, RGBcolor far *rgb );
void     SQ_get_RGB_color( int entry, RGBcolor far *rgb );
void     SQ_set_palette( int start, int count, RGBcolor far *rgb );
void     SQ_get_palette( int start, int count, RGBcolor far *rgb );
void     SQ_put_pixel( int x, int y, uchar color );
void     SQ_XOR_pixel( int x, int y );
uchar    SQ_get_pixel( int x, int y );
void     SQ_copy_forward_lines( char far *s, int topline, unsigned int count );
void     SQ_copy_forward( char far *s );
char far *SQ_get_screen_address( int x, int y );
void     SQ_get_xy_address( char far *addr, int *x, int *y );

#define SQ_S_TO_S_OPAQUE    0x00        /* blit                     */
#define SQ_S_TO_B_OPAQUE    0x01        /* save_screen      */
#define SQ_B_TO_S_OPAQUE    0x02        /* restore_screen       */
#define SQ_B_TO_B_OPAQUE    0x03        /* buf_to_buf_opaque    */

#define SQ_S_TO_S_TRANS 0x04        /* scrn_to_scrn     */
#define SQ_B_TO_S_TRANS 0x06        /* buf_to_scrn          */
#define SQ_B_TO_B_TRANS 0x07        /* buf_to_buf           */

void    SQ_copy( char far *s, char far *d, ushort w, ushort h, int copytype );
void    SQ_XOR_Box( char far *d, ushort w, ushort h );
void    SQ_hline( int x1, int x2, int y, uchar color );
void    SQ_vline( int x, int y1, int y2, uchar color );
void    SQ_fill( char far *d, ushort length, uchar value );
void    SQ_solid_box( int x1, int y1, int x2, int y2, uchar color );
void    SQ_hollow_box( int x, int y, int x1, int y1, uchar color );
void    SQ_set_hi( void );
void	ClipBox( int x, int y, int xs, int ys, struct VIEWPORT *, uchar color );

void
ClipXorBox( int x, int y, int xs, int ys, struct VIEWPORT *vp );


#endif
