
#include <iostream.h>
#include <iomanip.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef unsigned char UBYTE;
typedef signed char BYTE;
typedef unsigned short UWORD;
typedef signed short WORD;
typedef unsigned long ULONG;
typedef signed long LONG;
typedef unsigned int BOOL;


static UBYTE* lpInBuffer;

static unsigned
DecodeChar( void )
	{
	int c;

	c = *lpInBuffer;  ++lpInBuffer;
	if ( c == 255 )
		{
		c = *lpInBuffer + 255;  ++lpInBuffer;
		}

	return c;
	}

#define DECODE_CHAR		DecodeChar

ULONG
LZSDecode( UBYTE* inbuffer, UBYTE* outbuffer, ULONG )
	{
	ULONG length;
	int c;

	lpInBuffer = inbuffer;

	length  = DECODE_CHAR() << 8;
	length |= DECODE_CHAR();
	cout << length << " bytes" << endl;

	for ( int i=length; i>0; )
		{
		if ( (c=DECODE_CHAR() ) < 256 )
			{
			*outbuffer++ = c;
			--i;
			}
		else
			{
			int _length, _pos;

			if ( c == 256 )
				{
				_length = DECODE_CHAR() + 4;
				_pos = DECODE_CHAR();
				}
			else if ( c == 257 )
				{
				_length = DECODE_CHAR() + 4;
				_pos  = DECODE_CHAR() << 8;
				_pos |= DECODE_CHAR();
				}
			else if ( c == 258 )
				{
				_length  = DECODE_CHAR() << 8;
				_length |= DECODE_CHAR();
				_length += 259;
				_pos = DECODE_CHAR();
				}
			else if ( c == 259 )
				{
				_length  = DECODE_CHAR() << 8;
				_length |= DECODE_CHAR();
				_length += 259;
				_pos  = DECODE_CHAR() << 8;
				_pos |= DECODE_CHAR();
				}

			UBYTE* lpPrev = outbuffer-_pos;
			for ( ; _length-- > 0; )
				{
				*outbuffer++ = *lpPrev++;
				}

		    i -= _length;
			}
		}
	return length;
	}
