// mape.cpp

#include <string.h>

typedef unsigned char UBYTE;
typedef unsigned long ULONG;
typedef unsigned int BOOL;

ULONG CopyEncode( UBYTE* inbuffer, UBYTE* outbuffer, ULONG inlength )
	{
	memcpy( outbuffer, inbuffer, inlength );
	return inlength;
	}
