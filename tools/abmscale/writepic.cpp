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

#include <pclib/general.hpp>
#include <pclib/math.hpp>					// should be fixed.hpp
#include <pclib/iff.hpp>
#include <pclib/iffilbm.hpp>
#include <pclib/iffpbm.hpp>
#include <pclib/iffanim.hpp>

#include "anmsplit.hpp"
#include "cloption.hpp"

#define	WriteByteIFF(b,fp) fputc(b,fp)

extern ilbmCMAP myCMAP;
extern ilbmBMHD myBMHD;


int
	WriteCMAP(FILE * fo)
	{
	int color;

	for (color=0; color < 1<<myBMHD.nPlanes; ++color )
		{
		WriteByteIFF( myCMAP.Red( color ), fo );
		WriteByteIFF( myCMAP.Green( color ), fo );
		WriteByteIFF( myCMAP.Blue( color ), fo );
		}
	return( 0 );
	}

extern int xLbmSize, yLbmSize;
int WriteBMHD(FILE * fo)
	{
	ilbmBMHD animBMHD = myBMHD;

	WriteWordIFF(xLbmSize,fo);		// w
	WriteWordIFF(yLbmSize,fo);		// h

	WriteWordIFF(0,fo);			// x
	WriteWordIFF(0,fo);			// y

	WriteByteIFF(animBMHD.nPlanes,fo);

	WriteByteIFF(ilbmBMHD::mskNone,fo);
	WriteByteIFF(ilbmBMHD::cmpNone,fo);

	WriteByteIFF(0,fo);			// pad byte

	WriteWordIFF(0,fo);			// transparent

	WriteByteIFF(0,fo);			// Xaspect
	WriteByteIFF(0,fo);			// yaspect
	WriteWordIFF(0,fo);			// pgW
	WriteWordIFF(0,fo);			// pgH

	return( 0 );
	}


int
	WriteBODY(FILE *fo)
	{
	int xx,yy,tw,bpl,pix;

	for ( yy=0; yy<yLbmSize; ++yy )
		{
		for (bpl = 0; bpl < myBMHD.nPlanes; bpl++)
			{
			for ( xx=0; xx<xLbmSize/16; ++xx )
				{
				for (pix = 0; pix < 16; pix++)
					{
					tw <<= 1;
					tw |= (
						    (
								display->GetPixelIndex( point( (16*xx)+pix,yy) ) >> bpl
//							  (*MCGA_PIX((16*xx)+pix,yy)) >> bpl
							 ) &1
							);
					}
				WriteWordIFF(tw,fo);
				}
			}
		}

	return( NOERR );
	}


int
	WritePBMBODY(FILE *fo)
	{
	for ( int y=0; y<yLbmSize; ++y )
		{
		for ( int x=0; x<xLbmSize; ++x )
			{
			int col = display->GetPixelIndex( point(x,y) );
			WriteByteIFF( col, fo );
			}
		if ( x & 1 ) WriteByteIFF( 0, fo );			// odd?
		}
	return( NOERR );
	}


int PictWriter(FILE * fo) {
	int x;

	x = CreateChunk(fo,tagBMHD, (IffFuncPtr)WriteBMHD);
	if (x) return x;

	x = CreateChunk(fo,tagCMAP, (IffFuncPtr)WriteCMAP);
	if (x) return x;

	x = CreateChunk(fo,tagBODY, (IffFuncPtr)WriteBODY);
	if (x) return x;

	x = ferror(fo);
	if (x) return x;

	return( NOERR );
}


int PictWriterPBM(FILE * fo) {
	int x;

	x = CreateChunk(fo,tagBMHD, (IffFuncPtr)WriteBMHD);
	if (x) return x;

	x = CreateChunk(fo,tagCMAP, (IffFuncPtr)WriteCMAP);
	if (x) return x;

	x = CreateChunk(fo,tagBODY, (IffFuncPtr)WritePBMBODY);
	if (x) return x;

	x = ferror(fo);
	if (x) return x;

	return( NOERR );
}


void
	WritePict(char* szPictName)
	{
	FILE* fp;

	fp = fopen( szPictName, "wb" );
	if ( fp )
		{
		if ( bPbm )
			WriteFileIFF( fp, tagPBM, (IffFuncPtr)PictWriterPBM );
		else
			WriteFileIFF( fp, tagILBM, (IffFuncPtr)PictWriter );
		fclose( fp );
		}
	else
		{
		printf( "Couldn't open file \"%s\"", szPictName );
		}
	}
