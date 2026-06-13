//=============================================================================
// render.c: rendering funcitons
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
#include "\gfxtools\lib\gfx.h"
#include "\gfxtools\lib\general.h"
#include "\gfxtools\lib\mouse.h"
#include "\gfxtools\lib\gui.h"
#include "\gfxtools\lib\t.h"

//=============================================================================
// maped includes

#include "guidefs.h"
#include "guifunc.h"
#include "guiinit.h"
#include "render.h"

//=============================================================================
// rendering globals

int charoffset=0;
signed int mapXOffset=0;
signed int mapYOffset=0;

//=============================================================================

void
ShowBrush (int x,int y, struct _gad *gadget)
{
   int a,b,a2,b2,character,tile;

   HideMouse();
   MouseX &= 0xfff8;
   MouseY &= 0xfff8;
   for (b2=MouseY,b=0;b<selected.ySize && ((b2>>3) + mapYOffset<mapPtr->ySize);b++,b2+=8)
   	for (a2=MouseX,a=0;a<selected.xSize && ((a2>>3) + mapXOffset < mapPtr->xSize);a++,a2+=8)
      	{
	      tile = *(selected.buffer+(selected.xSize*b) + a);
			character = tile & CHARMASK;
			if (tile&HFLIPMASK) character+=chars;
			if (tile&VFLIPMASK) character+=chars*2;

//			if (((tile&CHARMASK)||(!xparentOn))&&(a2<320)&&(b2<mapBotEdge))
			if ( ( a2<320 ) && ( b2<mapBotEdge ) )
				{
				if ( tile & PALMASK )
        	   	CopyWithOffset((char far *) &chardata[(ulong)(character)<<6],
						GetVGAScreenAddress(a2,b2),8,8, xparentOn ? SQ_B_TO_S_TRANS : SQ_B_TO_S_OPAQUE,
						charpaloff + ((tile&PALMASK)>>PALBIT)<<4);
				else
      	  	    CopyWithOffset( (char far *)&chardata[(ulong)(character)<<6],
						GetVGAScreenAddress(a2,b2),8,8, xparentOn ? SQ_B_TO_S_TRANS : SQ_B_TO_S_OPAQUE,
						charpaloff );
				}
			}
	VGetMouse (&MouseX,&MouseY);
	ShowMouse ();
}

//=============================================================================

void
	RenderZoomMapBox(struct _gad *gadget,MAP *mapPtr,int plotType, int zoomFactor)
	{
   int a;
   int b;
   int character,data,pal;
	int x,y,xSize,ySize;
	uint far *bufPtr;
	char far *sPtr;

	x = gadget->x;
	y = gadget->y;
	xSize = mapPtr->xSize;
	ySize = mapPtr->ySize;
	bufPtr = mapPtr->buffer + mapXOffset + (xSize * mapYOffset);

   for (a=0;a<mapheight*8;a++)
		{
		sPtr = GetVGAScreenAddress(x,y+a);
    	for (b=0;b<mapwidth*8;b++)
      	{
			if (a < ySize && b < xSize)
				{
        		data = *(bufPtr + (a * xSize) + b);
        		character = data & CHARMASK;

				if(character)
					pal = ((data & PALMASK) >> PALBIT)<<4;
				else
					pal = 0;

        		if (data & HFLIPMASK) character += chars;
        		if (data & VFLIPMASK) character += chars*2;

				data = chardata[character<<6];
				if (data || plotType == 2)
					*sPtr++ = data + pal;
				else
					sPtr++;
			}
		else
			*sPtr++ = 255;
      	}
		}
	}

//=============================================================================

void
	RenderMapBox(struct _gad *gadget,MAP *mapPtr,int plotType)
	{
   int a;
   int b;
   int character,data;
	int x,y,xSize,ySize;
	uint far *bufPtr;
	char far *sPtr;

	SetCharSet( mapPtr->tileset );

	switch(displayMode)
		{
		case DSP_NORMAL:
			if (zoomFactor == 8)
		 		{
				RenderZoomMapBox(gadget,mapPtr,plotType,zoomFactor);
				return;
		 		}

			x = gadget->x;
			y = gadget->y;
			xSize = mapPtr->xSize;
			ySize = mapPtr->ySize;
			bufPtr = mapPtr->buffer + mapXOffset + (xSize * mapYOffset);

    		for (a=0;a<mapheight;a++)
    			for (b=0;b<mapwidth;b++)
      	 		{
					if(a < ySize && b < xSize)
			 		{
        				data = *(bufPtr + (a * xSize) + b);
        				//data = *(mapPtr->buffer + ((a+mapYOffset) * xSize) + (b+mapXOffset));
        				character = data & CHARMASK;
        				if (data & HFLIPMASK) character+=chars;
        				if (data & VFLIPMASK) character +=chars*2;
        				if (data & PALMASK)
							CopyWithOffset((char far *) &chardata[(ulong)character<<6],GetVGAScreenAddress (x+(b<<3),y+(a<<3)),8,8,plotType,(( data & PALMASK)>>PALBIT)<<4);
						else
        					CopyWithOffset((char far *) &chardata[(ulong)character<<6],GetVGAScreenAddress (x+(b<<3),y+(a<<3)),8,8,plotType,charpaloff);
			 		}
					else
						VGASolidBox( x+(b<<3), y+(a<<3), x+(b<<3)+7, y+(a<<3)+7,255);
      	 		}
			break;
		case DSP_COLOR:
			DrawMapColor(0,0,gadget);
			break;
		case DSP_PRI:
			DrawMapPri(0,0,gadget);
			break;
		case DSP_USAGE:
			DrawMapUsage(0,0,gadget);
			break;
	 }
}

//=============================================================================

void
	DrawMapBox(struct _gad *gadget)
	{
	int i;

	if ( multiLayer )
		{
		for ( i=0; i<num_of_maps; ++i )
			if ( !maps[i].fSpare )
				RenderMapBox( gadget, &maps[i],
				i==0 ? SQ_B_TO_S_OPAQUE : SQ_B_TO_S_TRANS );
		}
	else
		RenderMapBox( gadget, mapPtr, 2 );
	}


int DrawMapPri (int x,int y,struct _gad *gadget)
{
    int a;
    int b;
    WaitForLift ();
    HideMouse ();
    gadget=&MapBoxGadget;
    x = gadget->x;
    y = gadget->y;
    for (a=0;a<mapheight;a++)
     for (b=0;b<mapwidth;b++)
        if ((*(mapPtr->buffer + ((a+mapYOffset) * mapPtr->xSize) + (b+mapXOffset)) & PRIBITMASK)!=0)
         VGASolidBox(x+(b<<3),y+(a<<3),x+(b<<3)+7,y+(a<<3)+7,15);
            else
         VGASolidBox (x+(b<<3),y+(a<<3),x+(b<<3)+7,y+(a<<3)+7,0);
//    Message ("Priority Bits",WAIT_AND_EAT,DIMGRAY,MEDGRAY);
    ShowMouse ();
}

int DrawMapColor (int x,int y,struct _gad *gadget)
{
    int a, b;
    int color;

    WaitForLift ();
    HideMouse ();
    gadget=&MapBoxGadget;
    while (VGetMouse(&MouseX,&MouseY)!=0);      // wait for mouse to unclick
    for (a=0;a<mapheight;a++)
     for (b=0;b<mapwidth;b++)
         {
         color = (*(mapPtr->buffer + ((a+mapYOffset) * mapPtr->xSize) + (b+mapXOffset)) >> 13) & 3;
         VGASolidBox(gadget->x+(b<<3),gadget->y+(a<<3),gadget->x+(b<<3)+7,gadget->y+(a<<3)+7,color);
         }
//    Message ("Color Palettes",WAIT_AND_EAT,DIMGRAY,MEDGRAY);
    ShowMouse ();
}


#define WHITE		245
#define LT_GREY	MEDGRAY
#define DK_GREY	DIMGRAY
#define BLACK		240

int
	DrawMapUsage( int x,int y,struct _gad *gadget )
	{
   int a, b;
	int _white = WHITE;
	int _lt_grey = LT_GREY;
	int _dk_grey = DK_GREY;
	int _black = BLACK;

   WaitForLift();
   HideMouse();
   gadget=&MapBoxGadget;
   while (VGetMouse(&MouseX,&MouseY)!=0);      // wait for mouse to unclick
   for (a=0;a<mapheight;a++)
   	for (b=0;b<mapwidth;b++)
      	{
			int tile;
			int usage, color;

         tile = *(mapPtr->buffer + ((a+mapYOffset) * mapPtr->xSize) + (b+mapXOffset));
			usage = mapPtr->usage[tile & CHARMASK];

			// Calculate usage population color
			if ( usage == 1 )
				color = _white;
			else if ( ( usage >= 2 ) && ( usage <= 3 ) )
				color = _lt_grey;
			else if ( ( usage >= 4 ) && ( usage <= 7 ) )
				color = _dk_grey;
			else
				color = _black;

         VGASolidBox(gadget->x+(b<<3),gadget->y+(a<<3),gadget->x+(b<<3)+7,gadget->y+(a<<3)+7,color);
         }
	ShowMouse();
	}

//=============================================================================

void DrawCharBox (struct _gad *gadget)
	{
   int a;
   int b;
   int x,y;

	for (a=0;a<selectheight;a++)
		{
   	for (b=0;(b<selectwidth)&&(a*selectwidth+b<chars);b++)
//      	VGACopy ((char far *) &chardata[(a*selectwidth+b+charoffset)<<6],GetVGAScreenAddress (gadget->x+(b<<3),gadget->y+(a<<3)),8,8,02);
      	CopyWithOffset((char far *) &chardata[(a*selectwidth+b+charoffset)<<6],GetVGAScreenAddress (gadget->x+(b<<3),gadget->y+(a<<3)),8,8,02,charpaloff);
		if (b<selectwidth)
			VGASolidBox( gadget->x+(b<<3), gadget->y+(a<<3), selectwidth<<3, gadget->y+(a<<3)+7,0);
		}

  	if ( a<selectheight )
		VGASolidBox( gadget->x, gadget->y+(a<<3), gadget->x+(selectwidth<<3), gadget->y+(selectheight<<3),0);

	for ( a=0; a<selected.ySize; a++ )
		for ( b=0; b<selected.xSize; b++ )
      	{
        	x = (*(selected.buffer + (selected.xSize * a) + b) - charoffset) % selectwidth;
        	y = (*(selected.buffer + (selected.xSize * a) + b) - charoffset) / selectwidth;
        	if ((x>=0)&&(x<=gadget->width>>3)&&(y>=0)&&(y<=gadget->height>>3))
         	XORBOX (gadget->x+(x<<3),gadget->y+(y<<3),8,8);
			}
	}


//=============================================================================

void
XORChars( int x, int y, int width, int height)
{
	if(width < 0)
	 {
		x += width;
		width = 0-width;
	 }
	if(height < 0)
	 {
		y += height;
		height = 0-height;
	 }
	width++;
	height++;
	if(width != 0 && height != 0)
		VGAXORBox(GetVGAScreenAddress(x<<3,y<<3),width<<3,height<<3);
}

//=============================================================================

void
XORChars2( int x, int y, int width, int height)
{
	if(width < 0)
	 {
		x += width;
		width = 0-width;
	 }
	if(height < 0)
	 {
		y += height;
		height = 0-height;
	 }
	width++;
	height++;
	if(width != 0 && height != 0)
		VGAXORBox(GetVGAScreenAddress(x<<3,(y<<3)+144),width<<3,height<<3);
}

//=============================================================================

void
XORBOX( int x, int y, int width, int height)
{
	if(width < 0)
	 {
		x += width;
		width = 0-width;
	 }
	if(height < 0)
	 {
		y += height;
		height = 0-height;
	 }
	if(width != 0 && height != 0)
		VGAXORBox(GetVGAScreenAddress(x,y),width,height);
}

//=============================================================================

