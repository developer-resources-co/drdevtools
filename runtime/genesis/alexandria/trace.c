#include <stdlib.h>
#include <stdio.h>

#include "text.h"



typedef unsigned short ushort;
typedef unsigned long ulong;

void
Trace(ulong d0, ulong d1, ulong d2, ulong a0, ulong a1, ushort sr, ulong pc)
{
	int	i;
	i = SaveCursor();
	RestoreCursor(0x160002);
	printf("SR: %4X PC: %8X",sr,pc);
	RestoreCursor(i);
}
