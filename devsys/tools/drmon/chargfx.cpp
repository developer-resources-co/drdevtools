
#include <dos.h>

#if 0
main( int argc, char *argv[] )
	{
	WrFont( (void _far *)main, 0, 64 );
	}
#endif


void
	WrFont( void _far *data, int nBlock, int nLine )
	{
	_ES = FP_SEG( data );
	_BP = FP_OFF( data );
	
	_AX = (0x11 << 8) | 0x10;

	_BL = nBlock;
	_BH = nLine;

	_CX = 1;					// Just 1 character 0x100;
	_DX = 0;

	geninterrupt( 0x10 );
	}
	
