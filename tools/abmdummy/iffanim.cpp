//==============================================================================
// iffanim.cpp: funcions for anim parser built on iff parser
//==============================================================================

#include <ctype.h>
#include <assert.h>

#include "iffilbm.hpp"
#include "iffanim.hpp"
#include <pclib/grphport.hpp>

//==============================================================================

ostream&
	operator<<( ostream& s, animANHD& anhd )
	{
	s << "[ ANHD ]" << endl;
	s << "operation: "
	  << ( isprint( anhd.operation ) ? anhd.operation : int(anhd.operation) ) << "\n";
	s << "mask: " << int(anhd.mask) << "\n";
	s << "w: " << anhd.w << "\th: " << anhd.h << "\n";
	s << "x: " << anhd.x << "\ty: " << anhd.y << "\n";
	s << "abstime: " << anhd.abstime << "\treltime: " << anhd.reltime << "\n";
	s << "interleave: " << int(anhd.interleave) << "\n";
	s << "bits: " << anhd.bits << "\n";
	s << "\n";

	return( s );
	}


errorcode
	animANHD::Read( iffRead* readPtr )
	{
	operation = readPtr->GetChunkByte();
	mask = readPtr->GetChunkByte();
	w = readPtr->GetChunkWord();
	h = readPtr->GetChunkWord();
	x = readPtr->GetChunkWord();
	y = readPtr->GetChunkWord();
	abstime = readPtr->GetChunkLong();
	reltime = readPtr->GetChunkLong();
	if ( (interleave = readPtr->GetChunkByte()) == 0 )
		interleave = 2;
	readPtr->GetChunkByte();
	bits = readPtr->GetChunkLong();
	// readPtr->GetChunkByte( "16" );

	return(NOERR);
	}

//==============================================================================

errorcode
	animANHD::Write( iffWrite* pWrite )
	{
	pWrite->WriteChunkByte( operation );
	pWrite->WriteChunkByte( mask );
	pWrite->WriteChunkWord( w );
	pWrite->WriteChunkWord( h );
	pWrite->WriteChunkWord( x );
	pWrite->WriteChunkWord( y );
	pWrite->WriteChunkLong( abstime );
	pWrite->WriteChunkLong( reltime );
	pWrite->WriteChunkByte( interleave );
	pWrite->WriteChunkByte( 0 );
	pWrite->WriteChunkLong( bits );
	// pWrite->WriteChunkByte( "16" );

	return NOERR;
	}

//ננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננננ

#if 0
ilbmDLTA::ilbmDLTA( animANHD* newANHD, iffRead* pRead, rastGraphPort* rp )
	{
	cerr << "ILBM DLTA chunk not supported yet" << endl;
	}
#endif

//==============================================================================

animDLTA::animDLTA( animANHD* newANHD, iffRead* readPtr, rastGraphPort* rp )
	{
	anhd = newANHD;

	// Temporary line buffer used to decompress masks into
	ubyte* maskBuffer = NULL;
	if ( anhd->mask )
		{
		maskBuffer = new ubyte[ anhd->w ];
		assert( maskBuffer );
		}

	readPtr->GetChunkLong();						// offset

	switch ( anhd->operation )
		{
		case 'K':
			{
			int y = 0;
			for ( uint lines=anhd->h; lines; --lines, ++y )
				{
				// Beginning of line
				ubyte* linePtr = rp->mapPtr + rp->lineArrayPtr[y];
				ubyte* _linePtr = linePtr;
				int x = 0;									//D

				for ( int nTokens=readPtr->GetChunkByte(); nTokens; --nTokens )
					{
					byte command = readPtr->GetChunkByte();

					if ( command < 0 )
						{ // SKIP
						// copy data from previous frame
						_linePtr += command & 0x7F;	//T

						x += command & 0x7F;				//D
						}
					else if ( command == 0 )
						{
						ubyte len = readPtr->GetChunkByte();
						ubyte col = readPtr->GetChunkByte();

						rp->SetColorIndex( col );	//D
						for ( int i=0; i<len; ++i )
							{
							*_linePtr = col;				//T
							_linePtr++;						//T

							rp->PutPixel( point(anhd->x+x++,anhd->y+y) );	//D
							}

						}
					else
						{
						for ( int i=0; i<command; ++i )
							{
							ubyte col = readPtr->GetChunkByte();
							*_linePtr = col;				//T
							_linePtr++;						//T

							rp->SetColorIndex( col );		//D
							rp->PutPixel( point(anhd->x+x++,anhd->y+y) );	//D
							}
						}
					}
				}
			break;
			}
		default:
			{
			cerr << "Unknown compression type" << endl;
			break;
			}
		}
	delete[] maskBuffer;
	}

//==============================================================================

#if 0
#include <stdlib.h>
#include <pclib/color.hpp>
#include <pclib/grphport.hpp>
extern rastGraphPort* display;
extern ilbmCMAP myCMAP;

errorcode
	animDLTA::Read(iffRead* readPtr, errorcode parseLine(ubyte* line))
	{
	ubyte* linePtr = (ubyte*)calloc( anhd->w, 1 );			//new ubyte[anhd->w];
	assert( linePtr );

	if ( !linePtr )
		return( ERROR_NOMEM );

	readPtr->GetChunkLong();						// offset

	switch ( anhd->operation )
		{
		case 'K':
			{
			int y = 0;
			for ( uint lines=anhd->h; lines; --lines )
				{
				// Beginning of line
				ubyte* _linePtr = linePtr;
				int x = 0;									//D

				for ( int nTokens=readPtr->GetChunkByte(); nTokens; --nTokens )
					{
					byte command = readPtr->GetChunkByte();

					if ( command < 0 )
						{ // SKIP
						// copy data from previous frame
						_linePtr += command & 0x7F;	//T

						x += command & 0x7F;				//D
						}
					else if ( command == 0 )
						{
						ubyte len = readPtr->GetChunkByte();
						ubyte col = readPtr->GetChunkByte();

						display->SetColorIndex( col );	//D
						for ( int i=0; i<len; ++i )
							{
							*_linePtr = col;				//T
							_linePtr++;						//T

							display->PutPixel( point(anhd->x+x++,anhd->y+y) );	//D
							}

						}
					else
						{
						for ( int i=0; i<command; ++i )
							{
							ubyte col = readPtr->GetChunkByte();
							*_linePtr = col;				//T
							_linePtr++;						//T

							display->SetColorIndex( col );		//D
							display->PutPixel( point(anhd->x+x++,anhd->y+y) );	//D
							}
						}
					}
				(*parseLine)(linePtr);
				++y;
				}
			}
		default:
			return( -1 );
		}
	// delete[] linePtr;
	}
#endif

//==============================================================================
