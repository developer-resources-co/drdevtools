//=============================================================================
// guifunc.c
//=============================================================================

//=============================================================================
// c library includes

#include <stdio.h>
#include <alloc.h>
#include <conio.h>
#include <mem.h>
#include <stdlib.h>
#include <ctype.h>
#include <bios.h>

//=============================================================================
// DR library includes

#include "\gfxtools\lib\keys.h"
#include "\gfxtools\lib\global.h"
#include "\gfxtools\lib\input.h"
#include "\gfxtools\lib\gfx.h"
#include "\gfxtools\lib\general.h"
#include "\gfxtools\lib\mouse.h"
#include "\gfxtools\lib\gui.h"
#include "\gfxtools\lib\t.h"

//=============================================================================
// maped includes

#include "maped.h"
#include "guidefs.h"
#include "guifunc.h"
#include "guiinit.h"
#include "render.h"

//=============================================================================
// maped globals

int chars;
uchar huge *chardata;
int charpaloff;

//=============================================================================
// toggle switches/user parameters

FLAG refreshOn = TRUE;
FLAG xparentOn = FALSE;
FLAG clipSave = FALSE;
FLAG multiLayer = FALSE;
FLAG fullScreen = FALSE;
FLAG statusBar = TRUE;


int zoomFactor = 1;
int displayMode = DSP_NORMAL;
int currentmap =0;


// what to divide coordinates by

zoomDiv[9] =
{
	0,
	8,
	1,1,1,1,1,1,1
};


//=============================================================================

MAP selected;

int selectwidth=40, selectheight=7;
uint mapwidth=40, mapheight=17;

uint *undoBufferPtr = NULL;
unsigned long undoBufferSize = 0;

MAP maps[MAXMAPS];
MAP *mapPtr = &maps[0];
TILESET tilesets[MAXMAPS];
//uint tempbuffer[MAXSELECTY*MAXSELECTX];

//=============================================================================

unsigned long
GetLargestBuffer(void)
{
	unsigned long size = 0;
	int i;

	for(i=0;i<MAXMAPS;i++)
		if((maps[i].xSize * maps[i].ySize * sizeof(int)) > size)
			size = (maps[i].xSize * maps[i].ySize * sizeof(int));
	return(size);
}

//=============================================================================

void
AdjustUndoBuffer(void)
{
	unsigned long size;
	size = GetLargestBuffer();
	if(undoBufferSize > size)
		return;									// big enough already
	if(undoBufferPtr)
		free(undoBufferPtr);
	undoBufferPtr = SafeFarMalloc(size+4);
	undoBufferSize = size;
}

//=============================================================================

unsigned char helpText[] =
#if TESTING_FONT
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,'\n',
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,'\n',
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,'\n',
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,'\n',
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,'\n',
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,'\n',
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,'\n',
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,'\n',0
};
#else
{"\
                   Maped Help\n\
\n\
   x/y   Flips brush\n\
   1-4   Sets brush palette\n\
   F2/F3 Sets transparancy off/on\n\
   g     Selects char under mouse\n\
   .     Clears current brush(becomes 1x1 char 0) \n\
\n\
   F10   Toggles full screen mode\n\
   F9    Toggles status bar\n\
   m	   Toggles multiLayer\n\
   b     Toggles brush updates(refresh)\n\
   -/=   Zooms out/in\n\
   \\    Toggles display mode(normal/priority/color)\n\
   j     Swaps to next map page\n\
   J     Swaps contents of map page & spare page\n\
   a/z   Moves up/down in char display\n\
   w     Save current map to disk\n\
   W     Save current brush to disk\n\
   u     Undo previous plot\n\
   F1    Display this help window\n\
   i     Display Program info\n\
   s     Writes picture out to .LBM file\n\
   p     Print map\n\
   Q     Quit program\n\
   up/down/left/right          Scroll map\n\
   shift + up/down/left/right  Scroll fast\n\
   home/end/pgup/pgdwn         Scroll to edge\n\
  left mouse button plots current brush\n\
  right mouse button picks up new brush\n\
  shift+right button brings up options menu\n\
"
};
#endif


ShowHelp (int x,int y,struct _gad *gadget)
	{
	struct VIEWPORT subScreen = { 320, 0, 320, 0, 199  } ;

	MessageBox(&subScreen,DIMGRAY, MEDGRAY, &helpText);
	}

//=============================================================================

unsigned char aboutText[] =
{
"          Maped "
		MAPED_VERSION
			"\n\
              by\n\
       Kevin T. Seghetti,\n\
     William B. Norris IV,\n\
             and\n\
         Lars Norpchen\n\
(c) 1991,92 Developer Resources\n\
	   Last Revison: "
	   __DATE__
};


ShowAbout (int x,int y,struct _gad *gadget)
	{
	struct VIEWPORT subScreen = { 320, 0, 320, 0, 199  } ;

	MessageBox(&subScreen,DIMGRAY, MEDGRAY, &aboutText);
	}

//=============================================================================

ShowInfo (int x,int y,struct _gad *gadget)
{
	struct VIEWPORT subScreen = { 320, 0, 320, 136, 199  } ;
	char textString[200];
	char *sPtr;
    int a,b;

    if (MouseY>(mapBotEdge-1)) return;
	a = (MouseX/zoomDiv[zoomFactor]) + mapXOffset;
	b = (MouseY/zoomDiv[zoomFactor]) + mapYOffset;

	if(zoomFactor == 1)
    	XORChars (a-mapXOffset,b-mapYOffset,0,0);

    sprintf (textString,"X: %03d Y: %03d  Char Number: %04x\n",a,b,*(mapPtr->buffer + (mapPtr->xSize * b)+a));
	sPtr = FindChar(textString,0);
    sprintf (sPtr,"XSIZE: %03d YSIZE: %03d Zoom: %01d\n",mapPtr->xSize,mapPtr->ySize,zoomFactor);
	sPtr = FindChar(sPtr,0);
	sprintf (sPtr,"# chars: %d\n", chars);
	sPtr = FindChar(sPtr,0);
    sprintf (sPtr,"Map: %s",mapPtr->szFilename );
	sPtr = FindChar(sPtr,0);
	if(multiLayer)
    	sprintf (sPtr,"  Multi Layer: %d",multiLayer);
	MessageBox(&subScreen,DIMGRAY, MEDGRAY, textString);
}

//=============================================================================

int
WriteMap( MAP *mapPtr, char *szFileName )
{
	FILE *fp;
	char tempstr[100];
	int a,b;
	int width = mapPtr->xSize;
	int height = mapPtr->ySize;

	// If no filename passed, retrieve from map structure
	if ( !szFileName )
	 { 									// Regular, full-screen map
		szFileName = mapPtr->szFilename;

		if (clipSave)
		 {
			int breakout = FALSE;

    		a = height;
    		while ((!breakout)&&(a>0))
       		 {
        		a--;
        		for (b=0;b<mapPtr->xSize;b++)
            		if (*(mapPtr->buffer + (mapPtr->xSize * a)+b)!=0)
                		breakout = TRUE;
        	 }
    		height = a+1;

    		breakout = FALSE;
    		a=width;
    		while ((!breakout)&&(a>0))
       		 {
        		a--;
        		for (b=0;b<mapPtr->ySize;b++)
            		if (*(mapPtr->buffer + (mapPtr->xSize * b)+a)!=0)
                		breakout = TRUE;
      		 }
   			width = a+1;
		 }
	 }

	fp=fopen( szFileName, "wb" );
	if (fp==NULL) Error ("Cannot open output file...");

#if defined(GENESIS)
	if(fputc((unsigned char)(width >> 8),fp) == EOF) Error("Write fault on width");
	if(fputc((unsigned char)width,fp)== EOF) Error("Write fault on width");

	if(fputc((unsigned char)(height >> 8),fp)== EOF) Error("Write fault on width");
	if(fputc((unsigned char)height,fp)== EOF) Error("Write fault on width");

	for (a=0;a<height;a++)
   		for (b=0;b<width;b++)
		 {
			if(fputc((unsigned char)(*(mapPtr->buffer + (mapPtr->xSize * a)+b)>>8),fp)== EOF) Error("Write fault on map");
			if(fputc((unsigned char)*(mapPtr->buffer + (mapPtr->xSize * a)+b),fp)== EOF) Error("Write fault on map");
		 }
#endif

#if defined(SNES)
	if(fputc((unsigned char)width,fp)== EOF) Error("Write fault on width");
	if(fputc((unsigned char)(width >> 8),fp) == EOF) Error("Write fault on width");

	if(fputc((unsigned char)height,fp)== EOF) Error("Write fault on width");
	if(fputc((unsigned char)(height >> 8),fp)== EOF) Error("Write fault on width");

	for (a=0;a<height;a++)
		for (b=0;b<width;b++)
		 {
			if(fputc((unsigned char)*(mapPtr->buffer + (mapPtr->xSize * a)+b),fp)== EOF) Error("Write fault on map");
			if(fputc((unsigned char)(*(mapPtr->buffer + (mapPtr->xSize * a)+b)>>8),fp)== EOF) Error("Write fault on map");
		 }
#endif
	fclose (fp);
	sprintf (tempstr,"Saved file %s: %03d x %03d",szFileName,width,height );
	Message (tempstr,WAIT_AND_EAT,DIMGRAY,MEDGRAY);
}

//==============================================================================

int
	SaveBrush( void )
{
	WaitForLift();
	WriteMap( &selected, "brush.map" );
}

//==============================================================================

int
SaveHit(int x,int y,struct _gad *gadget)
{
	WaitForLift();
	WriteMap( mapPtr, NULL );			// NULL == use filename in map
}

//=============================================================================

int QuitHit (int x,int y,struct _gad *gadget)
{
	struct _input in;
    int a;
	in.inputMask = 0;
    WaitForLift ();
    Message ("Are you sure?",WAIT_DONT_EAT,DIMGRAY,MEDGRAY);
    if (kbhit ())
        a = GetDirectKey(&in);
    a = toupper (a);
    if (a=='Y')
     {
        SetGfxMode(3);
        if (chardata!=NULL) farfree ((void far *) chardata);
        Quit ();
     }
}

//=============================================================================

int RTHit (int x,int y,struct _gad *gadget)
{
    int a;
    WaitForLift ();
	refreshOn ^= 1;
}

//=============================================================================

int KeyMapBrushKill (int x,int y,struct _gad *gadget)
{
    selected.xSize = 1;
    selected.ySize = 1;
    *selected.buffer = 0;
}

//=============================================================================

int KeyToggleStatusBar(int x,int y,struct _keygad *gadget)
{
	if(statusBar)
		statusBar = FALSE;
	else
		statusBar = TRUE;
}

//=============================================================================

int KeyZoomIn(int x,int y,struct _keygad *gadget)
{
//	if(zoomFactor > 1)
//		zoomFactor--;
	zoomFactor = 1;
}

//=============================================================================

int KeyZoomOut(int x,int y,struct _keygad *gadget)
{
//	if(zoomFactor < 8)
//		zoomFactor++;
	zoomFactor = 8;
}

int KeyNextDspMode(int x,int y,struct _keygad *gadget)
{
	displayMode++;
	if(displayMode >= DSP_MAX)
		displayMode = 0;
}

//=============================================================================

int KeyToggleMulti(int x,int y,struct _keygad *gadget)
{
	if(multiLayer)
		multiLayer = FALSE;
	else
		multiLayer = TRUE;
}

//=============================================================================

char dspModeTbl[DSP_MAX] =
{
	'N',
	'P',
	'C',
	'U'
};

PrintStatusBar()
	{
	int a,b;
   char final[54];
	int tile, usage;

	a = (MouseX/zoomDiv[zoomFactor]) + mapXOffset;
	b = (MouseY/zoomDiv[zoomFactor]) + mapYOffset;

	CalcUsageCount(mapPtr);
	tile = *( mapPtr->buffer + (mapPtr->xSize * b) + a ) & CHARMASK;
	usage = *(mapPtr->usage+tile);

   sprintf (final,"X:%03d Y:%03d  Usage: %-5d  %c%c%c%c  Z:%01d  %s",
		a,b,usage,
		multiLayer?'M':' ',refreshOn?' ':' ',xparentOn?'X':' ',
		dspModeTbl[displayMode],zoomFactor, mapPtr->szFilename );
	PadString(final,52);
   T_xy_ostring ((char far *) final,0,194);
	}


//=============================================================================

int KeyToggleFullScreen (int x,int y,struct _keygad *gadget)
{
	if(fullScreen)
	 {
		fullScreen = FALSE;
    	firstgadget = &MapBoxGadget;
		mapBotEdge = 136;
	 }
	else
	 {
		fullScreen = TRUE;
    	firstgadget = &MapFullBoxGadget;
		mapBotEdge = 199;
	 }
	mapheight = (firstgadget->height+1)>>3;
	mapwidth = (firstgadget->width+1)>>3;

    while (mapYOffset>((signed int) mapPtr->ySize)-(signed int)mapheight) mapYOffset--;
	if(mapYOffset < 0)
		mapYOffset = 0;
}

//=============================================================================

int KeyMapBrushPal (int x,int y,struct _keygad *gadget)
{
	int a,b;
    for (a=0;a<selected.ySize;a++)
        for (b=0;b<selected.xSize;b++)
			switch(gadget->hitdata)
			 {
				case 1:
					*(selected.buffer + (a * selected.xSize) + b) &= PAL0MASK;
    				break;
    			case 2:
					*(selected.buffer + (a * selected.xSize) + b) &= PAL0MASK;
					*(selected.buffer + (a * selected.xSize) + b) |= PAL1MASK;
        			break;
    			case 3:
					*(selected.buffer + (a * selected.xSize) + b) &= PAL0MASK;
					*(selected.buffer + (a * selected.xSize) + b) |= PAL2MASK;
    				break;
    			case 4:
					*(selected.buffer + (a * selected.xSize) + b) |= PAL3MASK;
					break;
			 }
}

//==============================================================================

int
KeyMapFlipBrushX (int x,int y,struct _gad *gadget)
{
	uint tempbuffer[MAXSELECTY*MAXSELECTX];
	int d,e;
	uint dx,dy;

	dx = selected.xSize;
	dy = selected.ySize;

	Paste (selected.buffer,(uint huge *) tempbuffer,dx,dy,selected.xSize,selected.xSize);
	for (d=0;d<dy;d++)
		for (e=0;e<dx;e++)
			*(selected.buffer + (d * selected.xSize) + e) =
				*(tempbuffer + (d*selected.xSize)+((dx-1)-e)) ^ HFLIPMASK;
}

//=============================================================================

int
KeyMapFlipBrushY (int x,int y,struct _gad *gadget)
{
   uint tempbuffer[MAXSELECTY*MAXSELECTX];
	int d,e;
	uint dx,dy;

	dx = selected.xSize;
	dy = selected.ySize;

	Paste (selected.buffer,(uint huge *) tempbuffer,dx,dy,selected.xSize,selected.xSize);
   for (d=0;d<dy;d++)
   	for (e=0;e<dx;e++)
			*(selected.buffer + (d * selected.xSize) + e) =
				*(tempbuffer + (((dy-1)-d)*selected.xSize) + e) ^ VFLIPMASK;
}

//=============================================================================

int XParentOn (int x,int y,struct _gad *gadget)
{
	xparentOn = TRUE;
}

//=============================================================================

int XParentOff (int x,int y,struct _gad *gadget)
{
	xparentOn = FALSE;
}

//=============================================================================

int UndoHit (int x,int y,struct _gad *gadget)
{
    WaitForLift ();
    memcpy (mapPtr->buffer,undoBufferPtr,undoBufferSize);
}

//=============================================================================

void CopyUndoBuffer (void)
{
    memcpy (undoBufferPtr,mapPtr->buffer,undoBufferSize);
}

//=============================================================================

int
	SwapHit (int x,int y,struct _gad *gadget)
	{
   WaitForLift ();
   if (num_of_maps==0) return;
   mapPtr->xOffset=mapXOffset;
   mapPtr->yOffset=mapYOffset;
   currentmap++;
   if (currentmap>num_of_maps) currentmap=0;
	mapPtr = &maps[currentmap];
   CopyUndoBuffer ();
   mapXOffset = mapPtr->xOffset;
   mapYOffset = mapPtr->yOffset;
	}


void
	SwapWithSparePage( void )
	{
	MAP tempMap;
	char *szFilename;
	FLAG fSpare;
	int base = currentmap & ~1;

	tempMap = maps[ base ];
	maps[ base ] = maps[ base+1 ];
	maps[ base+1 ] = tempMap;

	szFilename = maps[ base ].szFilename;
	maps[ base ].szFilename = maps[ base+1 ].szFilename;
	maps[ base+1 ].szFilename = szFilename;

	fSpare = maps[ base ].fSpare;
	maps[ base ].fSpare = maps[ base+1 ].fSpare;
	maps[ base+1 ].fSpare = fSpare;
	}

//=============================================================================

int CharUpDownHit (int x,int y,struct _gad *gadget)
{
    charoffset += selectwidth * gadget->hitdata;
    if (charoffset<0)
	 {
		charoffset =0;
		return;
	 }
    if (charoffset > chars-(selectwidth*selectheight))
        charoffset -= selectwidth * gadget->hitdata;
}

//=============================================================================


int MapUpDownHit (int x,int y,struct _gad *gadget)
{
    mapYOffset+= gadget->hitdata;
    while (mapYOffset>((signed int) mapPtr->ySize)-(signed int)mapheight) mapYOffset--;
    if (mapYOffset < 0 || ((signed int)mapPtr->ySize-(signed int)mapheight) < 0)
		mapYOffset=0;
}

//=============================================================================

int KeyMapUpDownHit (int x,int y,struct _keygad *gadget)
{
    mapYOffset+= gadget->hitdata;
    while (mapYOffset > ((signed int) mapPtr->ySize)-(signed int)mapheight) mapYOffset--;
    if ((mapYOffset < 0) || (((signed int)mapPtr->ySize-mapheight) < 0))
		mapYOffset=0;
}

int KeyMapLeftEdgeHit (int x,int y,struct _keygad *gadget)
{
    mapXOffset = 0;
}

int KeyMapRightEdgeHit (int x,int y,struct _keygad *gadget)
{
    mapXOffset = mapPtr->xSize-mapwidth;
    if (mapXOffset<0) mapXOffset=0;
}

//=============================================================================

int KeyMapTopEdgeHit (int x,int y,struct _keygad *gadget)
{
    mapYOffset = 0;
    if (mapYOffset<0) mapYOffset=0;
}

//=============================================================================

int KeyMapBotEdgeHit (int x,int y,struct _keygad *gadget)
{
    mapYOffset = mapPtr->ySize-mapheight;

}

//=============================================================================

int MapLeftRightHit (int x,int y,struct _gad *gadget)
{
    mapXOffset+=gadget->hitdata;
    if (mapXOffset<0) mapXOffset=0;
    while ((signed int) mapXOffset>(signed int) mapPtr->xSize-mapwidth) mapXOffset--;
}

//=============================================================================

int TextBox (struct _gad *gadget)
{
    VGASolidBox(gadget->x,gadget->y,gadget->width+gadget->x,gadget->height+gadget->y,DIMGRAY);
    VGAHollowBox (gadget->x,gadget->y,gadget->width+gadget->x,gadget->height+gadget->y,MEDGRAY);
    T_xy_string ((char far *) gadget->drawdata,gadget->x+2,gadget->y+1);
}

//=============================================================================

int CharBox (int x,int y,struct _gad *gadget)
{
    int a,b,oldx,oldy;
    uint startx,starty,endx,endy;
    uint startchar;
    HideMouse ();
    startx = x/8;
    starty = y/8;
    oldx =startx;
    oldy =starty;
    XORChars2 (startx,starty,oldx-startx,oldy-starty);
    while (VGetMouse(&MouseX,&MouseY)!=0)
            if ((MouseX>>3!=oldx)||((MouseY-144)>>3!=oldy))
                {
                    XORChars2 (startx,starty,oldx-startx,oldy-starty);
                    oldx = MouseX>>3;
                    oldy = (MouseY-144)>>3;
                    XORChars2 (startx,starty,oldx-startx,oldy-starty);
                }
    endx = MouseX-gadget->x;
    endy = MouseY-gadget->y;
    endx /=8;
    endy /=8;
    if (startx>endx) Swap ((int *) &startx,(int *)&endx);
    if (starty>endy) Swap ((int *) &starty,(int *) &endy);
    selected.xSize = 1+endx-startx;
    selected.ySize = 1+endy-starty;
    startchar = startx + (selectwidth*starty);
    for (a=0;a<selected.ySize;a++)
     for (b=0;b<selected.xSize;b++)
        *(selected.buffer + (selected.xSize*a) + b) = charoffset + b + (a*selectwidth) + startchar;
    ShowMouse ();
}

//=============================================================================

int MapBox (int x,int y,struct _gad *gadget)
{
    int a,b;

    if (VGetMouse(&MouseX,&MouseY)==2)
     {
        SelectMapBox (x,y,gadget);
        return;
     }
    if (VGetMouse(&MouseX,&MouseY)==4)
     {
        SelectPiece (x,y,gadget);
        return;
     }
    CopyUndoBuffer ();
    WaitForLift ();
    x = x/zoomDiv[zoomFactor];
    y = y/zoomDiv[zoomFactor];
    for (a=0;(a<selected.ySize)&&(y+mapYOffset+a<mapPtr->ySize);a++)
     for (b=0;(b<selected.xSize)&&(x+mapXOffset+b<mapPtr->xSize);b++)
        if ((*(selected.buffer + (selected.xSize * a) + b)&CHARMASK)||(!xparentOn))
		   *(mapPtr->buffer + (x+b+mapXOffset) + ((y+a+mapYOffset) * mapPtr->xSize)) = *(selected.buffer + (selected.xSize * a) +b);
}

//=============================================================================

SelectPiece (int x,int y,struct _gad *gadget)
{
    uint startx,starty;
    startx = x/8;
    starty = y/8;
    selected.xSize = 1;
    selected.ySize = 1;
    *selected.buffer = *(mapPtr->buffer + (startx+mapXOffset) + ((starty+mapYOffset) * mapPtr->xSize));
}

//=============================================================================

SelectKB (int x,int y,struct _gad *gadget)
{
    SelectPiece (MouseX,MouseY,gadget);
}

//=============================================================================

SelectMapBox (int x,int y,struct _gad *gadget)
{
    uint tempbuffer[MAXSELECTY*MAXSELECTX];
    uint startx,starty,endx,endy,dx,dy,oldx,oldy;
    int in;
	struct _input instruct;
    int a,b,d,e;
	int addend = zoomDiv[zoomFactor];
	short mouseMask = 0xffff;

	instruct.inputMask = INPF_CASESENSE;
    HideMouse ();
	DrawMapBox(firstgadget);
    startx = x;
    starty = y;
	if(zoomFactor == 1)
		mouseMask = 0xfff8;
    oldx = startx;
    oldy = starty;
    XORBOX (startx,starty,(oldx-startx)+addend,(oldy-starty)+addend);
    while (VGetMouse(&MouseX,&MouseY)==INP_MOUSEMOVE)
	 {
		MouseX &= mouseMask;
		MouseY &= mouseMask;
		if ((MouseX!=oldx)||(MouseY!=oldy))
         {
    		XORBOX (startx,starty,(oldx-startx)+addend,(oldy-starty)+addend);
            oldx = MouseX;
            oldy = MouseY;
    		XORBOX (startx,starty,(oldx-startx)+addend,(oldy-starty)+addend);
         }
	 }
    endx = MouseX-gadget->x;
    endy = MouseY-gadget->y;
	if(zoomFactor == 1)
	 {
    	endx /=zoomDiv[zoomFactor];
    	endy /=zoomDiv[zoomFactor];
		startx/=zoomDiv[zoomFactor];
		starty/=zoomDiv[zoomFactor];
	 }
    if (startx>endx) Swap ((int *) &startx,(int *) &endx);
    if (starty>endy) Swap ((int *) &starty,(int *) &endy);

	if(endx >= mapPtr->xSize)
		endx = mapPtr->xSize-1;

	if(endy >= mapPtr->ySize)
		endy = mapPtr->ySize-1;

    dx = 1 + endx - startx;
    dy = 1 + endy - starty;

    if ((bioskey(2)&3))
	 {
		DrawSelectCommands ();
		while (!kbhit ());          // wait for keystroke
		in = GetDirectKey (&instruct);
	 }
	else
    	in = 'c';
    switch (in)
        {
			case KEY_UP:
			case KEY_DOWN:
            case '>':                                        // vertical flip
            case '.':
            	CopyUndoBuffer ();
				Paste (mapPtr->buffer + ((starty+mapYOffset) * mapPtr->xSize) + startx+mapXOffset,(uint huge *) tempbuffer,dx,dy,mapPtr->xSize,dx);
            	for (d=0;d<dy;d++)
                	for (e=0;e<dx;e++)
						*(mapPtr->buffer + ((d+starty+mapYOffset) * mapPtr->xSize) + (e + startx+mapXOffset)) = tempbuffer[(((dy-1)-d)*dx)+e] ^ VFLIPMASK;
            	break;
			case KEY_LEFT:
			case KEY_RIGHT:
            case '<':
            case ',':
                CopyUndoBuffer ();
				Paste (mapPtr->buffer + ((starty+mapYOffset) * mapPtr->xSize) + startx+mapXOffset,(uint huge *) tempbuffer,dx,dy,mapPtr->xSize,dx);
                for (d=0;d<dy;d++)
                    for (e=0;e<dx;e++)
						*(mapPtr->buffer + ((d+starty+mapYOffset) * mapPtr->xSize) + (e + startx+mapXOffset)) = tempbuffer[(d*dx)+((dx-1)-e)] ^ HFLIPMASK;
                break;

            case 'P':                                   // priority bit
            case 'p':
                CopyUndoBuffer ();
                for (a=starty;a<endy+1;a++)
                	for (b=startx;b<endx+1;b++)
						*(mapPtr->buffer + ((a+mapYOffset) * mapPtr->xSize) + (b+mapXOffset)) |= PRIBITMASK;
    			break;

            case 'U':                                   // unset priority bit
            case 'u':
                CopyUndoBuffer ();
                for (a=starty;a<endy+1;a++)
                	for (b=startx;b<endx+1;b++)
						*(mapPtr->buffer + ((a+mapYOffset) * mapPtr->xSize) + (b+mapXOffset)) &= ~PRIBITMASK;
    			break;

            case '1':
                CopyUndoBuffer ();
                for (a=starty;a<endy+1;a++)
                	for (b=startx;b<endx+1;b++)
						*(mapPtr->buffer + ((a+mapYOffset) * mapPtr->xSize) + (b+mapXOffset)) &= PAL0MASK;
    			break;
            case '2':
                CopyUndoBuffer ();
                for (a=starty;a<endy+1;a++)
               	for (b=startx;b<endx+1;b++)
    			 {
					*(mapPtr->buffer + ((a+mapYOffset) * mapPtr->xSize) + (b+mapXOffset)) &= PAL0MASK;
					*(mapPtr->buffer + ((a+mapYOffset) * mapPtr->xSize) + (b+mapXOffset)) |= PAL1MASK;
    			 }
                break;
            case '3':
    			CopyUndoBuffer ();
    			for (a=starty;a<endy+1;a++)
    			for (b=startx;b<endx+1;b++)
        		 {
					*(mapPtr->buffer + ((a+mapYOffset) * mapPtr->xSize) + (b+mapXOffset)) &= PAL0MASK;
					*(mapPtr->buffer + ((a+mapYOffset) * mapPtr->xSize) + (b+mapXOffset)) |= PAL2MASK;
        		 }
    			break;
            case '4':
    			CopyUndoBuffer ();
    			for (a=starty;a<endy+1;a++)
    				for (b=startx;b<endx+1;b++)
						*(mapPtr->buffer + ((a+mapYOffset) * mapPtr->xSize) + (b+mapXOffset)) |= PAL3MASK;

    			break;
                                                          // Copy Function
            case 'g':
            case 'G':
            case 'c':
            case 'C':
                selected.xSize = dx;
                selected.ySize = dy;
				Paste (mapPtr->buffer + ((starty+mapYOffset)* mapPtr->xSize) + (startx+mapXOffset),(uint huge *) selected.buffer,dx,dy,mapPtr->xSize,selected.xSize);
                break;
                                                          // Cut Function (copy then clear)
            case 'x':
            case 'X':
                selected.xSize = dx;
                selected.ySize = dy;
				Paste (mapPtr->buffer + ((starty+mapYOffset)* mapPtr->xSize) + (startx+mapXOffset),(uint huge *) selected.buffer,dx,dy,mapPtr->xSize,selected.xSize);
				// note: fall throught to clear
                                                          // Clear Function
            case 'z':
            case 'Z':
				CopyUndoBuffer ();
				Clear (mapPtr->buffer + ((starty+mapYOffset) * mapPtr->xSize) + (startx+mapXOffset),dx,dy,mapPtr->xSize);
				break;
        }
        ShowMouse ();
}

//=============================================================================

void DrawSelectCommands (void)
{
    int b;
    VGASolidBox (30,140,320-30,190,DIMGRAY);
    VGAHollowBox (30,140,320-30,190,MEDGRAY);
    T_xy_string ("Press C to Copy, X to Cut, Z to Clear",40,143);
    T_xy_string ("Press cursor keys to flip horiz & vert",40,153);
    T_xy_string ("Press 1,2,3, or 4 to select palettes",40,173);
    T_xy_string ("Press P to set priority bit, U to unset",40,183);
}

//=============================================================================

int
Paste (uint huge *sourcebuffer,uint huge *destbuffer,uint width,uint height,int sourcewidth,int destwidth)
{
	int a;

	for (a=0;a<height;a++)
	 {
		memcpy ((void far *) destbuffer,(void far *) sourcebuffer,width*2);
		sourcebuffer += sourcewidth;
		destbuffer   += destwidth;
	 }
	CalcUsageCount( mapPtr );
}

int Clear (uint *destbuffer,uint width,uint height,int destwidth)
{
  int a;
  for (a=0;a<height;a++)
    {
    memset (destbuffer,0,width*2);
    destbuffer += destwidth;
    }
}

//=============================================================================
