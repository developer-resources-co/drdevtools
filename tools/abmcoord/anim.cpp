
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>
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
#include <pclib/filename.hpp>
//#include <pclib/viewport.hpp>

#include "abmcoord.hpp"
#include "cloption.hpp"

unsigned char far* image[1000];

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
		image[i] = SafeFarMalloc( myBMHD.w );
		assert( image[i] );
		}

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

void
ReadANIM( const char *szAnimName, char* szSrcName )
	{
//	boolean bFirstFrame = boolean::TRUE;
	ifstream input( szAnimName, ios::in|ios::binary );
	ofstream output( szSrcName, ios::out );

	if ( input )
		{
		iffTag checkAnim;
		iffRead *readPtr = NULL;

		point* frameOffset = new point(0,0);

		ReadIFFTag( input );					// FORM
		ReadIFFLength( input );				// maybe save (?)
		checkAnim = ReadIFFTag( input );

		if ( checkAnim == tagANIM )
			{
			int nFrame = 1;

			filename f( szSrcName );

#define DEFAULT_LABEL "Coords"

			if ( !pLabel )
				{
				pLabel = new char[ 1 + strlen( f.file() ) + strlen( DEFAULT_LABEL ) + 1 ];
				strcpy( pLabel, "_" );
				strcat( pLabel, f.file() );
				strcat( pLabel, DEFAULT_LABEL );
				}

			if ( *pLabel )
				{
				output << "\txdef\t" << pLabel << '\n';
				output << pLabel << ":\n";
				}

			for (;;)
				{
				readPtr = new iffRead( input );
				if ( !readPtr->NextForm() )
					break;

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

				// Output the frame coordinate data
				int x, y;
				colorMapIndex i = -1;

				for ( y=0; y<myBMHD.h && i!=colorToTrack; ++y )
					{
					for ( x=0; x<myBMHD.w && i!=colorToTrack; ++x )
						{
						i = display->GetPixelIndex( point(x,y) );
						}
					}

				if ( nFrame==1 && bReferenceFrameOne )
					{
					delete frameOffset;
					frameOffset = new point( x, y );
					}
				else
					{
					if ( (x==myBMHD.w) && (y==myBMHD.h) )
					 {
						cerr << "No data found on frame #" << nFrame << "\r";
						output << "\tdc.w\t" << "32767,32767";
					 }
					else output << "\tdc.w\t" << x-frameOffset->X()
							 << "," << y-frameOffset->Y();
					output << "\t; Frame #" << nFrame << "\n";
					}
				++nFrame;

				delete readPtr;
				}

			delete[] pLabel;
			delete frameOffset;
			delete readPtr;

			if ( bPause )
				{
				if ( bDisplay )
					gotoxy( 1, 25 );
				cout << "Press any key to exit...";
				getch();
				}
			}
		}
	else
		cout << "Error: file \"" << szAnimName << "\" not found\n";
	}

