//==============================================================================
// math.cpp:
//==============================================================================

#include <pclib/math.hpp>

//==============================================================================

fixed&
fixed::operator*=(const fixed& i)
{
	long a = val;
	long b = i;

	int hia,hib;
	unsigned int lowa,lowb;
	unsigned long lowsum;
	unsigned long hisum,bd,ad,cb,ca;
	long result,overflow;
	int neg;

	neg = 0;

	if(a < 0)
	 {
		a = 0-a;
		neg = 1;
	 }
	if(b < 0)
	 {
		b = 0-b;
		if(neg)
			neg = 0;
		else
		neg = 1;
	 }

	hia = a >> 16;
	hib = b >> 16;
	lowa = a & 65535;
	lowb = b & 65535;
	bd = (long) lowa * lowb;
	ad = (long) hia  * lowb;
	cb = (long) lowa * hib ;
	ca = (long) hia  * hib ;
	lowsum = bd + (ad << 16) + (cb << 16);
	overflow = (((bd>>2)+ (0x3fff0000&(ad << 14)) + (0x3fff0000&(cb << 14))) >> 30);
	hisum	= ca + (ad >> 16) + (cb >> 16);
	hisum += overflow;
	result = (hisum<<16)+(lowsum>>16);
	if(neg)
		result = 0 - result;
	val = result;
	return(*this);
}

//==============================================================================

fixed&
fixed::operator/=(const fixed& in)
{
	long x = val;
	long y = in;
    long result;
    int a,c;
    uint b,d;

    a = x >> 16;
    b = x & 0xffff;
    c = y >> 16;
    d = y & 0xffff;

    if(c)
     {
		int k,l,r,r1;
		k = y/(c*4);
		l = x/k;
		r = x%k;
		result = ((long)l / (c * 4))<<16;
		r1 = ((long)l % ((long)c * 4));
		result += ((((long)r<<16)/((long)k))+((long)r1<<16))/(c*4);
     }
    else
     {
		result = (x / d)<<16;
		result += (((x % d)<<16)/d);
     }

	val = result;
    return(*this);
}

//==============================================================================



