//=============================================================================
// mapinpt.c:
//=============================================================================

#include "conio.h"
#include "\gfxtools\lib\global.h"
#include "\gfxtools\lib\general.h"
#include "\gfxtools\lib\gui.h"
#include "guifunc.h"
#include "dos.h"
#include "stdio.h"
#include "string.h"
#include <dir.h>
#include "maped.h"

//=============================================================================

char usageText[] =
{
	"Maped "
	MAPED_VERSION
	" (c) 1991-1992 Developer Resources\n\
Character Map Editor\nBy Lars Norpchen, Kevin T. Seghetti, and William B. Norris IV\n\n\
Format: Maped <switches> ( <mapfile> <charfile> <palfile> )...\n\
Switches:\n\
	-x: Set default X size(currently 280)\n\
	-y: Set default Y size(currently 25)\n\
	-c: Enable clipping of saved image\n"
};

//------------------------------------------------------------------------------

void Usage( void )
{
	printf(usageText);
}

//=============================================================================

