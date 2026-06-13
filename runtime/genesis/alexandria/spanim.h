#ifndef _SPANIM_H_
#define _SPANIM_H_

#include <alexdef.h>
#include <gentypes.h>
#include <object.h>

/*
 * Overview:
 *	This function is used to add a mapped sprite.
 */

extern	struct _Camera *pSpriteCamera;

/*--------------------------------------------------------------------------*/

typedef struct
{
		TCharSetData *pCharset;
		TLogicalSpriteData *ls;
		WORD	xOffset, yOffset;
		WORD	nDelay;
} TLSpriteAnimFrame;

typedef struct
{
	WORD nNumChars;
	WORD wCharOffset[1];	/* well ACTUALLY ... */
} TIndexData;

typedef struct
{
	TIndexData *index;
	TLogicalSpriteData *ls;
	WORD	xOffset,yOffset;
	WORD	nDelay;
} TSpriteAnimFrame;

typedef struct
{
		TMapData *map;
		TLogicalSpriteData *ls;
		WORD	xOffset, yOffset;
		WORD	nDelay;
} TOldSpriteAnimFrame;

typedef struct {
		TCharSetData *pCharSet;	/* this is unused for LSprites */
		TSpriteAnimFrame *frame;
        UBYTE   nNumFrames;
		UBYTE	nPalette;       /* 0 - 3 */
        UWORD   nFrameRate;
        TSmallRect rCollision;
} TSpriteAnim;


PUBLIC void 	SetAnimation(struct _Anim *, unsigned short );
PUBLIC void 	SafeSetAnimation(struct _Anim *, unsigned short );
PUBLIC UWORD 	GetLargestAnimFrame(const TSpriteAnim **);

PUBLIC void 	AddAnimatedSprite(short, short, short, const TSpriteAnim *, short);
PUBLIC void 	AddMagicSprite(WORD xPosition, WORD yPosition, struct _Anim *);
PUBLIC void		AddMagicFixedSprite(struct _Render *, struct _Anim *);

PUBLIC void 	RenderSprite(WORD xPosition, WORD yPosition, struct _Anim *);
PUBLIC void		RenderLSprite(WORD xPosition, WORD yPosition, struct _Anim *);
PUBLIC void 	RenderFixedSprite(struct _Render *, struct _Anim *);
PUBLIC void 	RenderSemiPermSprite(struct _Render *, struct _Anim *);

PUBLIC void		*SetRenderSpritePanic(void *);


#endif /* _SPANIM_H_ */

