
#ifndef _ANIMPLAY_H
#define _ANIMPLAY_H

/*
 *  C header file for animplay.asm library
 */

/* --------------------------------------------------------------------------

   pre:
	nField is either FIELD_A or FIELD_B
	pAnimStruct is uninitialized
	pMapPointers is an array of map pointers, zero terminated

   post:
	initializes animation

-------------------------------------------------------------------------- */

extern void InitAnim
	(short xOffset, short yOffset, short nCharBase, short nField,
	 void *pAnimStruct, void *pMapPointers);





/* --------------------------------------------------------------------------

   pre:
	pAnimStruct is initialized

   post:
	performs one tick (frame) of animation

-------------------------------------------------------------------------- */

extern void AnimTick
	(void *pAnimStruct);



#endif

/* EOF -- ANIMPLAY.H */

