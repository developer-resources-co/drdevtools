
#ifndef _EFFECTS_H_
#define _EFFECTS_H_

/*
 *  C header file for effects.asm library
 */

/* --------------------------------------------------------------------------

   pre:
	nCrossFadeValue is a 4 bit value, $0 indicating all source and $f
		indicating all destination
   post:
	sets pResult to a palette which is nCrossFadeValue fades from
		pSourceColors to pDestinationColors

-------------------------------------------------------------------------- */

extern void SetCrossFade
	(short	nCrossFadeValue,	/* current step of fade */
	short	nCrossFadeColors,	/* number of colors to fade */
	void	*pSourceColors,		/* pointer to source colors */
	void	*pDestinationColors,	/* pointer to dest colors */
	void	*pResult);		/* pointer to result */




/* --------------------------------------------------------------------------

   pre:
	nFramesToWait is the number of frames to wait between fade steps - 1
		e.g. for 4 frame delay set this to 3
   post:
	fades from source colors to destination colors

-------------------------------------------------------------------------- */

extern void CrossFade
	(short	nStartingColor,		/* first color to fade from */
	short	nCrossFadeColors,	/* number of colors to fade */
	short	nFramesToWait,		/* frames to delay between fade steps */
	void	*pSourceColors,		/* pointer to source colors */
	void	*pDestinationColors);	/* pointer to dest colors*/



extern unsigned short blackPalette[];

#endif


/* EFFECTS.H -- EOF */

