
#include <stdio.h>
#include "ljmain.h"

#if MSDOS

void set_binary_mode( FILE *fptr )
	{
	int handle;
	union REGS r;

	handle = fileno( fptr );

	r.h.ah = 0x44;
	r.h.al = 0x00;
	r.x.bx = handle;
	r.x.dx = 0;
	int86( 0x21, &r, &r );

	if ( r.h.dl & 0x80 )
		{
		if ( !(r.h.dl & 0x20) )
			{
			r.h.ah = 0x44;
			r.h.al = 0x01;
			r.x.bx = handle;
			r.h.dh = 0x00;

			r.h.dl |= 0x20;

			int86( 0x21, &r, &r );
			}
		}
	}
#endif

FILE *fopen_laserjet( char* filename )
	{
	FILE *fp;

	fp = fopen( filename, "wb" );

#if MSDOS
	if ( fp != NULL )
		set_binary_mode( fp );
#endif

	return( fp );
	}

