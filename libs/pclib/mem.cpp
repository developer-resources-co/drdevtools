//==============================================================================
// mem.cpp: bad memory management routines
//==============================================================================

#include <stdio.h>
#include <string.h>

#include <stdlib.h>

#include <pclib/general.hpp>

//==============================================================================

#if 0
void SetGfxMode( int mode_num )
{
    asm{
        MOV AX, mode_num
        SUB AH, AH
        INT 10H
    }
}
#endif

//==============================================================================

void MemError (unsigned long size,char type,unsigned long left)
{
    SetGfxMode(3);
    printf ("Memory error:\n%ld bytes\n",size);
    if (type) printf (" of far ");
         else printf (" of reg ");
    printf ("memory...\n%ld bytes available...\n\n",left);
    Beep ();
    Quit ();
}

//==============================================================================

char *SafeMalloc (unsigned size)
{
 char *a;
 a = (char *)malloc (size);
 if (a==NULL) MemError (size,0,(long) coreleft ());
 while(size)
	*(a+(--size)) = 0;
 return a;
}

void far *
	SafeFarMalloc (unsigned long size)
	{
	void far *a;

	a = (void far *) farmalloc (size);
	if (a==NULL) MemError (size,1,farcoreleft ());
	_fmemset( a, 0, size );

	return( a );
	}

//==============================================================================
