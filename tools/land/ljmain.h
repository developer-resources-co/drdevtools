
#ifndef LJMAIN
#define LJMAIN

#ifndef MSDOS
#if (__MSDOS__)
#define MSDOS 1
#endif
#endif

#if MSDOS
#include <dos.h>
#undef UNIX
#else
#define UNIX 1
#endif

#pragma pack(1)

#define ESC	0x1B
#define FormFeed 0x0C

#define ON 1
#define OFF 0

#define YES 1
#define NO 0

#define POINTS 1
#define DECIPOINTS 2
#define COLUMNS 4
#define ROWS 8

#define TOP 'T'
#define BOTTOM 'B'
#define LEFT 'L'
#define RIGHT 'R'

#define HORIZ 'H'
#define VERT 'V'

#endif




