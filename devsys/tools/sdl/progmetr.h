/////////////////////////////////////////////////////////////////////////////
//
// File:			progmetr.h
//
// Project:		Assembler
//
// 15 Aug 92	WBNIV		Created 
//
/////////////////////////////////////////////////////////////////////////////

#ifndef PROGRESS_METER_H
#define PROGRESS_METER_H

#include <conio.h>

class progressMeter
	{
	Create( unsigned long lower=0, unsigned long upper=99, int x=1, int y=1 );

	int nChars;
	int nKPerChar;

	int x_pm, y_pm;
	unsigned long lower_pm, upper_pm;

public:
	progressMeter( unsigned long lower=0, unsigned long upper=99, int x, int y )
		{ Create( lower, upper, x, y ); }
	progressMeter( unsigned long lower=0, unsigned long upper=99 );

	virtual ~progressMeter( void );

	proc( unsigned long lStep );
	virtual char displayChar( unsigned long lStep );
	};

#endif

