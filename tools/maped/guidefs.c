//=============================================================================
// guidefs.c
//=============================================================================

//=============================================================================
// c library includes

#include <stdio.h>

//=============================================================================
// DR library includes

#include "\gfxtools\lib\keys.h"
#include "\gfxtools\lib\global.h"
#include "\gfxtools\lib\gui.h"

//=============================================================================
// maped includes

#include "guidefs.h"
#include "guifunc.h"
#include "render.h"

//=============================================================================

#define XPOS 290

char QuitText[5]="QUIT";
char RTText[]="BSHOW";
char UndoText[5]="OOPS";
char SaveText[5]="SAVE";
char CharUpText[5]="UP";
char CharDownText[5]="DOWN";
char SwapText[5]="SWAP";
char PriText[5]="P";
char ColorText[5]="C";

char MapUpText[2]="D";
char MapDownText[2]="U";
char MapLeftText[2]="<";
char MapRightText[2]=">";

char TurboLeftText[5]="<<";
char TurboRightText[5]=">>";


void
RefreshImage(void)
{
	DrawGUI ();
}

struct _gad UndoGadget=						// undo previous plot operation
{
    'u',
    UndoHit,
    0,
    220,
    136,
    29,
    7,
    TextBox,
    (long) UndoText,
    NULL
};

struct _gad SwapGadget=						// swap to scrap(other) page
{
    'j',									// just like dpaint
    SwapHit,
    0,
    170,
    136,
    29,
    7,
    TextBox,
    (long) SwapText,
    &UndoGadget
};

struct _gad TurboLeftGadget=				// move left fast through map
{
    KEY_SHIFTED|KEY_LEFT,   //'H',
    MapLeftRightHit,
    -10,
    110,
    136,
    19,
    7,
    TextBox,
    (long) TurboLeftText,
    &SwapGadget
};

struct _gad TurboRightGadget=				// move right fast through map
{
    KEY_SHIFTED|KEY_RIGHT,  //'L',
    MapLeftRightHit,
    10,
    150,
    136,
    19,
    7,
    TextBox,
    (long) TurboRightText,
    &TurboLeftGadget
};


struct _gad MapUpGadget=					// move up through map
{
    KEY_DOWN,    //'M',
    MapUpDownHit,
    1,
    90,
    136,
    9,
    7,
    TextBox,
    (long) MapUpText,
    &TurboRightGadget
};


struct _gad MapDownGadget=					// move down through map
{
    KEY_UP,    //'I',
    MapUpDownHit,
    -1,
    100,
    136,
    9,
    7,
    TextBox,
    (long) MapDownText,
    &MapUpGadget
};

struct _gad MapLeftGadget=					// move left through map
{
    KEY_LEFT,    //'J',
    MapLeftRightHit,
    -1,
    130,
    136,
    9,
    7,
    TextBox,
    (long) MapLeftText,
    &MapDownGadget
};

struct _gad MapRightGadget=					// move right through map
{
    KEY_RIGHT,     //'K',
    MapLeftRightHit,
    1,
    140,
    136,
    9,
    7,
    TextBox,
    (long) MapRightText,
    &MapLeftGadget
};





struct _gad CharUpGadget=					// move up through characters
{
    'a',
    CharUpDownHit,
    -1,
    60,
    136,
    29,
    7,
    TextBox,
    (long) CharUpText,
    &MapRightGadget
};

struct _gad CharDownGadget=					// move down through characters
{
    'z',
    CharUpDownHit,
    1,
    30,
    136,
    29,
    7,
    TextBox,
    (long) CharDownText,
    &CharUpGadget
};

struct _gad SaveGadget=						// save current map to disk
{
    'w',
    SaveHit,
    0,
    0,
    136,
    29,
    7,
    TextBox,
    (long) SaveText,
    &CharDownGadget
};

struct _gad CharBoxGadget=					// region containing characters
{
    0,
    CharBox,
    0,
    0,
    144,
    319,
    199-144,
    DrawCharBox,
    0,
    &SaveGadget
};



struct _gad RefreshToggleGadget=						// controls drawing of current brush
{
    'b',
    RTHit,
    0,
    250,
    136,
    XPOS-250,
    7,
    TextBox,
    (long) RTText,
    &CharBoxGadget
};

struct _gad QuitGadget=						// leave program
{
    'Q',
    QuitHit,
    0,
    XPOS,
    136,
    319-XPOS,
    7,
    TextBox,
    (long) QuitText,
    &RefreshToggleGadget
};

struct _gad MapBoxGadget=					// region containing map
{
    0,
    MapBox,
    0,
    0,
    0,
    319,
    135,
    DrawMapBox,
    0,
    &QuitGadget
};

struct _gad MapFullBoxGadget=					// region containing map
{
    0,
    MapBox,
    0,
    0,
    0,
    319,
    199,
    DrawMapBox,
    0,
    NULL									// only gadget in this list
};

//===========================================================================
// key gadgets

void SwapWithSparePage( void );
void printscreen( void );
void WritePic( void );
void SaveBrush( void );


struct _keygad SaveBrushGadget =
	{
	'W',
	SaveBrush,
	0,
	NULL
	};

struct _keygad SaveIffGadget =
	{
	's',
	WritePic,
	0,
	&SaveBrushGadget
	};

struct _keygad PrintScreenGadget =
	{
	'P',
	printscreen,
	0,
	&SaveIffGadget
	};

struct _keygad SwapWithSparePageGadget =
	{
	'J',
	SwapWithSparePage,
	0,
	&PrintScreenGadget
	};

struct _keygad DosShellGadget =
{
	'd',
	DosShell,
	0,
	&SwapWithSparePageGadget
};
struct _keygad ZoomOutGadget=
{
    '-',
    KeyZoomOut,
    0,
   &DosShellGadget
};
struct _keygad ZoomInGadget=
{
    '=',
    KeyZoomIn,
    0,
	&ZoomOutGadget
};

struct _keygad Quit2Gadget=						// leave program
{
    KEY_ALTX,
    QuitHit,
    0,
    &ZoomInGadget
};

struct _keygad ToggleFullScreenGadget =					// set all of brush to palette 4
{
    KEY_F10,
    KeyToggleFullScreen,
    1,
	&Quit2Gadget
};

struct _keygad ToggleStatusBarGadget =
{
    KEY_F9,
    KeyToggleStatusBar,
    1,
	&ToggleFullScreenGadget
};


struct _keygad ToggleMultiGadget =
{
    'm',
    KeyToggleMulti,
    1,
	&ToggleStatusBarGadget
};


struct _keygad BrushPalGadget1 =					// set all of brush to palette 4
{
    '1',
    KeyMapBrushPal,
    1,
	&ToggleMultiGadget
};

struct _keygad BrushPalGadget2 =					// set all of brush to palette 4
{
    '2',
    KeyMapBrushPal,
    2,
	&BrushPalGadget1
};

struct _keygad BrushPalGadget3 =					// set all of brush to palette 4
{
    '3',
    KeyMapBrushPal,
    3,
	&BrushPalGadget2
};

struct _keygad BrushPalGadget4 =					// set all of brush to palette 4
{
    '4',
    KeyMapBrushPal,
    4,
	&BrushPalGadget3
};

struct _keygad BrushKillGadget =					// kill current brush
{
    '.',
    KeyMapBrushKill,
    0,
	&BrushPalGadget4
};

struct _keygad FlipBrushXGadget =					// flip brush along x axis
{
    'x',
    KeyMapFlipBrushX,
    0,
	&BrushKillGadget
};

struct _keygad FlipBrushYGadget =  					// flip brush along y axis
{
    'y',
    KeyMapFlipBrushY,
    0,
    &FlipBrushXGadget
};

struct _keygad TopEdgeGadget =						// move up to top edge of map
{
    KEY_PGUP,
    KeyMapTopEdgeHit,
    0,
    &FlipBrushYGadget
};

struct _keygad BottomEdgeGadget =					// move down to bottom edge of map
{
    KEY_PGDWN,
    KeyMapBotEdgeHit,
    0,
	&TopEdgeGadget
};

struct _keygad LeftEdgeGadget =						// move to left edge of map
{
    KEY_HOME,
    KeyMapLeftEdgeHit,
    0,
	&BottomEdgeGadget
};

struct _keygad RightEdgeGadget =						// move to right edge of map
{
    KEY_END,
    KeyMapRightEdgeHit,
    0,
	&LeftEdgeGadget
};

struct _keygad TurboUpGadget =						// move up quickly through map
{
    KEY_SHIFTED|KEY_UP,
    KeyMapUpDownHit,
    -10,
	&RightEdgeGadget
};

struct _keygad TurboDownGadget =						// move down quickly through map
{
    KEY_SHIFTED|KEY_DOWN,
    KeyMapUpDownHit,
    10,
    &TurboUpGadget
};

struct _keygad XparentOnGadget =						// enable transparency(default)
{
    KEY_F2,
    XParentOn,
    0,
    &TurboDownGadget
};

struct _keygad XparentOffGadget =						// disable transparency
{
    KEY_F3,
    XParentOff,
    0,
	&XparentOnGadget
};

struct _keygad InfoGadget =						// get info
{
    '/',
    ShowInfo,
    0,
    &XparentOffGadget
};

struct _keygad AboutGadget =						// show about box
{
	'i',
    ShowAbout,
    0,
    &InfoGadget
};

struct _keygad HelpGadget =						// show help box
{
    KEY_F1,
    ShowHelp,
    0,
    &AboutGadget
};

struct _keygad SelectKBGadget =					// select char under mouse
{
    'g',
    SelectKB,
    0,
    &HelpGadget
};

struct _keygad NextDSPModeGadget =					// toggle display modes
{
    '\\',
    KeyNextDspMode,
    0,
    &SelectKBGadget
};

struct _keygad ShowBrushGadget =				// draw brush this frame
{
    ' ',
    ShowBrush,
    0,
    &NextDSPModeGadget
};

//=============================================================================

