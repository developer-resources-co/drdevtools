/* Sprite Convertor Main Loop
 * (c) 1991, Developer Resources
 * Written by Scott Statton
 */
// pclib
#include <pclib\general.h>
#include <pclib\iff.h>
#include <pclib\grphport.h>

// local
#include "menu.hpp"
#include "sprite.hpp"


#include "global.hpp"
#include "utypes.hpp"
#include "keys.hpp"
#include	"localkey.hpp"
#include	"sprite.hpp"
#include "input.hpp"
#include	"sq.hpp"
#include	"gadget.hpp"
#include	"spgadget.hpp"
#include	"mouse.hpp"
#include	"spmain.hpp"
#include	"image.hpp"

void LoadSPRRoutine(struct _gadget *, struct _input *);
void LoadIFFRoutine(struct _gadget *, struct _input *);
void NukeRoutine(struct _gadget *, struct _input *);
void SaveRoutine(struct _gadget *, struct _input *);
void ShowInfoRoutine(struct _gadget *gPtr, struct _input *in);

void DoInput(struct _input *);

extern struct PANE spriteBase;
extern struct PANE pane;
extern struct GRID grid;
extern struct GRAPHIC graphic;


struct PANE *sprites;

struct VIEWPORT subScreen = { 320, 0, 254, 8, 199  } ;

struct _gadget spGadgetBase = { NULL, NULL, 0, 0, 0, 0, 0, 0, NULL, NULL, NullRoutine , NULL } ;

boolean justOnceboolean;
boolean spriteBorder;
boolean chrNameboolean;

void
KillAllSprites(void)
{
	struct PANE *pPtr = &spriteBase;
	while ( pPtr->next ) pPtr = pPtr->next; /* find tail */
	while ( pPtr->prev) {
		pPtr = pPtr->prev;
		farfree(pPtr->next);
		pPtr->next = NULL;
		}
}

void
HilightClippedRegion(short x, short y, short sizex, short sizey, short left,
	short right, short top, short bottom, word mask)
{
	char far *sp,*sp2;
	int i,j;

	if ( x < left ) x = left;
	if ( x+sizex > right ) sizex = right - x;

	if ( y < top ) y = top;
	if ( y + sizey > bottom ) sizey = bottom - y;

	if ( sizey <= 0 || sizex <= 0 )
		return;

	HilightRegion(x , y, sizex, sizey, mask);
}

void
UnHilightClippedRegion(short x, short y, short sizex, short sizey, short left,
	short right, short top, short bottom, word mask)
{
	char far *sp,*sp2;
	int i,j;

	if ( x < left ) x = left;
	if ( x+sizex > right ) sizex = right - x;

	if ( y < top ) y = top;
	if ( y + sizey > bottom ) sizey = bottom - y;

	if ( sizey <= 0 || sizex <= 0 )
		return;

	UnHilightRegion(x , y, sizex, sizey, mask);
}

void
HilightRegion(short x, short y, short sizex, short sizey, word mask)
{
	char far *sp,far *sp2;
	int i,j;
	HideMouse();
	sp = sp2 = SQ_get_screen_address( (int) x, (int) y);
	for ( i = 0 ; i < sizey ; i++ )
	{
		for ( j = 0 ; j < sizex ; j++ )
			*sp++ = (*sp) | mask;
		sp = (sp2 += 320);

	}
	ShowMouse();
}

/***************************************************************************/


void
UnHilightRegion(short x, short y, short sizex, short sizey, word mask)
{
	char far *sp,*sp2;
	int i,j;
	HideMouse();
	for ( i = 0 ; i < sizey ; i++ )
	{
		sp = SQ_get_screen_address( (int) x, (int) y + i );
		for ( j = 0 ; j < sizex ; j++ )
			*sp++ = (*sp) & (~mask);
	}
	ShowMouse();
}


void
UnDrawSpritePane(void)
{
	if ( pane.active )
		UnHilightClippedRegion(pane.x, pane.y, pane.sizex, pane.sizey,
			0, 254, 0, 199,16 );
	pane.active = boolean::FALSE;
}

void
DrawSpritePane(short x, short y )
{
	pane.prevx = pane.x;
	pane.prevy = pane.y;
	pane.x = x;
	pane.y = y;
	HilightClippedRegion(pane.x, pane.y, pane.sizex, pane.sizey,
			0,254,0,199, 16 );
	pane.active = boolean::TRUE;
}

void
DrawPermSpritePane(short x, short y, short sizex, short sizey)
{
	HilightClippedRegion(x, y, sizex, sizey,
			0,254,0,199, 32 );
	if (spriteBorder)
		ClipBox( x, y, sizex-1, sizey-1, &subScreen, 0xfe );
}

void
UnDrawPermSpritePane(short x, short y, short sizex, short sizey)
{
	UnHilightClippedRegion(x, y, sizex, sizey, 0, 254, 0, 199, 32 );
}

struct PANE *
CreateSprite(short x, short y, short sizex, short sizey )
{
	struct PANE *pPtr;

	pPtr = &spriteBase;

	while ( pPtr->next ) pPtr = pPtr->next;

	pPtr->next = (struct PANE *)malloc((unsigned long) sizeof (struct PANE) );
	if ( pPtr->next )
	{
		pPtr->next->prev = pPtr;
		pPtr = pPtr->next;
		pPtr->x = x;
		pPtr->y = y;
		pPtr->sizex = sizex;
		pPtr->sizey = sizey;
		pPtr->next = NULL;
		return pPtr;
	}
	return NULL;
}

boolean
DoMainLoop(void)
{
	if(InputPending(&inBase))
		DoInput(&inBase);
	return !(quitboolean);
}

void
kbSpriteSize(short identity)
{
	struct _gadget *gPtr;

	gPtr = FindGadget(&gadgetBase,identity);
	if (gPtr)
		NewSpriteSizeRoutine(gPtr, (struct _input *)NULL );
}

#define BLACK 0xE0
#define WHITE 0xEF

typedef short sprpoint;

#define UPLEFT 1
#define LOWRIGHT 2

void
cpDoInput(sprpoint p)
{
	boolean cpQuitboolean = boolean::FALSE;

	short *markerX, *markerY;
	char far * sa;

	if ( p == UPLEFT ) {
		markerX = &graphic.upLeftX;
		markerY = &graphic.upLeftY;
	}

	if ( p == LOWRIGHT ) {
		markerX = &graphic.lowRightX;
		markerY = &graphic.lowRightY;
	}
	if (graphic.flag ) {
		int tempx = graphic.upLeftX - graphic.offsetX;
		int tempy = graphic.upLeftY;
		int tempx1 = graphic.lowRightX - graphic.offsetX -1;
		int tempy1 = graphic.lowRightY-1;

		ClipXorBox(tempx, tempy, tempx1-tempx, tempy1-tempy, &subScreen );
		}

	while (!cpQuitboolean) {
		if(InputPending(&inBase))
			switch(inBase.inputType) {
				case INP_MOUSEMOVE:
				if (graphic.flag ) {
					int tempx = graphic.upLeftX - graphic.offsetX;
					int tempy = graphic.upLeftY;
					int tempx1 = graphic.lowRightX - graphic.offsetX -1;
					int tempy1 = graphic.lowRightY-1;

					ClipXorBox(tempx, tempy, tempx1-tempx, tempy1-tempy, &subScreen);
					}
					*markerX = inBase.mouseX;
					*markerY = inBase.mouseY;
					if (graphic.flag ) {
						int tempx = graphic.upLeftX - graphic.offsetX;
						int tempy = graphic.upLeftY;
						int tempx1 = graphic.lowRightX - graphic.offsetX -1;
						int tempy1 = graphic.lowRightY-1;

						ClipXorBox(tempx, tempy, tempx1-tempx, tempy1-tempy, &subScreen);
						}
					break;
				case INP_MOUSE_LEFTBUTTON_DOWN:
				case INP_MOUSE_RIGHTBUTTON_DOWN:
					if (graphic.flag ) {
						int tempx = graphic.upLeftX - graphic.offsetX;
						int tempy = graphic.upLeftY;
						int tempx1 = graphic.lowRightX - graphic.offsetX -1;
						int tempy1 = graphic.lowRightY-1;

						ClipXorBox(tempx, tempy, tempx1-tempx, tempy1-tempy, &subScreen);
						}
					cpQuitboolean = graphic.midFlag = boolean::TRUE;
					break;
			}
	}
}


void
CenterPointRoutine(struct _gadget *gPtr, struct _input *in)
{
	HideMouse();
	graphic.midFlag = boolean::FALSE;
	RefreshImage();
	switch(in->inputType) {
		case INP_MOUSE_LEFTBUTTON_DOWN:
			cpDoInput(UPLEFT);
			break;
		case INP_MOUSE_RIGHTBUTTON_DOWN:
			cpDoInput(LOWRIGHT);
			break;
	}
	graphic.midFlag = boolean::TRUE;
	RefreshImage();
	ShowMouse();
}

void GridRoutine(struct _gadget *gPtr, struct _input *)
{
	grid.flag = !grid.flag;
//	grid.flag = ( grid.flag ? boolean::FALSE : boolean::TRUE );

	if ( gPtr && grid.flag )
		HilightRegion ( gPtr->offsetX, gPtr->offsetY, gPtr->sizeX, gPtr->sizeY ,16 );
	 else
		UnHilightRegion ( gPtr->offsetX, gPtr->offsetY, gPtr->sizeX, gPtr->sizeY ,16 );
}

void
DoInput(struct _input *in)
{
	short i,j;
	quitboolean = boolean::FALSE;
	switch(in->inputType)
	{
		case INP_KEY:
			HideMouse();
			UnDrawSpritePane();
			switch (in->fullKey)
			{
				case KEY_ALTX:
					quitboolean = boolean::TRUE;
					break;
				case KEY_1X1:
					kbSpriteSize(GAD_PANE_1X1);
					break;
				case KEY_1X2:
					kbSpriteSize(GAD_PANE_1X2);
					break;
				case KEY_1X3:
					kbSpriteSize(GAD_PANE_1X3);
					break;
				case KEY_1X4:
					kbSpriteSize(GAD_PANE_1X4);
					break;
				case KEY_2X1:
					kbSpriteSize(GAD_PANE_2X1);
					break;
				case KEY_2X2:
					kbSpriteSize(GAD_PANE_2X2);
					break;
				case KEY_2X3:
					kbSpriteSize(GAD_PANE_2X3);
					break;
				case KEY_2X4:
					kbSpriteSize(GAD_PANE_2X4);
					break;
				case KEY_3X1:
					kbSpriteSize(GAD_PANE_3X1);
					break;
				case KEY_3X2:
					kbSpriteSize(GAD_PANE_3X2);
					break;
				case KEY_3X3:
					kbSpriteSize(GAD_PANE_3X3);
					break;
				case KEY_3X4:
					kbSpriteSize(GAD_PANE_3X4);
					break;
				case KEY_4X1:
					kbSpriteSize(GAD_PANE_4X1);
					break;
				case KEY_4X2:
					kbSpriteSize(GAD_PANE_4X2);
					break;
				case KEY_4X3:
					kbSpriteSize(GAD_PANE_4X3);
					break;
				case KEY_4X4:
					kbSpriteSize(GAD_PANE_4X4);
					break;
				case KEY_ALTS:
					SaveRoutine(FindGadget(&gadgetBase,GAD_SAVE), NULL);
					break;
				case KEY_ALTL:
					LoadIFFRoutine(FindGadget(&gadgetBase,GAD_LOADIFF), NULL);
					break;
				case KEY_ALTR:
					LoadSPRRoutine(FindGadget(&gadgetBase,GAD_LOADSPR), NULL);
					break;
				case KEY_ALTN:
					NukeRoutine(NULL, NULL);
					break;
				case KEY_ALTB:
				case 'B':
				case 'b':
					spriteBorder = !spriteBorder;
					RefreshImage();
					break;

				// WBN2.0  dpaint-like "grid" key equivalent
				case 'G':
				case 'g':
					GridRoutine( FindGadget( &gadgetBase, GAD_GRID ), NULL );
					break;

				case KEY_ALTT:
				case 'T':
				case 't':
					graphic.midFlag = !graphic.midFlag;
					RefreshImage();
					break;
				case KEY_ALTC:
					SetCharRoutine(FindGadget(&gadgetBase,GAD_SETCHAR), NULL);
					break;
				case KEY_ALTI:
				case 'I':
				case 'i':
					ShowInfoRoutine(FindGadget(&gadgetBase,GAD_SHOWINFO), NULL);
					break;
			}
			if (( in->mouseX < ACTIVE_SCREEN_RIGHT) && spriteboolean )
			{
				i = in->mouseX;
				j = in->mouseY;

				if ( grid.flag)
				{
					i -= i%grid.sizeX;
					i += grid.offsetX;
					j -= j%grid.sizeY;
					j += grid.offsetY;
				}
				DrawSpritePane(i,j);
			}
			ShowMouse();
			break;
		case INP_MOUSE_LEFTBUTTON_DOWN:
		case INP_MOUSE_RIGHTBUTTON_DOWN:
			HideMouse();
			DoHitGadget(gadgetBase.next,in);
			ShowMouse();
			break;
		case INP_MOUSEMOVE:
			HideMouse();
			UnDrawSpritePane();
			if (( in->mouseX < ACTIVE_SCREEN_RIGHT) && spriteboolean )
			{
				i = in->mouseX;
				j = in->mouseY;

				if ( grid.flag)
				{
					i -= i%grid.sizeX;
					i += grid.offsetX;
					j -= j%grid.sizeY;
					j += grid.offsetY;
				}
				DrawSpritePane(i,j);
			}
			ShowMouse();
			break;
	}
}

void
QuitRoutine(struct _gadget *gPtr, struct _input *in)
{
	quitboolean = boolean::TRUE;
}

unsigned char infoText[] =
{
 "         Sprite V2.0 Alpha       \n"
 "By Kevin Seghetti & Scott Statton\n"
 " (c) 1991,93 Developer Resources \n"
 "       All Rights Reserved"
};

void
ShowInfoRoutine(struct _gadget *gPtr, struct _input *in)
{
	MessageBox(&subScreen,239, 255, infoText);
}

void
RightRoutine(struct _gadget *gPtr, struct _input *in)
{
	int size= 0;
	if ( graphic.sizeX <= ( subScreen.right - subScreen.left ) )
		return;

	switch (in->inputType)
	{
		case INP_MOUSE_LEFTBUTTON_DOWN:
			size = 8;
			break;
		case INP_MOUSE_RIGHTBUTTON_DOWN:
			size = graphic.sizeX-subScreen.right;
			break;
		case INP_KEY:
			switch ( in->fullKey)
			{
				case KEY_RIGHT:
					size = 8;
					break;
				case KEY_SHIFTED|KEY_RIGHT:
					size = graphic.sizeX-subScreen.right;
					break;
			}
	}

	graphic.offsetX += size;

	graphic.offsetX = ( graphic.offsetX < (graphic.sizeX-subScreen.right) ? graphic.offsetX : (graphic.sizeX-subScreen.right));
	RefreshImage();
}

void
LeftRoutine(struct _gadget *gPtr, struct _input *in)
{
	int size = 0;
	if ( graphic.sizeX <= ( subScreen.right - subScreen.left ) )
		return;

	switch (in->inputType)
	{
		case INP_MOUSE_LEFTBUTTON_DOWN:
			size = 8;
			break;
		case INP_MOUSE_RIGHTBUTTON_DOWN:
			size = graphic.sizeX-subScreen.right;
			break;
		case INP_KEY:
			switch ( in->fullKey)
			{
				case KEY_LEFT:
					size = 8;
					break;
				case KEY_SHIFTED|KEY_LEFT:
					size = graphic.sizeX-subScreen.right;
					break;
			}
	}
	if ( size < 0 ) size = 0;
	graphic.offsetX -= size;

	graphic.offsetX = (((short) graphic.offsetX) >= 0 ? graphic.offsetX : 0 );
	RefreshImage();
}

void
PickupSprite(struct _gadget *gPtr, struct _input *in)
{
	struct PANE *pPtr;

	pPtr = (struct PANE *)(gPtr->dataPtr1);
	if (pPtr->prev)
		pPtr->prev->next = pPtr->next;
	if (pPtr->next)
		pPtr->next->prev = pPtr->prev;
	pane.sizex = pPtr->sizex;
	pane.sizey = pPtr->sizey;
	pane.x = pPtr->x;
	pane.y = pPtr->y;
	spriteboolean = boolean::TRUE;
	justOnceboolean = boolean::TRUE;
	RefreshImage();
}


void
InsideSpriteRoutine(struct _gadget *gPtr, struct _input *in)
{
	struct PANE *pPtr;


	if (in->inputType == INP_MOUSE_LEFTBUTTON_DOWN)
	{
		if (spriteboolean)
		{
			MakeSpriteRoutine(gPtr, in);
			return;
		}
		else
		{
			PickupSprite(gPtr, in);
			return;
		}
	}

	if ( in->inputType == INP_MOUSE_RIGHTBUTTON_DOWN )
	{
		pPtr = (struct PANE *)(gPtr->dataPtr1);
		if (pPtr->prev)
			pPtr->prev->next = pPtr->next;
		if (pPtr->next)
			pPtr->next->prev = pPtr->prev;
		farfree(pPtr);

		if (gPtr->prev)
			gPtr->prev->next = gPtr->next;
		if (gPtr->next)
			gPtr->next->prev = gPtr->prev;
		farfree(gPtr);
		RefreshImage();
	}
}

void
ScreenRoutine(struct _gadget *gPtr, struct _input *in)
{
	int	i,j;

	in->mouseX += graphic.offsetX;  /* convert mouse to bitmap coordinates */

	if (! DoHitGadget ( spGadgetBase.next , in ))
		MakeSpriteRoutine(gPtr, in);
}

void
MakeSpriteRoutine(struct _gadget *gPtr, struct _input *in)
{
	int	i,j;

	if ( spriteboolean )
	{
		i = in->mouseX;
		j = in->mouseY;
		if ( grid.flag)
		{
			i -= i%grid.sizeX;
			i += grid.offsetX;
			j -= j%grid.sizeY;
			j += grid.offsetY;
		}
		DrawPermSpritePane( i-graphic.offsetX, j, pane.sizex, pane.sizey);
		CreateGadget( &spGadgetBase,  i , j, pane.sizex, pane.sizey,
			0, 0, (void far *)(CreateSprite(i,j,pane.sizex,pane.sizey )), NULL, NULL,  InsideSpriteRoutine );
		if (justOnceboolean)
			spriteboolean = justOnceboolean = boolean::FALSE;
	}
}

void
NukeRoutine(struct _gadget *gPtr, struct _input *in)
{
	KillMultipleGadgets(&spGadgetBase);
	KillAllSprites();
	display->SetColorIndex( 0 );
	display->Box( point(0,0), point(254,199) );
//	SQ_solid_box( 0, 0, 254, 199, 0 );
	RefreshImage();
}
