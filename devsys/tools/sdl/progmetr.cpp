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

#include "progmetr.h"

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))

progressMeter::Create( unsigned long lower, unsigned long upper, int x, int y )
	{
	lower_pm = lower;
	upper_pm = upper;

	gotoxy( x, y );
	cout << "ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ ";
	cout << "³                                                                             ³Û";
	cout << "ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙÛ";
	cout << " ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß";

	struct text_info ti;
	gettextinfo( &ti );
	x_pm = x;
	y_pm = ti.cury-4;							// adjust by number of lines printed!

	gotoxy( x_pm+1, y_pm+1 );

	nChars = 79-1-2;
	nKPerChar = MAX( 1, (upper-lower) / nChars );
//	bFill = FALSE;
	}

progressMeter::progressMeter( unsigned long lower, unsigned long upper )
	{
	struct text_info ti;

	gettextinfo( &ti );
	Create( lower, upper, (int)ti.curx, (int)ti.cury );
	}

progressMeter::proc( unsigned long lStep )
	{
	int percentage;

	struct text_info ti;
	gettextinfo( &ti );

	percentage = lStep ? (lStep*100L)/(upper_pm-lower_pm) : 0;

	for ( int i=0; i<77*percentage/100; ++i )
		cout << displayChar( lStep );

	gotoxy( 39-2, y_pm+1 );
	printf( "%3d%%", percentage );

	gotoxy( ti.curx, ti.cury );
	}


progressMeter::~progressMeter( void )
	{
	gotoxy( 1, y_pm+4 );
	}


char
	progressMeter::displayChar( unsigned long lStep )
	{
	return( 'Û' );
	}
