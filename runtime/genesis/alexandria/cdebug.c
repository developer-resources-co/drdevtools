/*============================================================================*/
/* debug.c: debugging functions, created 02-11-93 11:01pm					  */
/* By Kevin T. Seghetti                                                       */
/*============================================================================*/

#include <vdp.h>

/*============================================================================*/

void
ColorCycle(short baseNum)
{
	static short col;

	SetPalette(baseNum,1,&col);
	col++;
}

void
	DumpMemory( void* p, int count, int size )
	{
	int i;
	unsigned long* pp = (unsigned long*)p;
	unsigned short white = 0xFFF;

	SetPalette( 8,1,&white );
	RestoreCursor( 0x00000000 );

	for ( i=0; i<size; ++i )
		printf( "%08lx ", *pp++ );

	while ( !ReadJoy1() ) ;
	ButtonRelease();
	}


/*============================================================================*/
