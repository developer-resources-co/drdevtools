
#include <stdio.h>

#include <pclib/general.hpp>
#include <pclib/fileio.hpp>

#include "grabber.hpp"
#include "cloption.hpp"
#include "chario.hpp"

extern  int chars;
extern  uchar huge *data[];
extern  uint map[MAXGRID_Y][MAXGRID_X];
extern  int xCount,yCount;
extern int nLimitChars;

//==============================================================================

void
	WriteChars( char *OutputFile )
	{
	FILE *fp;

	if ( fp = fopen( OutputFile, "wb" ) )
		{
		for ( int i=0; i<chars; ++i )
			fwrite( data[i], 1, charSize, fp );
		fclose( fp );
		}
	else
		Error( 10, "Cannot open output file \"%s\"\n", OutputFile );
	}

//==============================================================================

extern boolean bGenesis;
// !!! THIS SHOULD BE PART OF THE TARGET-SYSTEM CLASS AND SHOULD BE BASED
// !!! ON BYTE-ORDER SWAP, NOT SYSTEM
void WriteInt(int data,FILE *fp)
	{
	if ( bGenesis )
		putc(data >> 8,fp), putc(data & 0xFF,fp);
	else
		putc(data & 0xFF,fp), putc(data >> 8,fp);
	}

//==============================================================================

void
	OpenExistingCharFile( char *charFile )
	{
	FILE *fp;

	if ( fp=fopen(charFile, "rb") )
		{
//		if(!displayOn) printf ("Reading char file...\n");
		chars = ffilesize( fp ) / charSize;
		if (chars>nLimitChars) Error ( 10, "Too many chars!");
//		if (chars>maxChars) Error ( 10, "Too many chars!");
		for ( int a=0; a<chars; ++a )
			fread (data[a],1,charSize,fp);   // load existing chars
		fclose (fp);
		}
	}

//==============================================================================

extern int bgpri;

void WriteMap (char *MapFile)
	{
	FILE *fp;

	fp = fopen (MapFile,"wb");
	if (fp==NULL) Error ( 10, "Cannot open map output file..");

	WriteInt(xCount,fp);
	WriteInt(yCount,fp);

	for ( int a=0; a<yCount; ++a )
   		for ( int b=0; b<xCount; ++b )
			{
			if ( bgpri )
				map[a][b] |= PRIBITMASK;
			WriteInt( map[a][b], fp );
			}
	fclose (fp);
	}

//==============================================================================
