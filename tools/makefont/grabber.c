//==============================================================================
// makefont.c: Convert LBM to font file by William B. Norris IV
// based on code by Lars Norphcen
//==============================================================================

#include <mem.h>
#include <dos.h>
#include <stdio.h>
#include <alloc.h>
#include "grabber.h"
#include "ilbmread.h"
#include "\gfxtools\lib\general.h"
#include "\gfxtools\lib\gfx.h"
#include "grabinpt.h"

extern FLAG displayOn;

//==============================================================================

extern int GetPixel(unsigned long x,unsigned long y);

//==============================================================================

extern height,pixelWidth;

const int charSize = 6;
const uchar numXChars = 16;
const uchar numYChars = 8;

main (int argc,char *argv[])
{
	int x,y;
	FILE *fp;

	GetInput (argc,argv);
	if(displayOn)
		SetGfxMode(0x13);
	else
		printf ("Reading ILBM...\r\n");
	ReadILBM (IFFName);

	if ( fp = fopen( OutputFile, "wb" ) )
		{
		static uchar zero = 0;
		uchar numChars = numXChars * numYChars;

		fwrite( &charSize, sizeof(uchar), 1, fp );
		fwrite( &charSize, sizeof(uchar), 1, fp );
		fwrite( &numChars, sizeof(numChars), 1, fp );
		fwrite( &zero, sizeof(zero), 256, fp );

		for ( y=0; y<numYChars*charSize; y+=charSize )
			for ( x=0; x<numXChars*charSize; x+=charSize )
				{
				int p,l;
				unsigned char temp = 0;

				for ( l=0; l<charSize; ++l )
					{
					for ( p=0; p<charSize; ++p )
						{
						temp = GetPixel( x+p, y+l );
						fwrite( &temp, sizeof(temp), 1, fp );
						}
					}
				}
		fclose( fp );
		}
	else
		fprintf( "Error opening output file %s\n", OutputFile );

	if(displayOn)
    	SetGfxMode(3);
	printf( "Done...\n" );
	Quit();
}
