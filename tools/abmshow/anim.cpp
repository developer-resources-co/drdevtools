
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

#include "abmshow.hpp"
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
		image[i] = (char far*)malloc( myBMHD.w );
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
ReadANIM( const char *szAnimName )
	{
	for ( boolean fLoopForever = !boolean( nLoop );
			fLoopForever || nLoop; --nLoop )
		{
		ifstream input( szAnimName, ios::in|ios::binary );

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

				for (;;)
					{
					check_ctrl_break();

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

					if ( bPause )
						getch();

					delete readPtr;
					}

				delete readPtr;
				}
			}
		else
			cout << "Error: file \"" << szAnimName << "\" not found\n";
		}
	}

