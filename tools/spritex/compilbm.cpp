
#include <pclib/general.h>
#include <pclib/iff.h>
#include <pclib/grphport.h>

#include "sprite.hpp"
#include "spmain.hpp"

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>

#include <stdlib.h>
#include <alloc.h>
#include <assert.h>
#include <mem.h>
//#include <conio.h>							// for kbhit()

#include <pclib/general.h>
#include <pclib/iff.h>
#include <pclib/iffilbm.h>
#include <pclib/iffpbm.h>
#include <pclib/iffanim.h>
#include <pclib/error.h>
#include <pclib/mem.h>
#include <pclib/grphport.h>
#include <pclib/mono.h>

//unsigned char far* image[1000];
//unsigned char far* output_image[1000];

//ilbmCMAP myCMAP;
//ilbmBMHD myBMHD;
//ilbmCAMG myCAMG;

pbmBODY mypbmBODY( &myPicture.bmhd );
ilbmBODY myilbmBODY( &myPicture.bmhd );
//animANHD myANHD;
//animDLTA myDLTA( &myANHD );

omonostream mono;

// One per file, even in ANIM's
errorcode
HandleCMAP( iffRead* readPtr )
	{
	myPicture.cmap.Read(readPtr);

	colorPalette pal;
	for ( int i=0; i<myPicture.cmap.GetNumColors(); ++i )
		{
		pal.Create( color(
			myPicture.cmap.Red(i), myPicture.cmap.Green(i), myPicture.cmap.Blue(i) ) );
		}
	display->Palette( pal );


	return( NOERR );
	}

//=============================================================================

errorcode
HandleBMHD( iffRead* readPtr )
	{
	myPicture.bmhd.Read( readPtr );

	graphic.sizeX = myPicture.bmhd.w*8;
	graphic.sizeY = myPicture.bmhd.h;

	// Allocate memory for entire image
	for ( int i=0; i<myPicture.bmhd.h; ++i )
		{
		myPicture.image[i] = SafeFarMalloc( myPicture.bmhd.w );
		assert( myPicture.image[i] );
		}

	return( NOERR );
	}

//=============================================================================

errorcode
HandleCAMG( iffRead* readPtr )
	{
	myPicture.camg.Read( readPtr );
	return( NOERR );
	}

//==============================================================================

errorcode NullRoutine( ubyte* line )
	{
	return( NOERR );
	}

//=============================================================================

static int nParseLine;

errorcode ParseLine( ubyte* line )
	{
	memcpy( myPicture.image[nParseLine], line, myPicture.bmhd.w );

/*
	if ( nParseLine < 200 )
		{
		for ( int x=0; x<320 && x<myPicture.bmhd.w; ++x )
			{
			int i = oldGetPixel( myPicture.image, x, nParseLine );
			display->SetColorIndex( i );
			display->PutPixel( point(x,nParseLine) );
			}
		}
*/
	++nParseLine;

	return( NOERR );
	}

errorcode
	HandleBODY( iffRead* readPtr )
	{
	nParseLine = 0;
	myilbmBODY.Read(readPtr,&ParseLine);
	return( 0 );
	}

errorcode
	HandlePBMBODY( iffRead* readPtr )
	{
	nParseLine = 0;
	mypbmBODY.Read(readPtr,&ParseLine);
	return( NOERR );
	}

//=============================================================================

void
ReadPict( const char *szPictName )
	{
	boolean bFirstFrame = boolean::TRUE;
	ifstream input( szPictName, ios::in|ios::binary );

	if ( input )
		{
		iffRead *readPtr = new iffRead(input);

/*
		graphic.flag = boolean::FALSE;
		graphic.offsetX =
		graphic.offsetY =
		graphic.sizeX =
		graphic.sizeY =
		graphic.upLeftX =
		graphic.upLeftY =
		graphic.lowRightX =
		graphic.lowRightY = 0;
		graphic.midFlag = boolean::FALSE;
*/

		readPtr->NextForm();
		if ( readPtr->GetForm() == tagILBM )
			{
			readPtr->RegisterChunk(tagCMAP,HandleCMAP);
			readPtr->RegisterChunk(tagBMHD,HandleBMHD);
			readPtr->RegisterChunk(tagCAMG,HandleCAMG);
			readPtr->RegisterChunk(tagBODY,HandleBODY);
			readPtr->ParseChunks();
			}
		else if ( readPtr->GetForm() == tagPBM )
			{
			readPtr->RegisterChunk(tagCMAP,HandleCMAP);
			readPtr->RegisterChunk(tagBMHD,HandleBMHD);
			readPtr->RegisterChunk(tagCAMG,HandleCAMG);
			readPtr->RegisterChunk(tagBODY,HandlePBMBODY);
			readPtr->ParseChunks();
			}
		delete readPtr;
		}
	else
		cout << "Error: file \"" << szPictName << "\" not found\n";
	}
