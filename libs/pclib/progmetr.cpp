/////////////////////////////////////////////////////////////////////////////
//
// File:			progmetr.cpp
//
// Project:		Assembler
//
// 15 Aug 92	WBNIV		Created
// 13 Nov 92	WBNIV		Changed bounds to unsigned longs, prints percentage,
//								 cursor movement less destructive
//
/////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <conio.h>
#include <stdio.h>											// printf

#include <pclib/progmetr.hpp>

#ifdef __WATCOMC__
#define gotoxy(x,y) _settextposition(x, y)
#include <graph.h>
#endif

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))

progressMeter::Create( int x, int y, unsigned long lower, unsigned long upper )
	{
	lower_pm = lower;
	upper_pm = upper;

	gotoxy( x, y );
	cout << "\xDA\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF ";
	cout << "\xB3                                                                             \xB3\xDB";
	cout << "\xC0\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9\xDB";
	cout << " \xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF";



#ifdef __BORLANDC__
	struct text_info ti;
	gettextinfo( &ti );
	y_pm = ti.cury-4;							// adjust by number of lines printed!
#endif

#ifdef __WATCOMC__
	struct rccoord old_pos;
	old_pos = _gettextposition();
	y_pm = old_pos.row;
#endif

	x_pm = x;

	gotoxy( x_pm+1, y_pm+1 );

	nChars = 79-1-2;
	nKPerChar = MAX( 1, (upper-lower) / nChars );
//	bFill = FALSE;
	}

progressMeter::progressMeter( unsigned long lower, unsigned long upper )
	{

#ifdef __BORLANDC__
	struct text_info ti;
	gettextinfo( &ti );
	Create( lower, upper, (int)ti.curx, (int)ti.cury );
#endif

#ifdef __WATCOMC__
	struct rccoord old_pos;
	old_pos = _gettextposition();
	Create( lower, upper, (int)old_pos.col, (int)old_pos.row );
#endif
	}

progressMeter::proc( unsigned long lStep )
	{
	int percentage;

#ifdef __BORLANDC__
	struct text_info ti;
	gettextinfo( &ti );
	int x = ti.curx;							// adjust by number of lines printed!
	int y = ti.cury;
#endif

#ifdef __WATCOMC__
	struct rccoord old_pos;
	old_pos = _gettextposition();
	int x = old_pos.col;							// adjust by number of lines printed!
	int y = old_pos.row;
#endif

	percentage = lStep ? (lStep*100L)/(upper_pm-lower_pm) : 0;

	for ( int i=0; i<77*percentage/100; ++i )
		cout << displayChar( lStep );

	gotoxy( 39-2, y_pm+1 );
	printf( "%3d%%", percentage );

	gotoxy( x, y );
	}


progressMeter::~progressMeter( void )
	{
	gotoxy( 1, y_pm+4 );
	}


char
	progressMeter::displayChar( unsigned long lStep )
	{
	return( '\xDB' );
	}
