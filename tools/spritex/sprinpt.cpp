//==============================================================================
// sprinpt.cpp
//==============================================================================

//==============================================================================
// pclib
#include <pclib\general.h>
#include <pclib\iff.h>
#include <pclib\grphport.h>

// local
#include "menu.hpp"
#include "sprite.hpp"

#include <string.h>

//==============================================================================

#include "utypes.hpp"
#include "global.hpp"
#include "output.hpp"
#include "input.hpp"
#include "gadget.hpp"
#include "spmain.hpp"
#include "image.hpp"

#include "sprite.hpp"

//==============================================================================

//#define DONTNUKE			/* comment this line out to nuke sprites on load */

char IFFName[20] = "test.lbm";
char OutputFile[20] = "test.chr";
char MapFile[20] = "test.map";

// global switches

void
ChangeExtension(char *string,char *newExt)
{
	int i;
	while(*string != 0 && *string != '.')
		string++;
	if (*string == '.')
		strcpy(string,newExt);
}


void GetInput (int argc,char *argv[])
{
static char helpString[] =
"Sprite V0.1 (c) 1991,92,93 Developer Resources.  All Rights Reserved.\n"
"IFF graphic to Genesis Sprite converter\n"
"By Scott Statton\n\n"
"Usage: sprite <input-file> \n\n";

	int i = 0;

	while(*argv[i+1] == '-')
	 {
		switch(*(argv[i+1]+1))
			{
			case '?':
				printf(helpString);
				exit( 0 );
			}

		i++;
	 }

	 return;

}

extern char sprLoadName[];
extern struct _gadget spGadgetBase;

void
LoadSprite(void)
{
	FILE *fp;
	struct _lSprite LS;
	struct _hSprite HS;
	int i,j;
	int ii,jj,ss;

	long filelen;
	byte xxx[] = {  8,  8,  8,  8, 16, 16, 16, 16, 24, 24, 24, 24, 32, 32, 32, 32 };
	byte yyy[] = {  8, 16, 24, 32,  8, 16, 24, 32,  8, 16, 24, 32,  8, 16, 24, 32 };


	fp = fopen (sprLoadName,"rb");
	if (!fp)
		return;
	fseek( fp, 0L , 2 );
	filelen = ftell(fp);
	fseek( fp, 0L, 0 );

#ifndef DONTNUKE
	KillMultipleGadgets(&spGadgetBase);
	KillAllSprites();
#endif
	fread(&LS, sizeof(LS), 1, fp);
	LS.xOffset = Swab(LS.xOffset);
	LS.yOffset = Swab(LS.yOffset);
	LS.xSize = Swab(LS.xSize);
	LS.ySize = Swab(LS.ySize);
	LS.spriteCount = Swab(LS.spriteCount);

	if ( filelen != LS.spriteCount * sizeof(HS)  + sizeof(LS) )
		return;

	for ( i = 0, j = LS.spriteCount ; i < j ; i ++ ) {
		fread(&HS, sizeof(HS), 1, fp);
		jj = Swab(HS.yOffset) + LS.yOffset;
		ii = Swab(HS.xOffset) + LS.xOffset;
		ss = HS.size & 0xf;
		graphic.upLeftX = LS.xOffset;
		graphic.upLeftY = LS.yOffset;
		graphic.lowRightX = LS.xSize + graphic.upLeftX;
		graphic.lowRightY = LS.ySize + graphic.upLeftY;
		graphic.midFlag = boolean::TRUE;
		DrawPermSpritePane( ii, jj,xxx[ss], yyy[ss]);
		CreateGadget( &spGadgetBase, ii  , jj, xxx[ss], yyy[ss],
			0, 0, (void far *)(CreateSprite(ii,jj,xxx[ss],yyy[ss] )), NULL, NULL, InsideSpriteRoutine );
		}
	fclose(fp);
	RefreshImage();
	return;
}
