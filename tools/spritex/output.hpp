/*
 * output.h - header file for output.c
 * (c) 1991 Developer Resources, Scott Statton
 */


struct _hSprite
{
	short yOffset;					/* from corner of lSprite */
	byte size;						/* 1x1 thru 4x4 */
	byte pad;						/* I can ignore this one */
	short startChar;					/* keep this running up */
	short xOffset;					/* from corner of lSprite */
};

struct _lSprite
{
	short	xOffset,yOffset;		/* from upper left of original iff image */
	short	xSize,ySize;			/* of logical sprite(for flipping) */
	short	hardXOffset,hardYOffset; /* from corner of lsprite */
	short	hardXSize, hardYSize;	/* based on top/bottom/left/right */
	short	spriteCount;			/* # of hSprite structs following */
};


struct _lSprite *
CreateLogicalSprite(void);

void
SaveRoutine(struct _gadget *, struct _input *);

word
Swab(word);