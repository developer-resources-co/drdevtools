/*============================================================================*/
/* spanim.c: animated sprite handler */
/*============================================================================*/
#include "gentypes.h"
#include "spanim.h"
#include "msprite.h"
#include "vdp.h"
#include "render.h"
#include "alexdef.h"

GLOBAL	struct _Camera *pSpriteCamera;

GLOBAL	TMapSpriteHeader	oMSH[];

/*
 * spanim:  Sprite Animation.
 * (c) 1992 Alexandria, Inc.  By Scott Statton
 */

/*--------------------------------------------------------------------------*/

PUBLIC void
SetAnimation(Anim *pAnim, unsigned short nAnimToPlay)
{

	pAnim->nAnimNumber = nAnimToPlay;
	pAnim->nFrameNumber = 0;
	pAnim->nPrevFrameNumber = -1;
	pAnim->nNumFrames = pAnim->oAnims[nAnimToPlay]->nNumFrames;
	pAnim->fAnimComplete = FALSE;
	pAnim->xAnimSize = pAnim->oAnims[nAnimToPlay]->frame->ls->xSize;
	pAnim->yAnimSize = pAnim->oAnims[nAnimToPlay]->frame->ls->ySize;
	pAnim->wPalette = pAnim->oAnims[nAnimToPlay]->nPalette;
    pAnim->nFrameRate = pAnim->oAnims[nAnimToPlay]->frame->nDelay;
    pAnim->nLoopAnim = TRUE;
    pAnim->nCurrentFrame = 0;
#if 0
    pAnim->xOffset = pAnim->oAnims[nAnimToPlay]->frame->xOffset;
    pAnim->yOffset = pAnim->oAnims[nAnimToPlay]->frame->yOffset;
#endif
}

/*--------------------------------------------------------------------------*/


PUBLIC void
SafeSetAnimation(Anim *pAnim, unsigned short nAnimToPlay)
{
	if (pAnim->nAnimNumber != nAnimToPlay)
		SetAnimation(pAnim, nAnimToPlay);
}

/*============================================================================*/

PUBLIC UWORD
GetLargestAnimFrame(const TSpriteAnim 	**anims )
{
	int i,j;
	short temp,numChars = 0,numFrames;
	const TSpriteAnim *sequence;
	const TSpriteAnimFrame *frame;

	for(i=0; sequence= anims[i]; i++)
	 {
		sequence = anims[i];
		numFrames = sequence->nNumFrames;
		for(j=0;j<numFrames;j++)
		 {
			frame = sequence->frame+j;

		    temp = CountSpriteChars(frame->ls);
			if(temp > numChars)
				numChars = temp;
		 }
	 }
	return(numChars);
}

/*--------------------------------------------------------------------------*/
