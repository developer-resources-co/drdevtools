
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>

#include <stdlib.h>
#include <alloc.h>
#include <assert.h>
#include <mem.h>
//#include <conio.h>							// for kbhit()

#include <pclib/general.hpp>
#include <pclib/iff.hpp>
#include <pclib/iffilbm.hpp>
#include <pclib/iffpbm.hpp>
#include <pclib/iffanim.hpp>
#include <pclib/error.hpp>
#include <pclib/mem.hpp>
#include <pclib/grphport.hpp>
//#include <pclib/mono.hpp>

#include "abmsplit.h"
#include "cloption.h"

unsigned char far* image[1000];
unsigned char far* output_image[1000];

ilbmCMAP myCMAP;
ilbmBMHD myBMHD;
ilbmCAMG myCAMG;
pbmBODY mypbmBODY( &myBMHD );
ilbmBODY myilbmBODY( &myBMHD );
animANHD myANHD;
animDLTA myDLTA( &myANHD );

//omonostream mono;

// One per file, even in ANIM's
errorcode
HandleCMAP( iffRead* readPtr )
	{
	myCMAP.Read(readPtr);

	colorPalette pal;
	for ( int i=0; i<myCMAP.GetNumColors(); ++i )
		{
		pal.Create( color(
			myCMAP.Red(i), myCMAP.Green(i), myCMAP.Blue(i) ) );
		}
	display->Palette( pal );

	display->SetColorIndex( 240 );
	display->Erase();

	return( NOERR );
	}

//=============================================================================

errorcode
HandleBMHD( iffRead* readPtr )
	{
	myBMHD.Read( readPtr );

	// Allocate memory for entire image
	for ( int i=0; i<myBMHD.h; ++i )
		{
		image[i] = (unsigned char*)malloc( myBMHD.w );
		assert( image[i] );
		}

	// Allocate the output buffer
	if ( bClipLbmSize )
		{
		xLbmSize = myBMHD.w;
		yLbmSize = myBMHD.h;
		}
	else
		{
		xLbmSize = xLbmSize < myBMHD.w ? myBMHD.w : xLbmSize;
		yLbmSize = yLbmSize < myBMHD.h ? myBMHD.h : yLbmSize;
		}

#if 0
	for ( i=0; i<yLbmSize; ++i )
		{
		output_image[i] = malloc( xLbmSize );
		assert( output_image[i] );
		}
#endif

	return( NOERR );
	}

//=============================================================================

errorcode
HandleCAMG( iffRead* readPtr )
	{
	myCAMG.Read( readPtr );
	return( NOERR );
	}

//==============================================================================

static int nParseLine;

errorcode ParseLine( ubyte* line )
	{
	memcpy( image[nParseLine], line, myBMHD.w );

	if ( nParseLine < 200 )
		{
		for ( int x=0; x<320 && x<myBMHD.w; ++x )
			{
			int i = oldGetPixel( x, nParseLine );
			display->SetColorIndex( i );
			display->PutPixel(point(x,nParseLine));
			}
		}

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

int xOutputPos = 0;
int yOutputPos = 0;

errorcode
	HandleANHD( iffRead* readPtr )
	{
	myANHD.Read( readPtr );

//	cout << myANHD;

	myANHD.x += xOutputPos;
	myANHD.y += yOutputPos;

	return( NOERR );
	}


errorcode NullRoutine( ubyte* line )
	{
	return( NOERR );
	}

errorcode
	HandleDLTA( iffRead* readPtr )
	{
	nParseLine = 0;
	myDLTA.Read( readPtr, &NullRoutine );

	return( NOERR );
	}


//=============================================================================

extern void WritePict( char* );

void
ReadANIM( const char *szAnimName, char* szPictName )
	{
	boolean bFirstFrame = boolean::TRUE;
	ifstream input( szAnimName, ios::in|ios::binary );

	if ( input )
		{
		iffTag checkAnim;
		iffRead *readPtr = NULL;

		ReadIFFTag( input );					// FORM
		ReadIFFLength( input );				// maybe save (?)
		checkAnim = ReadIFFTag( input );

		if ( checkAnim == tagANIM )
			{
			int nFrame = 0;

			for (;;)
				{
				readPtr = new iffRead( input );
				if ( !readPtr->NextForm() )
					break;

				// Copy from previous frame to current frame
				if ( !bMultipleLbm && !bFirstFrame )
					{
					int xOld = xOutputPos;
					int yOld = yOutputPos;

					xOutputPos += myBMHD.w + nSpacing;

					// Check if we're going to go past edge of screen
					if ( xOutputPos+myBMHD.w + nSpacing >= xLbmSize )
						{
						xOutputPos = 0;
						yOutputPos += myBMHD.h + nSpacing;
						}

					//CopyBitmap( src,x,y,w,h, dest,x,y );

					for ( int y=0; y<myBMHD.h; ++y )
						{
						for ( int x=0; x<myBMHD.w; ++x )
							{
							color col;

							col = display->GetPixel( point(xOld+x, yOld+y) );
							display->SetColor( col );
							display->PutPixel( point(
								xOutputPos+x, yOutputPos+y) );
							}
						}
					}
				bFirstFrame = boolean::FALSE;

				if ( readPtr->GetForm() == tagILBM )
					{
					// Real ILBM things
					readPtr->RegisterChunk( tagCMAP, HandleCMAP );
					readPtr->RegisterChunk( tagBMHD, HandleBMHD );
					readPtr->RegisterChunk( tagCAMG, HandleCAMG );
					readPtr->RegisterChunk( tagBODY, HandleBODY );

					// ANIM things
					readPtr->RegisterChunk( tagANHD, HandleANHD );
					readPtr->RegisterChunk( tagDLTA, HandleDLTA );

					readPtr->ParseChunks();
					}
				if ( readPtr->GetForm() == tagPBM )
					{
					// Real ILBM things
					readPtr->RegisterChunk(tagCMAP,HandleCMAP);
					readPtr->RegisterChunk(tagBMHD,HandleBMHD);
					readPtr->RegisterChunk(tagCAMG,HandleCAMG);
					readPtr->RegisterChunk(tagBODY,HandlePBMBODY);

					// ANIM things
					readPtr->RegisterChunk( tagANHD, HandleANHD );
					readPtr->RegisterChunk( tagDLTA, HandleDLTA );

					readPtr->ParseChunks();
					}

				if ( bMultipleLbm )
					{
					char szFrameName[_MAX_PATH];

					sprintf( szFrameName, szPictName, nFrame++ );
					WritePict( szFrameName );
					}

				delete readPtr;
				}

			if ( !bMultipleLbm )
				{
				WritePict( szPictName );
				}

			delete readPtr;
			}
		}
	else
		cout << "Error: file \"" << szAnimName << "\" not found\n";
	}

