/*============================================================================*/
/* cdebug.h: header file for cdebug.c
/*============================================================================*/

#ifndef ALEXLIB_CDEBUG_H
#define ALEXLIB_CDEBUG_H

void ColorCycle(short baseNum);

void Crash(char *text);

void DumpMemory( void* p, int count, int size );


static const unsigned short palBreakpointText[1] = { 0xFFF };

#define Breakpoint( msg ) \
	do { \
		SetPalette( 8,1,palBreakpointText );\
		RestoreCursor( 0x00190000 );\
		printf( "%s(%d): %-40s", __FILE__, __LINE__, msg );\
		while ( !ReadJoy1() ) ;\
		ButtonRelease();\
	} while ( 0 )

#endif

/*============================================================================*/
