
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>

#include <mem.h>

#include <pclib/general.hpp>
#include <pclib/iffilbm.hpp>
#include <pclib/iffpbm.hpp>
#include <pclib/error.hpp>
#include <pclib/mem.hpp>
#include <pclib/grphport.hpp>

#include "grabber.hpp"

unsigned char far *image[1000];

ilbmCMAP myCMAP;
ilbmBMHD myBMHD;
ilbmCAMG myCAMG;
pbmBODY mypbmBODY(&myBMHD);

errorcode
HandleCMAP(iffRead *readPtr)
	{
	myCMAP.Read(readPtr);
	return(0);
	}

//=============================================================================

errorcode
HandleBMHD(iffRead *readPtr)
	{
	myBMHD.Read(readPtr);

	// Allocate memory for entire image
	for ( int i=0; i<myBMHD.h; ++i )
	 {
		image[i] = (unsigned char*)malloc( myBMHD.w );
		assert(image[i]);
	 }

	return(0);
	}

//=============================================================================

errorcode
HandleCAMG(iffRead *readPtr)
	{
	myCAMG.Read(readPtr);
	return(0);
	}

//==============================================================================

errorcode
HandleBODY(iffRead *readPtr)
	{
	cout << "ilbmHandleBody::boom\n";
	return(0);
	}

//=============================================================================

static int nParseLine;

errorcode ParseLine(ubyte *line)
	{
	memcpy( image[nParseLine], line, myBMHD.w );

	if ( nParseLine < 200 )
		{
		for ( int x=0; x<320 && x<myBMHD.w; ++x )
			{
			int i = GetPixel( x, nParseLine );

			display->SetColor( color(
				myCMAP.Red(i), myCMAP.Green(i), myCMAP.Blue(i) ) );
			display->PutPixel(point(x,nParseLine));
			}
		}

	++nParseLine;

	return(NOERR);
	}

errorcode
HandlePBMBODY(iffRead *readPtr)
	{
	nParseLine = 0;
	mypbmBODY.Read(readPtr,&ParseLine);
	return(0);
	}

//=============================================================================

void
ReadILBM( const char *szLbmName )
	{
	ifstream input( szLbmName, ios::in|ios::binary );

	if ( input )
		{
		iffRead *readPtr = NULL;

		readPtr = new iffRead(input);
		readPtr->NextForm();
		if(readPtr->GetForm() == tagILBM)
			{
			readPtr->RegisterChunk(tagCMAP,HandleCMAP);
			readPtr->RegisterChunk(tagBMHD,HandleBMHD);
			readPtr->RegisterChunk(tagCAMG,HandleCAMG);
			readPtr->RegisterChunk(tagBODY,HandleBODY);
			readPtr->ParseChunks();
			}
		if(readPtr->GetForm() == tagPBM)
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
		cout << "Error: file \"" << szLbmName << "\" not found\n";
	}

