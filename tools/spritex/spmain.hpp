/*
 * SPMAIN.hpp -- Header for SPMAIN.C
 */

#define ACTIVE_SCREEN_RIGHT 254

/*
 * prototypes found in SPMAIN.C
 */

void HilightClippedRegion(short x, short y, short sizex, short sizey, short left,
	short right, short top, short bottom, word mask);
void UnHilightClippedRegion(short x, short y, short sizex, short sizey, short left,
	short right, short top, short bottom, word mask);
void HilightRegion(short x, short y, short sizex, short sizey, word mask);
void UnHilightRegion(short x, short y, short sizex, short sizey, word mask);
void DrawPermSpritePane(short x, short y, short sizex, short sizey);
boolean DoMainLoop(void);
void UnDrawSpritePane(void);
void DoInput(struct _input *in);
void QuitRoutine(struct _gadget *, struct _input *);
void ScreenRoutine(struct _gadget *, struct _input *);
void MakeSpriteRoutine(struct _gadget *, struct _input *);
void InsideSpriteRoutine(struct _gadget *, struct _input *);

struct PANE {
	boolean active;
	short	x;
	short	y;
	short	sizex;
	short	sizey;
	short	prevx;
	short	prevy;
	struct _gadget *gPtr;
	struct PANE *next;
	struct PANE *prev;
	};

struct GRID {
	boolean flag;
	short	sizeX;
	short	sizeY;
	short	offsetX;
	short	offsetY;
	};

struct GRAPHIC {
	boolean flag;
	char far	*imagePtr;
	short	offsetX;
	short	offsetY;
	short	sizeX;
	short	sizeY;
	short	upLeftX;
	short	upLeftY;
	short	lowRightX;
	short	lowRightY;
	boolean midFlag;
	};

struct VIEWPORT {
	short	sizeX;	/* hardware size ... don't meddle */
	short	left;
	short	right;
	short	top;
	short	bottom;	/* clip rectangle */
	};

extern boolean quitboolean;
extern struct _input inBase;
extern boolean spriteboolean;

extern struct PANE spriteBase;

extern struct GRAPHIC graphic;


#include <pclib/general.h>
#include <pclib/iff.h>
#include <pclib/iffilbm.h>
#include <pclib/iffpbm.h>
#include <pclib/iffanim.h>

typedef struct 
	{
	ilbmCMAP cmap;
	ilbmBMHD bmhd;
	ilbmCAMG camg;
	unsigned char far* image[1000];
//	animANHD anhd;
	} Animation;

extern Animation myPicture;


struct PANE *CreateSprite(short x, short y, short sizex, short sizey );

void
KillAllSprites(void);

//==============================================================================

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
