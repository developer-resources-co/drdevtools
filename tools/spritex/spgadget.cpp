/*
 * SPGADGET.cpp - Sprite Editor specific gadgets
 * (c) 1991, Developer Resources
 * Written By Scott Statton
 *
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
#include "input.hpp"
#include "gadget.hpp"
#include "spgadget.hpp"
#include "spmain.hpp"
#include "image.hpp"
#include "general.hpp"
#include "sprinpt.hpp"
#include "mouse.hpp"

struct _gadget *previousGadget;

extern void QuitRoutine (struct _gadget *, struct _input *);
extern void GridRoutine (struct _gadget *, struct _input *);
extern void RightRoutine (struct _gadget *, struct _input *);
extern void LeftRoutine (struct _gadget *, struct _input *);
extern void ScreenRoutine (struct _gadget *, struct _input *);
extern void SaveRoutine (struct _gadget *, struct _input *);
extern void SaveSprite(void);
extern void NukeRoutine(struct _gadget *, struct _input *);
extern void CenterPointRoutine(struct _gadget *, struct _input *);

extern struct VIEWPORT subScreen;


extern void FileReqRoutine (struct _gadget *, void (*routine)(void));

extern void LoadImage();
extern void LoadSprite();
extern struct _string textBox;
extern boolean chrNameboolean;

/* global variable */

boolean spriteboolean;

extern struct PANE pane;

static char iffLoadText[] 		= "Load IFF Image  ";
static char sprLoadText[]		= "Load Sprite Data";
static char sprSaveText[]		= "Save Sprite Data";
static char setText[]			= "Set Char Name   ";
static char loadButtonText[]	= "LOAD";
static char saveButtonText[]	= "SAVE";
static char setButtonText[]		= " SET";

char iffLoadName[] 				= "TEST.LBM    ";
char sprLoadName[] 				= "SPRITE.DAT  ";
char sprSaveName[] 				= "SPRITE.DAT  ";
char chrSaveName[]				= "SPRITE.CHR  ";

/***************************************************************************/
#define COL1	512
#define COL2	544
#define COL3	576
#define COL4	608

#define ROW1	5
#define ROW2	21
#define ROW3	37
#define	ROW4	53
#define ROW5	69
#define ROW6	85
#define ROW7	101
#define ROW8	117
#define	ROW9	165
#define ROW10	181




struct _multigadget permanentGadgets[] =
 {
	{ COL1, ROW1, 31, 15, GAD_PANE_1X1 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL1, ROW2, 31, 15, GAD_PANE_1X2 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL1, ROW3, 31, 15, GAD_PANE_1X3 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL1, ROW4, 31, 15, GAD_PANE_1X4 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL2, ROW5, 31, 15, GAD_LEFT , GTYPE_HIT , LeftRoutine } ,
	{ COL1, ROW10, 127, 15, GAD_QUIT , GTYPE_HIT , QuitRoutine } ,
	{ COL1, ROW6, 127, 15, GAD_LOADIFF , GTYPE_HIT , LoadIFFRoutine } ,
	{ COL2, ROW1, 31, 15, GAD_PANE_2X1 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL2, ROW2, 31, 15, GAD_PANE_2X2 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL2, ROW3, 31, 15, GAD_PANE_2X3 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL2, ROW4, 31, 15, GAD_PANE_2X4 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL3, ROW5, 31, 15, GAD_GRID , GTYPE_HIT , GridRoutine } ,
	{ COL3, ROW1, 31, 15, GAD_PANE_3X1 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL3, ROW2, 31, 15, GAD_PANE_3X2 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL3, ROW3, 31, 15, GAD_PANE_3X3 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL3, ROW4, 31, 15, GAD_PANE_3X4 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL4, ROW5, 31, 15, GAD_RIGHT , GTYPE_HIT , RightRoutine } ,
	{ COL1, ROW8, 127, 15, GAD_SAVE , GTYPE_HIT , SaveRoutine } ,
	{ COL4, ROW1, 31, 15, GAD_PANE_4X1 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL4, ROW2, 31, 15, GAD_PANE_4X2 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL4, ROW3, 31, 15, GAD_PANE_4X3 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL4, ROW4, 31, 15, GAD_PANE_4X4 , GTYPE_HIT , NewSpriteSizeRoutine } ,
	{ COL1, ROW7, 127, 15, GAD_LOADSPR , GTYPE_HIT , LoadSPRRoutine } ,
	{ COL1, ROW9, 127, 15, GAD_NUKE, GTYPE_HIT, NukeRoutine } ,
	{ COL1, ROW5, 31, 15, GAD_CENTER, GTYPE_HIT, CenterPointRoutine } ,
	{ 0 , 0, 254*2, 199, GAD_MAIN_SCREEN, GTYPE_HIT, ScreenRoutine } ,
	{ COL4, ROW8, 0, 0, GAD_SETCHAR , GTYPE_HIT, SetCharRoutine } ,
	{ COL4, ROW8, 0, 0, GAD_SHOWINFO , GTYPE_HIT, ShowInfoRoutine } } ;


/***************************************************************************/


void
SpriteInitGadgets(void)
{
	struct _gadget *gPtr;

	CreateMultipleGadgets(permanentGadgets, 28);

	if(gPtr = FindGadget(&gadgetBase,GAD_LOADIFF)) {
		gPtr->dataPtr1 = (char *) iffLoadName;
		gPtr->dataPtr2 = (char *) iffLoadText;
		gPtr->dataPtr3 = (char *) loadButtonText;
		}

	if(gPtr = FindGadget(&gadgetBase,GAD_LOADSPR)) {
		gPtr->dataPtr1 = (char *) sprLoadName;
		gPtr->dataPtr2 = (char *) sprLoadText;
		gPtr->dataPtr3 = (char *) loadButtonText;
		}

	if(gPtr = FindGadget(&gadgetBase,GAD_SAVE)) {
		gPtr->dataPtr1 = (char *) sprSaveName;
		gPtr->dataPtr2 = (char *) sprSaveText;
		gPtr->dataPtr3 = (char *) saveButtonText;
		}

	if(gPtr = FindGadget(&gadgetBase,GAD_SETCHAR)) {
		gPtr->dataPtr1 = (char *) chrSaveName;
		gPtr->dataPtr2 = (char *) setText;
		gPtr->dataPtr3 = (char *) setButtonText;
		}

	return;
}


/***************************************************************************/

void
NewSpriteSizeRoutine(struct _gadget *gPtr, struct _input *in)
{
	if (previousGadget)
		UnHilightRegion(previousGadget->offsetX, previousGadget->offsetY,
			previousGadget->sizeX, previousGadget->sizeY, 16);

	if (previousGadget == gPtr)
	{
		previousGadget = gPtr = NULL;
		spriteboolean = boolean::FALSE;
		return;
	}
	HilightRegion(gPtr->offsetX, gPtr->offsetY, gPtr->sizeX, gPtr->sizeY, 16);
	previousGadget = gPtr;
	spriteboolean = boolean::TRUE;
	switch (gPtr->identity)
	{
		case	GAD_PANE_1X1:
			pane.sizex = 8;
			pane.sizey = 8;
			break;
		case	GAD_PANE_1X2:
			pane.sizex = 8;
			pane.sizey = 16;
			break;
		case	GAD_PANE_1X3:
			pane.sizex = 8;
			pane.sizey = 24;
			break;
		case	GAD_PANE_1X4:
			pane.sizex = 8;
			pane.sizey = 32;
			break;
		case	GAD_PANE_2X1:
			pane.sizex = 16;
			pane.sizey = 8;
			break;
		case	GAD_PANE_2X2:
			pane.sizex = 16;
			pane.sizey = 16;
			break;
		case	GAD_PANE_2X3:
			pane.sizex = 16;
			pane.sizey = 24;
			break;
		case	GAD_PANE_2X4:
			pane.sizex = 16;
			pane.sizey = 32;
			break;
		case	GAD_PANE_3X1:
			pane.sizex = 24;
			pane.sizey = 8;
			break;
		case	GAD_PANE_3X2:
			pane.sizex = 24;
			pane.sizey = 16;
			break;
		case	GAD_PANE_3X3:
			pane.sizex = 24;
			pane.sizey = 24;
			break;
		case	GAD_PANE_3X4:
			pane.sizex = 24;
			pane.sizey = 32;
			break;
		case	GAD_PANE_4X1:
			pane.sizex = 32;
			pane.sizey = 8;
			break;
		case	GAD_PANE_4X2:
			pane.sizex = 32;
			pane.sizey = 16;
			break;
		case	GAD_PANE_4X3:
			pane.sizex = 32;
			pane.sizey = 24;
			break;
		case	GAD_PANE_4X4:
			pane.sizex = 32;
			pane.sizey = 32;
			break;
		}
	return;
}

/***************************************************************************/

void
LoadRoutine(struct _gadget *gPtr, struct _input *in)
{
	LoadImage();
	return;
}

//=============================================================================

void
LoadIFFRoutine(struct _gadget *gPtr, struct _input *in)
{
//	ChangeExtension(textBox.text,".lbm");
//	PadString(textBox.text,12);
	FileReqRoutine(gPtr, LoadImage);
	return;
}

//=============================================================================

void
LoadSPRRoutine(struct _gadget *gPtr, struct _input *in)
{
	ChangeExtension(textBox.text,".spr");\
	PadString(textBox.text,12);
	FileReqRoutine(gPtr, LoadSprite);
	return;
}

//=============================================================================

void
SetChar(void)
{
	if(*chrSaveName != ' ')
		chrNameboolean = boolean::TRUE;
	  else
		chrNameboolean = boolean::FALSE;
	return;
}

//=============================================================================

int
CountLines(unsigned char *text)
{
	int lines = 1;
	if(*text == 0)
		return(0);
	while(*text)
	 {
		if(*text == '\n')
			lines++;
		*text++;
	 }
	return(lines);
}

//=============================================================================

int
WidestLine(unsigned char *text)
{
	int count = 0,maxCount = 0;

	while(*text)
	 {
		if(*text == '\n')
		 {
			if(count > maxCount)
				maxCount = count;
			count = 0;
		 }
		else
			count++;
		text++;
	 }
	if(count > maxCount)
		maxCount = count;
	return(maxCount);
}

//=============================================================================

MessageBox(struct VIEWPORT *vp,int backColor, int borderColor, unsigned char *text)
{
	int x,y,xSize,ySize;

	xSize = (WidestLine(text)*8)+8;
	ySize = (CountLines(text)*8)+8;

	x = ((vp->right/2) - xSize/2) + vp->left;
	y = ((vp->bottom/2) - ySize/2) + vp->top;

	HideMouse();
	display->SetColorIndex( backColor );
	display->Box( point(x,y), point(x+xSize,y+ySize) );

	display->SetColorIndex( borderColor );
	display->HLine( point(x+1,y+1), xSize-2 );
	display->HLine( point(x+1,y+ySize-1), xSize-2 );
	display->VLine( point(x+xSize-1,y+1), ySize-2 );
	display->VLine( point(x+1,y+1), ySize-2 );

	font->Move( point(x,y) );
	font->PrintLine( text );
//	T_xy_string(text, x+4 , y+4);

	// wait for key
	GetDirectKey();
	// erase image
	display->SetColorIndex( 0 );
	display->Box( point(x,y), point(x+xSize,y+ySize) );
	RefreshImage();
	ShowMouse();
}

void
SetCharRoutine(struct _gadget *gPtr, struct _input *in)
{
	ChangeExtension(textBox.text,".chr");
	PadString(textBox.text,12);
	FileReqRoutine(gPtr, SetChar);
	return;
}

void
SaveRoutine(struct _gadget *gPtr, struct _input *in)
{
	ChangeExtension(textBox.text,".spr");
	PadString(textBox.text,12);
	FileReqRoutine(gPtr, SaveSprite);
	return;
}


//=============================================================================
