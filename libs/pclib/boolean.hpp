//=============================================================================
// boolean.h: a concrete class representing TRUE or FALSE
//=============================================================================
/*

Documentation:

	Abstract:
		this class implements a complete set of boolean operations on the
		class type boolean.

	History:
			Created	? Kevin T. Seghetti
			First Documented: 10-19-92 02:06pm

			20 Oct 92  WBNIV	Added output capability

	Class Hierarchy:
			none


	Dependancies:
			none

	Restrictions:


	Example:
		boolean foo = boolean::TRUE;
		boolean bar = boolean::FALSE;


		if(foo & bar)
			won't happen;

		if(foo | bar)
			will happen;

		if(foo * bar)
			won't happen;


*/
//==============================================================================
// use only once insurance

#ifndef PCLIB_BOOLEAN_H
#define PCLIB_BOOLEAN_H

//==============================================================================
// dependencies
#include <iostream.h>
#include <pclib/general.hpp>				// shouldn't this be libtypes.h ?

//==============================================================================
// class declaration

class boolean
{
	unsigned char fl:1;
	void set(int i) { fl = !!(i); }
	boolean& operator-=(const boolean& i);		// make these invalid operations
	boolean& operator/=(const boolean& i);
	boolean& operator-(const boolean& i);
	boolean& operator/(const boolean& i);
public:
	enum {FALSE, TRUE};

	boolean(int i = 0) { fl = !!(i); }

	boolean& operator= (const boolean& i) { fl = i.fl; return *this; }
	boolean& operator= (int i) { set(i); return *this; }

	boolean& operator|=(const boolean& i) { fl |= i.fl; return *this; }
	boolean& operator+=(const boolean& i) { fl |= i.fl; return *this; }
	boolean& operator&=(const boolean& i) { fl &= i.fl; return *this; }
	boolean& operator*=(const boolean& i) { fl &= i.fl; return *this; }
	boolean& operator^=(const boolean& i) { fl ^= i.fl; return *this; }

	boolean operator+(const boolean& a) { boolean res = a; res += *this; return res; }
	boolean operator|(const boolean& a) { boolean res = a; res |= *this; return res; }
	boolean operator*(const boolean& a) { boolean res = a; res *= *this; return res; }
	boolean operator&(const boolean& a) { boolean res = a; res &= *this; return res; }
	boolean operator^(const boolean& a) { boolean res = a; res ^= *this; return res; }

	operator int(void) const { return(int(fl?1:0)); }

	// Printing
	friend ostream& operator<<(ostream& s, boolean &b)	{ return s << (b.fl ? "TRUE" : "FALSE"); }
};

//=============================================================================
// inline functions

#endif

//=============================================================================

