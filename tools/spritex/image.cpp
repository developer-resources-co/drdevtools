//==============================================================================
// Image.c -  Image handling routines.
//==============================================================================

// pclib
#include <pclib\general.h>
#include <pclib\iff.h>
#include <pclib\grphport.h>

// local
#include "menu.hpp"
#include "sprite.hpp"

#include <stdlib.h>

#include "utypes.hpp"
#include "global.hpp"
#include "sprite.hpp"
#include "sq.hpp"
#include "compilbm.hpp"
#include "spmain.hpp"
#include "image.hpp"
#include "scolor.hpp"

//==============================================================================

extern char iffLoadName;
extern struct GRAPHIC graphic;
extern struct VIEWPORT subScreen;
extern struct PANE spriteBase;
extern boolean spriteBorder;

//==============================================================================

void
LoadImage(void)
{
	if (graphic.flag) {
		display->SetColorIndex( 0 );
		display->Box( point(0,0), point(254,199) );
//		SQ_solid_box( 0,0,254,199,0 );
		if ( graphic.imagePtr ) farfree(graphic.imagePtr), graphic.imagePtr = NULL;
		}

#if 1
	ReadPict( &iffLoadName );
		{
		graphic.imagePtr = (char far*)myPicture.image;
		NewDrawClippedBitmap( &myPicture, &subScreen, 0, 0 );
		graphic.flag = boolean::TRUE;
		graphic.upLeftX = 0;
		graphic.upLeftY = 0;
		graphic.lowRightX = graphic.sizeX;
		graphic.lowRightY = graphic.sizeY;
		graphic.midFlag = boolean::TRUE;
//		FixColors();
//		RefreshImage();
		}
#else
	iffLoadBuffer = NULL;
	ReadILBM(&iffLoadName);
	if (iffLoadBuffer)
	{
		graphic.imagePtr = iffLoadBuffer;
		graphic.sizeX = width*8;
		graphic.sizeY = height;
		DrawClippedBitmap(&graphic, &subScreen, 0, 0 );
		graphic.flag = boolean::TRUE;
		graphic.upLeftX = 0;
		graphic.upLeftY = 0;
		graphic.lowRightX = width*8;
		graphic.lowRightY = height;
		graphic.midFlag = boolean::TRUE;
		FixColors();
		RefreshImage();
	}
#endif
}

//==============================================================================

void
NewDrawClippedBitmap( Animation* anmPtr,
	struct VIEWPORT *vpPtr, word offsetx, word offsety )
{
	char far *screenBuffer;
	char far *imagePtr;

	word	screenXMod;		/* wrap offset in screen */
	word	imageXMod;		/* wrap offset in image */
	word	xsize;			/* actual size to draw */
	word	ysize;
	int	i,j;				/* misc */


	for ( int y=0; y<grPtr->sizeY; ++y )
		{
		for ( int x=0; x<grPtr->sizeX; ++x )
			{
			int i = oldGetPixel( grPtr->image, x, y );
			display->SetColorIndex( i );
			display->PutPixel( point(x,y) );			
			}
		}

/*
	if ( nParseLine < 200 )
		{
		for ( int x=0; x<320 && x<myPicture.bmhd.w; ++x )
			{
			int i = oldGetPixel( myPicture.image, x, nParseLine );
			display->SetColorIndex( i );
			display->PutPixel( point(x,nParseLine) );
			}
		}

	imagePtr = grPtr->imagePtr;

	screenBuffer = SQ_get_screen_address(vpPtr->left,vpPtr->top);

	xsize = min(vpPtr->right-vpPtr->left , grPtr->sizeX );

	imageXMod = grPtr->sizeX - xsize;
	screenXMod = vpPtr->sizeX - xsize;

	ysize = min(grPtr->sizeY, vpPtr->bottom - vpPtr->top);

	imagePtr += offsetx;	/* move forward in graphic */
	imagePtr += (offsety * grPtr->sizeX);	/* move down */

	while ( ysize-- )
	{
		j = xsize;
		while ( j-- )
			*screenBuffer++ = *imagePtr++;
		imagePtr += imageXMod;
		screenBuffer += screenXMod;
	}
*/
}


void
DrawClippedBitmap
	(struct GRAPHIC *grPtr, struct VIEWPORT *vpPtr, word offsetx, word offsety)
{
	char far *screenBuffer;
	char far *imagePtr;

	word	screenXMod;		/* wrap offset in screen */
	word	imageXMod;		/* wrap offset in image */
	word	xsize;			/* actual size to draw */
	word	ysize;
	int		i,j;			/* misc */

	imagePtr = grPtr->imagePtr;

	screenBuffer = SQ_get_screen_address(vpPtr->left,vpPtr->top);

	xsize = min(vpPtr->right-vpPtr->left , grPtr->sizeX );

	imageXMod = grPtr->sizeX - xsize;
	screenXMod = vpPtr->sizeX - xsize;

	ysize = min(grPtr->sizeY, vpPtr->bottom - vpPtr->top);

	imagePtr += offsetx;	/* move forward in graphic */
	imagePtr += (offsety * grPtr->sizeX);	/* move down */

	while ( ysize-- )
	{
		j = xsize;
		while ( j-- )
			*screenBuffer++ = *imagePtr++;
		imagePtr += imageXMod;
		screenBuffer += screenXMod;
	}
}
/* backup
	for ( i = 0 ; i < ysize ; i++ )
	{
		for ( j = 0 ; j < xsize ; j++ )
			*screenBuffer++ = *imagePtr++;
		imagePtr += imageXMod;
		screenBuffer += screenXMod;
	}
	*/

void
PlusSign(int x, int y)
	{
	display->SetColorIndex( 0xEF );
	display->HLine( point(max(0,x-1),y), (min(255,x+1))-(max(0,x-1))+1 );
//	SQ_hline (max (0,x-1),min(255,x+1),y,0xef);
	display->VLine( point(x,max(0,y-1)), (min(199,y+1))-(max(0,y-1))+1 );
//	SQ_vline (x, max (0,y-1),min(199,y+1),0xef);
	return;
}

//==============================================================================

void
RefreshImage()
{
	struct PANE *pPtr = spriteBase.next;
	if (spriteBorder)
	{
		while (pPtr) {
			ClipBox( pPtr->x - graphic.offsetX, pPtr->y,
				pPtr->sizex-1,
				pPtr->sizey-1,&subScreen, 0);
				pPtr = pPtr->next;
		}
	}
	if (graphic.flag )
	{
		NewDrawClippedBitmap
			(&graphic,  &subScreen, graphic.offsetX, graphic.offsetY);
	}


	pPtr = spriteBase.next;

	while ( pPtr )
	{
		DrawPermSpritePane ( pPtr->x - graphic.offsetX , pPtr->y ,
			 pPtr->sizex, pPtr->sizey );
		pPtr = pPtr->next;
	}
	if (graphic.flag && graphic.midFlag) {
		int tempx = graphic.upLeftX - graphic.offsetX;
		int tempy = graphic.upLeftY;
		int tempx1 = graphic.lowRightX - graphic.offsetX -1;
		int tempy1 = graphic.lowRightY-1;

		ClipBox(tempx, tempy, tempx1-tempx, tempy1-tempy, &subScreen, 0xf7 );
		}

	return;
}

//==============================================================================
