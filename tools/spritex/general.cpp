//==============================================================================
// general.cpp: old sprite stuff
//==============================================================================
// new includes

// pclib
#include <pclib\general.h>
#include <pclib\iff.h>
#include <pclib\grphport.h>

// local
#include "menu.hpp"
#include "sprite.hpp"

//==============================================================================

#include <process.h>
#include <stdio.h>
#include <alloc.h>
#include <dos.h>
#include <string.h>

#include "utypes.hpp"
#include "sq.hpp"
#include "general.hpp"

//==============================================================================

void
truncate (char *s, int l)
{
	int	i = 0;

	while (s && *s++)
		if (!(--l)) *s=0;

	return;
}

void Swap (int *a,int *b)
{
    int c;
    c = *a;
    *a = *b;
    *b = c;
}

void Error (char *a)
{
	delete display, display=NULL;
//    SQ_set_mode (3);
    printf ("%s",a);
    Beep ();
    Quit ();
}



static ushort offset = 0;
static uchar far *mono_base = (uchar far *)0xB0000000;

void DebugChar( char c )
{
    mono_base[offset++] = c;
    mono_base[offset++] = 7;
    offset %= 4000;
}

void Debug( char *message )
{
    while (*message)
        DebugChar(*message++);
    mono_base[offset] = 127;
    mono_base[offset+1] = 135;
}



void
PadString(unsigned char *buffer,int offset)
{
	int i;
	i = 0;
	while(*buffer)
	 {
		buffer++;
		i++;
	 }

	while(i<offset)					// kts note: was <=
	 {
		*buffer++ = ' ';
		i++;
	 }
	*buffer = 0;
}

void FarMemCpy (void far *to, void far *from, unsigned length)
{
    asm{
        lds si,from
        les di,to
        mov cx,length
        rep movsb
    }
}

void
UnPadString(unsigned char *buffer)
{
	while(*buffer)
		buffer++;

	buffer--;
	while(*buffer == ' ')
		buffer--;

	buffer++;
	*buffer = 0;
}


int
UnPaddedStrlen(unsigned char *buffer)
{
	int	i = 0;

	while(*buffer) {
		buffer++;
		i++;
	}

	buffer--; i--;
	while(*buffer == ' ') {
		buffer--;
		i--;
	}

	buffer++; i++;
	return i;
}

char *
DupString(char *s)
{
	char *t;

	t = (char *)malloc(strlen(s)+1);
	strcpy(t,s);
	return t;
}


unsigned char *
FindChar(char *sourceString,unsigned char c)
{
	while(*sourceString)
	 {
		if(*sourceString == c)
			break;
		sourceString++;
	 }
	return(sourceString);
}
