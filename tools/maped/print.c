
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

#define ESC 			0x1B
#define FORM_FEED		0x0C

static FILE *fp;

#define SCR_WIDTH	32

#define PIXELS_HEIGHT (3000)
#define PIXELS_WIDTH	(2250)
#define TILEW	(8)
#define TILEH	(8)
extern int vga_pal[];

void 
	printscreen()
	{
	int page,x,y,l,w;
	unsigned char v;
	int width = mapPtr->xSize;

	int rgb;
	int tile;

	Message( "Printing...", DONT_WAIT, DIMGRAY, MEDGRAY );

	fp = fopen( "test.prn", "wb" );
	width = 64;

	fputc(0x1B,fp);
	fputc(0x40,fp);    // init printer
	fputc(0x1B,fp);
	fputc(0x41,fp);
	fputc(0x0A,fp);    // 10/72 line space

	for ( page=0; page<width/32; ++page )
		{
		for ( y=0; y<32; ++y )
			{
			fprintf(fp,"%02d:%02d           ",page,y);
			for ( x=0; x<32; ++x )
				{
				int nBytes;
			
				tile = *( mapPtr->buffer + (page*32 + x) + mapPtr->xSize*y );
				tile &= CHARMASK;
	
				fputc( 0x1B, fp );
				fputc( 0x4B, fp );
				nBytes = 8+2;
				fputc( nBytes%256, fp );
				fputc( nBytes/256, fp );
	
				for ( l=0; l<8; ++l )
					{
					int c;
			
					for ( v=0, w=0; w<8; ++w )
						{
						v <<= 1;
						c = *(chardata + tile*64 + w*8 + l);
						v |= c ? 0 : 1;
						}
					fputc( v, fp );
					}

				fputc(0x00,fp);
				fputc(0x00,fp);
				}

			fputc(0x0D,fp);
			fputc(0x0A,fp);
			}
  		fputc(0x0C,fp);     // FormFeed
		}
	fclose( fp );
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
