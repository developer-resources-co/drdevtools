//=============================================================================
// guidefs.c
//=============================================================================

//=============================================================================
#include "\gfxtools\lib\keys.h"
#include "\gfxtools\lib\global.h"
//#include "\gfxtools\lib\input.h"
#include "\gfxtools\lib\gfx.h"
#include "\gfxtools\lib\general.h"
#include "\gfxtools\lib\mouse.h"
#include "\gfxtools\lib\gui.h"
#include "\gfxtools\lib\t.h"

#include "guidefs.h"
#include "guifunc.h"

#include <mem.h>

int
	CalcUsageCount( MAP *mapPtr )
	{
	int i;

//	if ( mapPtr->usage ) free( mapPtr->usage );

//	if ( mapPtr->usage = malloc( chars * sizeof(uint) ) )
		{
		int x, y, tile;

		for (i=0; i<chars; ++i)
			*(mapPtr->usage+i) = 0;

		for (i=0, y=0; y<mapPtr->ySize; ++y)
			for (x=0; x<mapPtr->xSize; ++x, ++i)
				{
				tile = *( mapPtr->buffer + (mapPtr->xSize * y) + x ) & CHARMASK;
				++mapPtr->usage[tile];
//				++*(mapPtr->usage + tile);
				}
		}
	return( i );
	}
