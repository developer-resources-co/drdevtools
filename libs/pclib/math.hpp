//==============================================================================
// math.h:
//==============================================================================
/*

Documentation:

	fixed point math class

	Abstract:
		fixed is a concrete data type implementing a 16 bit fraction,
		a 15 bit integer, and 1 bit of sign.

	History:
			Created	10-19-92 02:28pm Kevin T. Seghetti

	Class Hierarchy:
		none

	Dependancies:
		general

	Restrictions:
		untested, incomplete

	Example:

*/

//==============================================================================
// use only once insurance

#if !defined(GFXTOOLS_math_H)
#define GFXTOOLS_math_H

#include <pclib\general.hpp>

//==============================================================================

class fixed
{
public:

	void set(int i, uint f = 0)
	{
		parts.frac = f;
		parts.in = i;
		}
	void set(long v) { val = v; }

	fixed(int i = 0, uint f = 0) { parts.frac = f; parts.in = i; }

	fixed& operator= (const fixed& i) { val = i.val; return *this; }
	fixed& operator= (int i) { set(i); return *this; }
	fixed& operator= (long i) { set(i); return *this; }

	fixed& operator&=(const fixed& i) { val&= i.val; return *this; }
	fixed& operator|=(const fixed& i) { val |= i.val; return *this; }
	fixed& operator^=(const fixed& i) { val ^= i.val; return *this; }
	fixed& operator+=(const fixed& i) { val += i.val; return *this; }
	fixed& operator-=(const fixed& i) { val -= i.val; return *this; }
	fixed& operator*=(const fixed& i);
	fixed& operator/=(const fixed& i);

	fixed operator&(const fixed& a) { fixed res = a; res &= *this; return res; }
	fixed operator|(const fixed& a) { fixed res = a; res |= *this; return res; }
	fixed operator^(const fixed& a) { fixed res = a; res ^= *this; return res; }
	fixed operator+(const fixed& a) { fixed res = a; res += *this; return res; }
	fixed operator-(const fixed& a) { fixed res = a; res -= *this; return res; }
	fixed operator*(const fixed& a) { fixed res = a; res *= *this; return res; }
	fixed operator/(const fixed& a) { fixed res = a; res /= *this; return res; }

	operator int(void) const { return(parts.in); }
	operator long(void) const { return(val); }
private:
	union
	 {
		long val;
		struct
		 {
			uint frac;					// fractional portion
			int in;						// integer portion(signed)
		 } parts;
	 };
};

//==============================================================================

#endif

//==============================================================================
