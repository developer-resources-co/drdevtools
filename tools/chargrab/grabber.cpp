//===========================================================================
// grabber.cpp
//===========================================================================

//#define VERBOSE

//== C++ ====================================================================
#include <iostream.h>
#include <fstream.h>

//== C ======================================================================
#include <stdlib.h>
//#include <string.h>
//#include <mem.h>
#include <assert.h>

//== Turbo C++ (IBM PC C libraries, but not standard C) =====================
#include <dir.h>
#include <dos.h>
#include <malloc.h>
#include <conio.h>

//== Adept Creations Library ================================================
#include <pclib/general.hpp>
#include <pclib/error.hpp>
#include <pclib/mem.hpp>

//== Application ============================================================
#include "grabber.hpp"
#include "grabinpt.hpp"
#include "cloption.hpp"
#include "quit.hpp"
#include "iffmisc.hpp"

//===========================================================================

void
	check_ctrl_break( void )
	{
	kbhit();
	}


Picture::~Picture( void )
	{
	delete rp;
	delete BitmapHeader;
	delete ColorMap;
	}


Picture::Picture()
	{
	pNext = NULL;

	rp = NULL;
	BitmapHeader = NULL;
	ColorMap = NULL;
	}


Picture::Picture( istream& input )
	{
	pNext = NULL;

	rp = NULL;
	BitmapHeader = NULL;
	ColorMap = NULL;
	oldBODY = NULL;
	newBODY = NULL;

	iffRead* readPtr = NULL;

	readPtr = new iffRead( input );
	readPtr->NextForm();					// new should put you at first
	iffTag form = readPtr->GetForm();
	assert( form == tagILBM || form == tagPBM || form == tagANIM );

	iffTag chunk;
	while ( readPtr->NextChunk() && ( chunk = readPtr->GetChunk() ) )
		{
		if ( chunk == tagCMAP )
			{
			assert( !ColorMap );

			ColorMap = new ilbmCMAP( readPtr );
			assert( ColorMap );
#ifdef VERBOSE
//			cout << *ColorMap << endl;
#endif
			}
		else if ( chunk == tagBMHD )
			{
			assert( !BitmapHeader );

			BitmapHeader = new ilbmBMHD( readPtr );
			assert( BitmapHeader );
#ifdef VERBOSE
			cout << *BitmapHeader << endl;
#endif
			}
		else if ( chunk == tagBODY )
			{
			assert( readPtr->GetForm() == tagILBM || readPtr->GetForm() == tagPBM );

			// Can't deal with more than 8 bitplane pictures yet
			assert( BitmapHeader->nPlanes <= 8 );

			rp = new rastGraphPort( BitmapHeader->w, BitmapHeader->h, 256 );
			assert( rp );

			if ( readPtr->GetForm() == tagILBM )
				oldBODY = new ilbmBODY( readPtr, BitmapHeader, rp );
			else if ( readPtr->GetForm() == tagPBM )
				newBODY = new pbmBODY( readPtr, BitmapHeader, rp );
			}
		}
	delete readPtr;
	}


class Animation
	{
public:
	rastGraphPort* rp;
	ilbmCMAP* ColorMap;
	// This appears in the first frame
	ilbmBMHD* BitmapHeader;
	ilbmBODY* oldBODY;
	pbmBODY*  newBODY;
	// This appears in subsequent frames
	animANHD* AnimationHeader;

	Animation* pPrev, * pNext;

	Animation( istream& input );
	~Animation( void );
	};

Animation::~Animation( void )
	{
	// !!! Add code to walk chain
	delete rp;
	delete BitmapHeader;
	delete AnimationHeader;
	delete ColorMap;
	}


Animation::Animation( istream& input )
	{
	assert( input );

	int nFrame = 0;

	pPrev = pNext = NULL;

	rp = NULL;
	BitmapHeader = NULL;
	AnimationHeader = NULL;
	ColorMap = NULL;

	Animation* a = this;

	iffTag checkAnim;
	iffRead *readPtr = NULL;

	ReadIFFTag( input );					// FORM
	ReadIFFLength( input );				// maybe save (?)
	checkAnim = ReadIFFTag( input );
	assert( checkAnim == tagANIM );

	for (;;)
		{
		readPtr = new iffRead( input );
		if ( !readPtr->NextForm() )
 			break;

		if ( ++nFrame > 1 )
			{
			a->pNext = (Animation*)calloc( 1, sizeof( Animation ) );
			a->pNext->pPrev = a;
			a = a->pNext;
			a->pNext = NULL;
			}

		assert( readPtr->GetForm() == tagILBM || readPtr->GetForm() == tagPBM );

		iffTag chunk;
		while ( readPtr->NextChunk() && ( chunk = readPtr->GetChunk() ) )
			{
			if ( chunk == tagCMAP )
				{
				assert( !a->ColorMap );

				a->ColorMap = new ilbmCMAP( readPtr );
				assert( ColorMap );
				}
			else if ( chunk == tagCAMG )
				;
			else if ( chunk == tagDPPS )
				;
			else if ( chunk == tagCRNG )
				;
			else if ( chunk == tagTINY )
				;
			else if ( chunk == tagBMHD )
				{
				;
				assert( !a->BitmapHeader );

				a->BitmapHeader = new ilbmBMHD( readPtr );
				assert( a->BitmapHeader );

#ifdef VERBOSE
				cout << *(a->BitmapHeader) << endl;
#endif

				// Can't deal with more than 8 bitplane pictures yet
				assert( a->BitmapHeader->nPlanes <= 8 );
				}
			else if ( chunk == tagBODY )
				{
				assert( readPtr->GetForm() == tagILBM || readPtr->GetForm() == tagPBM );
				a->rp = new rastGraphPort( a->BitmapHeader->w,
					a->BitmapHeader->h, 256 );
				assert( a->rp );

				if ( readPtr->GetForm() == tagILBM )
					oldBODY = new ilbmBODY( readPtr, a->BitmapHeader, a->rp );
				else if ( readPtr->GetForm() == tagPBM )
					newBODY = new pbmBODY( readPtr, a->BitmapHeader, a->rp );
				}
			else if ( chunk == tagANHD )
				{
				a->AnimationHeader = new animANHD( readPtr );
#if 1
				// !!! Actually, create new rastport based on "interleave"
				a->rp = new rastGraphPort( *a->pPrev->rp );
#else
				// testing to screen
				a->rp = a->pPrev->rp;
#endif
#ifdef VERBOSE
				cout << *a->AnimationHeader << endl;
#endif
				}
			else if ( chunk == tagDLTA )
				{
				assert( readPtr->GetForm() == tagILBM || readPtr->GetForm() == tagPBM );
				if ( readPtr->GetForm() == tagILBM )
					cerr << "ILBM DLTA not supported yet" << endl;
//					new ilbmDLTA( a->AnimationHeader, readPtr, a->rp );
				else if ( readPtr->GetForm() == tagPBM )
					new animDLTA( a->AnimationHeader, readPtr, a->rp );
				}
			else if ( chunk == tagCopyright )
				{
				//cout << "(c) ";
				}
			}

		delete readPtr;
		}
	}


screenGraphPort* screen;

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

void
DisplayRastPort( rastGraphPort* rp )
	{ // Copy the portion that will fit on the screen
	for ( int y=0; y<min(rp->GetHeight(),200); ++y )
		for ( int x=0; x<min(rp->GetWidth(),320); ++x )
			{
			rp->Move( point(x,y) );
			screen->Move( point(x,y) );
			screen->SetColorIndex( rp->GetPixelIndex() );
			screen->PutPixel();
			}
	}


void
FlipBook( Animation* anim )
	{
	screen->Erase();
	for ( Animation* p = anim; p; p = p->pNext )
		DisplayRastPort( p->rp );
//!!!	rather would have something like: screen << p->rp;
//!!!	screen->Move( 0,0 );  screen << p->rp;
//!!!	which would start rendering at the current point

	}

Picture* pict1;

/***** OLD CHARGRAB *****/
boolean nukeChars = boolean::TRUE;
boolean bTile0Blank = boolean::FALSE;
int nLimitChars = 2048;
boolean checkRedundantChars = boolean::TRUE;
boolean checkFlips = boolean::TRUE;

#define SCRXSIZE 320

int bgpri = 0;
int chars;
uchar huge *data[MEGAMAXCHARS];
int xCount,yCount;
uint map[MAXGRID_Y][MAXGRID_X];
int nBadChars;

int x,y;
int a,b,c,d,e,f;
int a2,b2;
ubyte mapBad[MAXGRID_Y][MAXGRID_X];
int charX = CHAR_X;
int charY = CHAR_Y;
int charSize = CHAR_X*CHAR_Y/2;
uint palette = 0;

uchar huge *tempchar;
uchar huge *tempcharX;
uchar huge *tempcharY;
uchar huge *tempcharXY;

extern boolean bAllTransparent;
extern boolean bGenesis;
#include "gen.cpp"

CharacterData* chData;

int
	CheckDups (int a,int b)
	{
	if ( checkRedundantChars )
		for ( int e=0; e<chars; ++e )
			{
			if (!memcmp (data[e],tempchar,charSize))
				{
				map[a][b]=e|palette;
				return (boolean::TRUE);
				}

			if ( checkFlips )
				{
				if (!memcmp (data[e],tempcharX,charSize))
					{
					map[a][b]=e | chData->xFlip() | palette;
					return (boolean::TRUE);
					}
			if (!memcmp (data[e],tempcharY,charSize))
				{
				map[a][b]=e | chData->yFlip() | palette;
				return (boolean::TRUE);
				}
			if (!memcmp (data[e],tempcharXY,charSize))
				{
				map[a][b]=e | chData->xFlip() | chData->yFlip() | palette;
				return (boolean::TRUE);
				}
			}
		}
	return (boolean::FALSE);
	}


/***** OLD CHARGRAB *****/

void
	main (int argc,char *argv[])
	{
	GetInput( argc, argv );
	ctrlbrk( ctrlBreakHandler );

	screen = new screenGraphPort( 320, 200, 256 );
	assert( screen );

	ifstream* input;

	input = new ifstream( InputName, ios::in | ios::binary );
	assert( input );
	pict1 = new Picture( *input );
	delete input;

	rastGraphPort* rp = pict1->newBODY ? pict1->newBODY->rp : pict1->oldBODY->rp;
	DisplayRastPort( rp );

	// Convert to characters

/***** OLD CHARGRAB *****/
	for ( a=0; a<nLimitChars; ++a )
		{
		data[ a ]= (unsigned char huge*)malloc( (long)charSize );
		assert( data[ a ] );
		}
	tempchar = (unsigned char huge*)malloc    ((long) charSize);
	assert(tempchar);
	tempcharX = (unsigned char huge*)malloc    ((long) charSize);
	assert(tempcharX);
	tempcharY = (unsigned char huge*)malloc    ((long) charSize);
	assert(tempcharY);
	tempcharXY = (unsigned char huge*)malloc    ((long) charSize);
	assert(tempcharXY);

	y=x=chars=0;

	if ( !nukeChars )
    	OpenExistingCharFile( OutputFile );
	else if ( bTile0Blank )
		memset( data[chars++], 0, charSize );					// Zero tile #0

	yCount = pict1->BitmapHeader->h/charX + !!(pict1->BitmapHeader->h & (charY-1));
	xCount = pict1->BitmapHeader->w/charY + !!(pict1->BitmapHeader->w & (charX-1));

	if ( bGenesis )
		chData = new GenesisCharacterData();
	else
		chData = new SnesCharacterData();

	for ( a=0; a<yCount; ++a )
		{
		// Check for break (only) once per line
		check_ctrl_break();

		for ( b=0; b<xCount; ++ b)
			{
			a2=a*charY;
			b2=b*charX;
			if ( chData->MakeChar( rp, b*charX, a*charY ) )
				{
				map[a][b]=chars|palette;
				if ( CheckDups(a,b) )
		 	 		{
					if ( a2 < screen->GetHeight() )
						{
						screen->SetColor( color( 0, 0, 0 ) );
						screen->Box( point( b2%SCRXSIZE, a2),
							point( (b2+(charX-1))%SCRXSIZE, a2+(charY-1) ) );
						}
					}
				else
	         		{ // copy if not duplicated
					if ( a2 < screen->GetHeight() )
						{
						screen->SetColor( chars>nLimitChars ? color(255,0,0) : color(0,255,0) );
						screen->Box( point( b2%SCRXSIZE, a2 ),
							point( (b2+(charX-1))%SCRXSIZE, a2+(charY-1) ) );
						}

        			memcpy (data[chars],tempchar,charSize);
	     			++chars;
					}
//				if (chars>maxChars) Error ("Too many chars!");
				}
			else
	      	{
				mapBad[a][b] = boolean::TRUE;
	         nBadChars++;
	         }
			}
		}

	// CHARACTERS
	WriteChars( OutputFile );

	// MAP
	WriteMap( MapFile );

	// ERROR/WARNING MESSAGES
	if ( nBadChars > 0 )
		{
		Beep();
		printf( "%d chars invalid\n", nBadChars );
		}

	if ( chars > nLimitChars )
		{
		Beep();
		printf( "Character count exceeded maximum of %d chars by %d chars\n",
			nLimitChars, chars-nLimitChars );
		}

	for (a=0;a<nLimitChars;a++)
	 {
	   free( data[a] );
	 }
	free( tempchar );
	free( tempcharX );
	free( tempcharY );
	free( tempcharXY );

/***** OLD CHARGRAB *****/


	delete pict1;
	delete screen;

	Quit();
	}
