
#include <iostream.h>
#include <iomanip.h>

#include <math.h>

#include <pclib/fixed.hpp>

void main( int argc, char* argv[] )
	{
	double d = 0;
	fixed f( 0, 0 );

	for ( ; d < 1; d += 0.0333333333, f += fixed( 0, 2185 ) )
		{
		cout << "sin( double d=" << setw(8) << d << " ): " << sin( d );
		cout << "  sin( fixed f=" <<
			int(f) << '.' << hex << setw(4) << (long(f) & 0xFFFF) << dec << " ): " <<
			int( sin( f ) ) << '.' << hex << setw(4) << (long( sin(f) ) & 0xFFFF);
		cout << endl;
		}
	}
