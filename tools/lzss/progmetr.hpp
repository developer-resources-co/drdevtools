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
	void Create( int x=1, int y=1, unsigned long lower=0, unsigned long upper=99 );

	int nChars;
	int nKPerChar;

	int x_pm, y_pm;
	unsigned long lower_pm, upper_pm;

public:
	progressMeter( int x, int y, unsigned long lower=0, unsigned long upper=99 )
		{ Create( x, y, lower, upper ); }
	progressMeter( unsigned long lower=0, unsigned long upper=99 );

	virtual ~progressMeter( void );

	void proc( unsigned long lStep );
	virtual char displayChar( unsigned long lStep );
	};

#endif

