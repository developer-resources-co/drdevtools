/*
 * sprite.h -- Manifest constants for all sprite routines.
 */

#include <gentypes.h>
#include <alexdef.h>

#ifndef _SPRITE_H_
#define _SPRITE_H_


enum
{
    SPRITE_XFlip = 0x800,
    SPRITE_HFlip = 0x800,
    SPRITE_YFlip = 0x1000,
    SPRITE_VFlip = 0x1000,
    SPRITE_Pal0 = 0,
    SPRITE_Pal1 = 0x2000,
    SPRITE_Pal2 = 0x4000,
    SPRITE_Pal3 = 0x6000,
    SPRITE_Priority = 0x8000,
	SPR_DESC_BASE = 0xd000,		/* sprite descriptor table location */
    VDP_TEXT_LOCATION = 0xd280,
	VDP_TOPFREE	= 0xc000    /* base of Scroll Map A */
};

void InitSprites(short firstChar);
void ClearSpriteTable(void);
void CopySprites(short addr);

void AddLSprite(void *pSpriteStruct,unsigned int x,unsigned int y,
		unsigned int nSpriteDescriptor);

void AddLSpriteBase(void *pSpriteStruct,unsigned int x,unsigned int y,
		unsigned int nSpriteDescriptor, UWORD base);



#endif /* _SPRITE_H_ */

