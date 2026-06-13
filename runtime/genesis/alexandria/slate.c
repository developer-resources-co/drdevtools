/*============================================================================*/
/* slate.c: Alexandria submission slate screen								  */
/* Written By Kevin T. Seghetti	                                              */
/*============================================================================*/

/*============================================================================*/
/* C compiler includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* library includes */
#include <gentypes.h>
#include <vdp.h>
#include <object.h>
#include <sprite.h>
#include <scroll.h>
#include <surfmap.h>
#include <genesis.h>
#include <joy.h>
#include <text.h>
#include <slate.h>

/*============================================================================*/

short slatePal[16] =
{
	0x800,0,0,0,0,0,0,0,0xfff
};

/*============================================================================*/
/*
	call with string pointers to the publisher's name, any cart requirments,
	programmer comments, version struct, and creation date as a string

*/
/*============================================================================*/

UWORD
ShowSlateScreen(char *publisher, char *cartRequirments, char *comments, TVersion *ver, char *date)
{
	UWORD ret;
	char tempBuff[17];

	InitVDP(VDPTable);

	SetPalette(0,9,slatePal);

	InitText(32);

	RestoreCursor(0x40000);
	printf("    Alexandria, Inc.\n");
	strncpy(tempBuff,(char *)0x150L,16);			/* get u.s. title of game */
	tempBuff[16] = '\0';
	printf("    %s\n    Version %d.%d.%d\n",tempBuff,ver->release, ver->milestone, ver->internal);
	printf("    Submitted to: %s\n",publisher);
	printf("    Date: %s\n",date);
	printf("    Milestone #%d\n\n",ver->milestone);
	printf("    Cart Requirements: %s\n\n",cartRequirments);
	printf("    Comments: %s\n",comments);			/* print programmer comments */
	RestoreCursor(0x190000);
	printf("             Press Start\n");
	printf("    Confidential Alexandria, Inc.");

	while ( ! (ret=ReadJoy1() ) )
		/* wait for button hit */;

	return ret;
}

/*============================================================================*/
