
//=============================================================================
// c library includes

#include <stdio.h>
#include <alloc.h>
#include <conio.h>
#include <mem.h>
#include <stdlib.h>
#include <ctype.h>
#include <bios.h>
#include <dir.h>

//=============================================================================
// DR library includes

#include "\gfxtools\lib\gfx.h"
#include "\gfxtools\lib\color.h"

#include "\gfxtools\lib\keys.h"
#include "\gfxtools\lib\global.h"
//#include "\gfxtools\lib\input.h"
#include "\gfxtools\lib\general.h"
#include "\gfxtools\lib\mouse.h"
#include "\gfxtools\lib\gui.h"

//=============================================================================
// maped includes

#include "guifunc.h"

#include "iff.h"
#include "ilbm.h"
//#include	"\lib\lbmpriv.h"


#define	WriteByteIFF(b,fp) fputc(b,fp)

#define	ID_ILBM	MakeID('I','L','B','M')
#define	ID_CMAP	MakeID('C','M','A','P')
#define	ID_BMHD	MakeID('B','M','H','D')
#define	ID_BODY	MakeID('B','O','D','Y')

extern RGBcolor vga_pal[];			// BAD

int
	WriteCMAP(FILE * fo)
	{
	int color;

	for (color=0; color < 16; color++)
		{
		WriteByteIFF( (vga_pal[charpaloff+color].r) << 2, fo );
		WriteByteIFF( (vga_pal[charpaloff+color].g) << 2, fo );
		WriteByteIFF( (vga_pal[charpaloff+color].b) << 2, fo );
		}
	return 0;
	}

int WriteBMHD(FILE * fo) {
	WriteWordIFF(mapPtr->xSize*8,fo);	// w
	WriteWordIFF(mapPtr->ySize*8,fo);		// h

	WriteWordIFF(0,fo);	// x
	WriteWordIFF(0,fo);	// y

	WriteByteIFF(4,fo);	// 4 bit planes

	WriteByteIFF(0,fo);	// no masking
	WriteByteIFF(cmpNone,fo);	// compression

	WriteByteIFF(0,fo);	// pad byte

	WriteWordIFF(0,fo);	// transparent

	WriteByteIFF(0,fo);	// Xaspect
	WriteByteIFF(0,fo);	// yaspect
	WriteWordIFF(0,fo);	// pgW
	WriteWordIFF(0,fo);	// pgH
	return 0;
}

int
	WriteBODY(FILE *fo)
	{
	int xx,yy,tw,bpl,pix;

	for (yy = 0; yy < mapPtr->ySize*8; yy++)
		{
		for (bpl = 0; bpl < 4; bpl++)
			{
			for (xx = 0; xx < (mapPtr->xSize*8+15)/16; xx++)
				{
				// get enough tiles to fit into 16 pixels
				int data, tile1, tile2;
				int tile1Pal, tile2Pal, tilePal;
				uchar *tile1Gfx, *tile2Gfx, *tileGfx;

				data = *(mapPtr->buffer + (yy/8*mapPtr->xSize) + xx*(16/8) );
				tile1 = data & CHARMASK;
				if ( data & HFLIPMASK ) tile1 += chars;
				if ( data & VFLIPMASK ) tile1 += chars*2;
				tile1Pal = (data & PALMASK) >> PALBIT;
				tile1Gfx = &( chardata[ (ulong)tile1<<6 ] );


				data = *(mapPtr->buffer + (yy/8*mapPtr->xSize) + xx*(16/8) + 1);
				tile2 = data & CHARMASK;
				if ( data & HFLIPMASK ) tile2 += chars;
				if ( data & VFLIPMASK ) tile2 += chars*2;
				tile2Pal = (data & PALMASK) >> PALBIT;
				tile2Gfx = &( chardata[ (ulong)tile2<<6 ] );

				tileGfx = tile1Gfx;
				tilePal = tile1Pal;
				for (pix = 0; pix < 16; pix++)
					{
					if ( pix >= 8 ) tileGfx = tile2Gfx, tilePal = tile2Pal;
					tw <<= 1;
					tw |= (
						    (
//							  (*MCGA_PIX((16*xx)+pix,yy)) >> bpl
								( *( tileGfx + ((yy%8)*8) + (pix%8) ) + tilePal )
							>> bpl ) &1
							);
					}
				WriteWordIFF(tw,fo);
				}
			}
		}
	return( 0 );
	}


int PictWriter(FILE * fo) {
	int x;

	x = CreateChunk(fo,ID_CMAP,WriteCMAP,-1L,NULL,0);
	if (x) return x;

	x = CreateChunk(fo,ID_BMHD,WriteBMHD,-1L,NULL,0);
	if (x) return x;

	x = CreateChunk(fo,ID_BODY,WriteBODY,-1L,NULL,0);
	if (x) return x;

	x = ferror(fo);
	if (x) return x;

	return	0;
}

void
	AWritePic(char * thingname)
	{
	FILE * fp;
	char szDrive[ MAXDRIVE ];
	char szDir[ MAXDIR ];
	char szFile[ MAXFILE ];
	char szFilename[ MAXPATH ];

	char tempstr[256];

	fnsplit( thingname, szDrive, szDir, szFile, NULL );		// strip extension
	fnmerge( szFilename, szDrive, szDir, szFile, ".lbm" );

	fp = fopen( szFilename,"wb");
	if ( fp )
		{
		sprintf( tempstr, "Saving file \"%s\"", szFilename );
		Message( tempstr, DONT_WAIT, DIMGRAY, MEDGRAY );

		WriteFileIFF( fp, ID_ILBM, PictWriter );
		fclose( fp );
		}
	else
		{
		char tempstr[256];

		sprintf( tempstr, "Couldn't open file \"%s\"", szFilename );
		Message( tempstr, WAIT_AND_EAT, DIMGRAY, MEDGRAY );
		}
	}




void
	WritePic()
	{
	AWritePic( mapPtr->szFilename );
	}

