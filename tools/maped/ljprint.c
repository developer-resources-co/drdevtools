
#include <dos.h>
#include <process.h>
#include <stdio.h>

#include "\gfxtools\lib\global.h"
#include "guifunc.h"
#include "\gfxtools\lib\Gui.h"
#include "guiinit.h"
#include "guidefs.h"
#include "\gfxtools\lib\general.h"
#include "mapinpt.h"

#define FILL_CROSSHATCH	3
#define FILL_SHADING		2

#define ESC 			0x1B
#define FORM_FEED		0x0C

static FILE *lj;

void
	position_cursor( int x, int y )
	{
	fprintf( lj, "%c*p%dx%dY", ESC, x, y );
	}

void
	rect_width( int width )
	{
	fprintf( lj, "%c*c%dA", ESC, width );
	}

void
	rect_height( int height )
	{
	fprintf( lj, "%c*c%dB", ESC, height );
	}

void
	pattern( int selection )
	{
	fprintf( lj, "%c*c%dG", ESC, selection );
	}
#define shading(x) pattern(x)

void
	fill_type( int type )
	{
	fprintf( lj, "%c*c%dP", ESC, type );
	}

#define SCR_WIDTH	32

#define PIXELS_HEIGHT (3000)
#define PIXELS_WIDTH	(2250)
#define TILEW	(8)
#define TILEH	(8)
extern int vga_pal[];

void 
	printscreen()
	{
	int i,j,k,l,w;
	int page,x,y,pix;
	unsigned char v;
	int wdth = mapPtr->xSize;

	int x1,y1;
	int nTilePixelWidth, nTilePixelHeight;
	int xPrint, yPrint;
	int rgb;
	unsigned huge char *tile;

	Message( "Printing...", DONT_WAIT, DIMGRAY, MEDGRAY );

	fp = stdprn;
	wdth = 32;

	nTilePixelWidth = 70;
	nTilePixelHeight = 70;

	fprintf( fp, "%cE", ESC );			// reset printer

	for ( page=0; page<wdth; page+=SCR_WIDTH )
		{
		for ( yPrint=0, y=0; y<25; ++y, yPrint+=nTilePixelHeight )
			{
			for ( xPrint=0, x=0; x<SCR_WIDTH; ++x, xPrint-=nTilePixelWidth )
				{
				tile = *(mapPtr->buffer + y*mapPtr->xSize + x);

				for ( y1=0; y1<TILEH; ++y1 )
					{
					for ( x1=0; x1<TILEW; ++x1 )
						{
						int rgb, r, g, b;

						rgb = vga_pal[ *(tile + 8*y1 + x) ];
						r = (rgb & 0x0F00) >> 16;
						g = (rgb & 0x00F0) >> 8;
						b = (rgb & 0x000F);

						position_cursor( xPrint, yPrint );
						rect_width( nTilePixelWidth );
						rect_height( nTilePixelHeight );
						shading( (r*30 + g*59 + b*11) / 63 );		// 0--100
						fill_type( 2 );
						}
					}
				}
			}
		fputc( FORM_FEED, fp );
		}


#if 0
	for (i=0;i<wdth;i+=32)
		{
		for (j=0;j<32;j++)
			{
			fprintf(fp,"%02i,%02i           ",i>>5,j);
			fputc(0x1B,fp);
			fputc(0x4B,fp);
			fputc( 200+24*2, fp );
			fputc(0x00,fp); 	// grafix mode set

			for (k=24*wdth;k>=0;k-=wdth)
				{
				tile = *(mapPtr->buffer + i + k + j);
				tile &= CHARMASK;

				for ( l=7; l>=0; --l )
					{
					for ( v=0, w=0; w<8; ++w, v<<=1 )
						{
						v |= *( chardata + tile*64 + w ) ? 0 : 1;
						}
					fputc(v,fp);
//					v ^= 0xFF;
					}
				fputc(0x00,fp);
				fputc(0x00,fp);
				}
			fputc(0x0D,fp);
			fputc(0x0A,fp);
			}

		if (i&32)
			fputc(0x0C,fp);     // FormFeed
		else
			{
			fputc(0x0D,fp);
			fputc(0x0A,fp);
			}
		}
	if (i&32)
		fputc(0x0C,fp);	  // FormFeed
#endif

//	fclose( fp );
	}


void
	printout()
	{
#if 0
	int op;

	strcpy(oname,sname);
	if ((setfilename(sname)!=13)||(sname[0]==0))
		{
		strcpy(sname,oname);
		resetall();
		return;
		}
	op=pstm;
	mouseptr(waiting);
	strcpy(&dos1[strlen(dos1)-3],"PRN");
	printscreen(dos1);
	switchscn(op);
	strcpy(sname,oname);
	resetall();
#endif

	printscreen();
	}
