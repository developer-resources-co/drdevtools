
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <mem.h>
//#include <conio.h>							// for kbhit()

#include <pclib/general.hpp>
#include <pclib/math.hpp>						// should be fixed.hpp
#include <pclib/iff.hpp>
#include <pclib/iffilbm.hpp>
#include <pclib/iffpbm.hpp>
#include <pclib/iffanim.hpp>
#include <pclib/error.hpp>
#include <pclib/mem.hpp>
#include <pclib/grphport.hpp>
#include <pclib/filename.hpp>
//#include <pclib/viewport.hpp>

#include "abmscale.hpp"
#include "cloption.hpp"

ubyte far* image[200];

ilbmCMAP myCMAP;
ilbmBMHD myBMHD;
ilbmCAMG myCAMG;
pbmBODY mypbmBODY( &myBMHD );
ilbmBODY myilbmBODY( &myBMHD );
animANHD myANHD;
animDLTA myDLTA( &myANHD );

// One per file, even in ANIM's
errorcode
HandleCMAP( iffRead* readPtr )
	{
	myCMAP.Read( readPtr );

	colorPalette pal;
	for ( int i=0; i<myCMAP.GetNumColors(); ++i )
		{
		pal.Create( color(
			myCMAP.Red(i), myCMAP.Green(i), myCMAP.Blue(i) ) );
		}
	display->Palette( pal );

	display->SetColorIndex( 240 );
	display->Erase();

	return NOERR;
	}

//=============================================================================

errorcode
HandleBMHD( iffRead* readPtr )
	{
	myBMHD.Read( readPtr );

	// Allocate memory for entire image
	for ( int i=0; i<myBMHD.h; ++i )
		{
		image[i] = (ubyte far*)malloc( myBMHD.w );
		assert( image[i] );
		}

	return NOERR;
	}

//=============================================================================

errorcode
HandleCAMG( iffRead* readPtr )
	{
	myCAMG.Read( readPtr );
	return NOERR;
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

	return NOERR;
	}

errorcode
	HandleBODY( iffRead* readPtr )
	{
	nParseLine = 0;
	myilbmBODY.Read( readPtr, &ParseLine );
	return 0;
	}

errorcode
	HandlePBMBODY( iffRead* readPtr )
	{
	nParseLine = 0;
	mypbmBODY.Read( readPtr, &ParseLine );
	return NOERR;
	}

//=============================================================================

int xOutputPos = 0;
int yOutputPos = 0;

errorcode
	HandleANHD( iffRead* readPtr )
	{
	myANHD.Read( readPtr );

	myANHD.x += xOutputPos;
	myANHD.y += yOutputPos;

	return NOERR;
	}


errorcode NullRoutine( ubyte* line )
	{
	return NOERR;
	}

errorcode
	HandleDLTA( iffRead* readPtr )
	{
	nParseLine = 0;
	myDLTA.Read( readPtr, &NullRoutine );

	return NOERR;
	}


//=============================================================================

void
ReadANIM( const char *szAnimName, char* szOutputName )
	{
	ifstream input( szAnimName, ios::in|ios::binary );
	ofstream output( szOutputName, ios::out|ios::binary );
	iffWrite* writePtr;

	if ( input )
		{
		iffTag checkAnim;
		iffRead* readPtr = NULL;

		ReadIFFTag( input );					// FORM
		ReadIFFLength( input );				// maybe save (?)
		checkAnim = ReadIFFTag( input );

		if ( checkAnim == tagANIM )
			{
			int nFrame = 0;
			rastGraphPort* rpDelta = NULL;

			int xOfs = 160;
			int yOfs = 0;

 			writePtr = new iffWrite( output );
			writePtr->NewForm( tagANIM );

			for (;;)
				{
				// Setup for output

				if ( nFrame > 1 )
					{ // Copy current frame to "delta" buffer
					for ( int y=0; y<rpDelta->GetHeight(); ++y )
						{
						for ( int x=0; x<rpDelta->GetWidth(); ++x )
							{
							int i = display->GetPixelIndex( point( xOfs+x, yOfs+y ) );
							rpDelta->SetColorIndex( i );
							rpDelta->PutPixel( point( x, y ) );
							}
						}
					}

				// Process
				readPtr = new iffRead( input );
				if ( !readPtr->NextForm() )
					break;

				if ( readPtr->GetForm() == tagILBM )
					readPtr->RegisterChunk( tagBODY, HandleBODY );
				else if ( readPtr->GetForm() == tagPBM )
					readPtr->RegisterChunk(tagBODY,HandlePBMBODY);
				else
					Error( 10, "Undefined ANIM chunk" );

				// Real ILBM things
				readPtr->RegisterChunk( tagCMAP, HandleCMAP );
				readPtr->RegisterChunk( tagBMHD, HandleBMHD );
				readPtr->RegisterChunk( tagCAMG, HandleCAMG );

				// ANIM things
				readPtr->RegisterChunk( tagANHD, HandleANHD );
				readPtr->RegisterChunk( tagDLTA, HandleDLTA );

				readPtr->ParseChunks();
				++nFrame;

				int xScr, yScr;				// used later to determine
											//  size of scaled image

				{ // Process the frame
				fixed w( myBMHD.w, 0 ), h( myBMHD.h, 0 );
				fixed y( 0, 0 );

				for ( yScr=0;
						long(y)<long(h);
						y+=yScale, ++yScr )
					{
					fixed x(0,0);
					for ( xScr=0; long(x)<long(w); x+=xScale, ++xScr )
						{
						int i = display->GetPixelIndex( point( int(x),int(y) ) );
						display->SetColorIndex( i );
						display->PutPixel( point( xOfs+xScr, yOfs+yScr ) );
						}
					}
				}

				// At this location, because now xScr & yScr have been calculated
				if ( nFrame == 1 )
					{ // Create a new "delta" graphport
					rpDelta = new rastGraphPort( xScr, yScr, 256 );
					}

				{ // Output one frame's worth of data
				if ( nFrame == 1 )
					{ // Output ILBM/BODY header/chunk
					writePtr = new iffWrite( output );
					writePtr->NewForm( tagPBM );

						ilbmBMHD scaledBMHD = myBMHD;
						// Adjust width and height
						scaledBMHD.w = xScr;
						scaledBMHD.h = yScr;
						scaledBMHD.compression = ilbmBMHD::cmpNone;

						writePtr->NewChunk( tagCMAP );
						myCMAP.Write( writePtr );

						writePtr->NewChunk( tagBMHD );
						scaledBMHD.Write( writePtr );

						writePtr->NewChunk( tagBODY );
						for ( int y=0; y<yScr; ++y )
							{
							for ( int x=0; x<xScr; ++x )
								writePtr->WriteChunkByte(
									display->GetPixelIndex(
									point( xOfs+x, yOfs+y ) ) );
							if ( x & 1 )
								writePtr->WriteChunkByte( 0 );
							}

					delete writePtr;
					}
				else
					{ // Output ANIM/DLTA header/chunk
					writePtr = new iffWrite( output );
					writePtr->NewForm( tagPBM );

					myANHD.w = xScr;
					myANHD.h = yScr;
					writePtr->NewChunk( tagANHD );
					myANHD.Write( writePtr );

					writePtr->NewChunk( tagDLTA );
					writePtr->WriteChunkLong( 4 );			// offset

					assert( myANHD.w <= 127 );
					for ( int y=0; y<myANHD.h; ++y )
						{
						writePtr->WriteChunkByte( 1 );		// nTokens
						writePtr->WriteChunkByte( myANHD.w );
						for ( int x=0; x<myANHD.w; ++x )
							writePtr->WriteChunkByte( display->GetPixelIndex( point(xOfs+x,yOfs+y) ) );
						}

					delete writePtr;
					}
				if ( bPause )
					getch();
				}

				delete readPtr;
				}

			delete readPtr;
			delete writePtr;
			delete rpDelta;

#if 0
			if ( bPause )
				{
				if ( bDisplay )
					gotoxy( 1, 25 );
				cout << "Press any key to exit...";
				getch();
				}
#endif
			}
		}
	else
		cout << "Error: file \"" << szAnimName << "\" not found\n";
	}

