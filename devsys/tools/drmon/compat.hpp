
#ifndef COMPAT_HPP
#define COMPAT_HPP

#ifdef __BORLANDC__
#include <malloc.h>
#include <dir.h>
#else

#define clrscr()
#define gotoxy( x, y )

#endif


#if defined( __WATCOMC__ ) || defined( __SC__ ) || defined( __OS2__ )
#include <conio.h>
#define inportb inp
#define outport  outp
#define outportb outp
#endif

#endif

