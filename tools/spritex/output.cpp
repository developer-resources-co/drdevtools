/*
 * output.c -- create Lsprite / Hsprite format.
 * (c) 1991 Developer Resources, Scott Statton
 */

//=============================================================================

// pclib
#include <pclib\general.h>
#include <pclib\iff.h>
#include <pclib\grphport.h>

// local
#include "menu.hpp"
#include "sprite.hpp"

#include "global.hpp"
#include "utypes.hpp"
#include "gadget.hpp"
#include "input.hpp"
#include "output.hpp"
#include "spmain.hpp"
#include "general.hpp"
#include "spgadget.hpp"

//=============================================================================

/*
 * flip tables
 */

#define NONE 	0
#define XNOY 	1
#define YNOX 	2
#define XANDY 	3

static char fliptable[64][16] = {

/* first, the no flips */

/* 1x1 */		{ 0 } ,
/* 1x2 */		{ 0, 1 } ,
/* 1x3 */		{ 0, 1, 2 } ,
/* 1x4 */		{ 0, 1, 2, 3 },
/* 2x1 */		{ 0, 1 },
/* 2x2 */		{ 0, 1, 2, 3 } ,
/* 2x3 */		{ 0, 1, 2, 3, 4, 5 },
/* 2x4 */		{ 0, 1, 2, 3, 4, 5, 6, 7 },
/* 3x1 */		{ 0, 1, 2, } ,
/* 3x2 */		{ 0, 1, 2, 3, 4, 5 },
/* 3x3 */		{ 0, 1, 2, 3, 4, 5, 6, 7, 8 },
/* 3x4 */		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
/* 4x1 */		{ 0, 1, 2, 3 },
/* 4x2 */		{ 0, 1, 2, 3, 4, 5, 6, 7 },
/* 4x3 */		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
/* 4x4 */		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },

/* now, flip x, but not y */

/* 1x1 */		{ 0 } ,
/* 1x2 */		{ 0, 1 } ,
/* 1x3 */		{ 0, 1, 2 } ,
/* 1x4 */		{ 0, 1, 2, 3 },
/* 2x1 */		{ 1, 0 },
/* 2x2 */		{ 2, 3, 0, 1 } ,
/* 2x3 */		{ 3, 4, 5, 0, 1, 2 },
/* 2x4 */		{ 4, 5, 6, 7, 0, 1, 2, 3 },
/* 3x1 */		{ 0, 1, 2, } ,
/* 3x2 */		{ 4, 5, 2, 3, 0, 1 },
/* 3x3 */		{ 6, 7, 8, 3, 4, 5, 0, 1, 2},
/* 3x4 */		{ 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3 },
/* 4x1 */		{ 0, 1, 2, 3 },
/* 4x2 */		{ 4, 5, 6, 7, 0, 1, 2, 3 },
/* 4x3 */		{ 9, 10, 11, 6, 7, 8, 3, 4, 5, 0, 1, 2 },
/* 4x4 */		{ 12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3 },

/* now, flip y, but not x */

/* 1x1 */		{ 0 } ,
/* 1x2 */		{ 1, 0 } ,
/* 1x3 */		{ 2, 1, 0 } ,
/* 1x4 */		{ 3, 2, 1, 0 },
/* 2x1 */		{ 0, 1 },
/* 2x2 */		{ 1, 0, 3, 2 } ,
/* 2x3 */		{ 2, 1, 0, 5, 4, 3 },
/* 2x4 */		{ 3, 2, 1, 0, 7, 6, 5, 4 },
/* 3x1 */		{ 0, 1, 2 } ,
/* 3x2 */		{ 1, 0, 3, 2, 5, 4 },
/* 3x3 */		{ 2, 1, 0, 5, 4, 3, 8, 7, 6 },
/* 3x4 */		{ 3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8 },
/* 4x1 */		{ 0, 1, 2, 3 },
/* 4x2 */		{ 1, 0, 3, 2, 5, 4, 7, 6 },
/* 4x3 */		{ 2, 1, 0, 5, 4, 3, 8, 7, 6, 11, 10, 9 },
/* 4x4 */		{ 3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12 },

/* flip both x and y */

/* 1x1 */		{ 0 } ,
/* 1x2 */		{ 1, 0 } ,
/* 1x3 */		{ 2, 1, 0 } ,
/* 1x4 */		{ 3, 2, 1, 0 },
/* 2x1 */		{ 1, 0 },
/* 2x2 */		{ 3, 2, 1, 0} ,
/* 2x3 */		{ 5, 4, 3, 2, 1, 0 },
/* 2x4 */		{ 7, 6, 5, 4, 3, 2, 1, 0 },
/* 3x1 */		{ 2, 1, 0 } ,
/* 3x2 */		{ 5, 4, 3, 2, 1, 0 },
/* 3x3 */		{ 8, 7, 6, 5, 4, 3, 2, 1, 0 },
/* 3x4 */		{ 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 },
/* 4x1 */		{ 3, 2, 1, 0 },
/* 4x2 */		{ 7, 6, 5, 4, 3, 2, 1, 0 },
/* 4x3 */		{ 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 },
/* 4x4 */		{ 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 } };





byte charFlip[4][32] = 
	{
	/* no x, no y */

		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 
			16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 },

	/* x flip, y normal */

		{ 3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 7, 15, 14, 13, 12, 
			19, 18, 17, 16, 23, 22, 21, 20, 27, 26, 25, 24, 31, 30, 29, 28 },

	/* x normal, y flip */

		{ 28, 29, 30, 31, 24, 25, 26, 27, 20, 21, 22, 23, 16, 17, 18, 19,
			12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3 } ,

	/* x flip, y flip */

		{ 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,
			15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 } };

//=============================================================================


#define XFLIP 0x800
#define YFLIP 0x1000

#define FLIPENABLED

unsigned char chars[32768];
word currChar;

extern char *chrSaveName;
extern char *sprSaveName;
extern boolean chrNameboolean;
extern struct VIEWPORT subScreen;

//=============================================================================

byte xsize[] = {  8,  8,  8,  8, 16, 16, 16, 16, 24, 24, 24, 24, 32, 32, 32, 32 };
byte ysize[] = {  8, 16, 24, 32,  8, 16, 24, 32,  8, 16, 24, 32,  8, 16, 24, 32 };

FILE *fp2;

//=============================================================================

word
Swab(word a)
{
	return (( ( a & 0xff ) << 8 ) | ( a >> 8 ));
}

void
WriteLogicalSprite(struct _lSprite *lsPtr)
{
	struct _lSprite llSprite;

	llSprite = (*lsPtr);
	llSprite.yOffset = Swab(llSprite.yOffset);
	llSprite.xOffset = Swab(llSprite.xOffset);
	llSprite.xSize = Swab(llSprite.xSize);
	llSprite.ySize = Swab(llSprite.ySize);
	llSprite.spriteCount = Swab(llSprite.spriteCount);
	fwrite((char *)(&llSprite),sizeof(llSprite),1,fp2);
	return;
}

//=============================================================================

struct _lSprite *
CreateLogicalSprite(void)
{
	/* based on the data structure in spritBase, find the top-left
		corner, the bottom right corner, and the number of physical
		sprites in the image.	*/

	struct PANE *sPtr;
	struct _lSprite *lsPtr;

	word	left, top, bottom, right;

	word	count = 0;

	left = top = 0xffff;
	bottom = right = 0;

	lsPtr = (struct _lSprite *)malloc((unsigned long)sizeof(struct _lSprite));

	sPtr = spriteBase.next;

	while ( sPtr )
	{
		left = ( sPtr->x < left ? sPtr->x : left );
		top = ( sPtr->y < top ? sPtr->y : top );
		bottom = ( (sPtr->y+sPtr->sizey) > bottom ? (sPtr->y+sPtr->sizey) : bottom );
		right = ( (sPtr->x + sPtr->sizex) > right ? (sPtr->x + sPtr->sizex) : right );
		count++;
		sPtr= sPtr->next;
	}

	lsPtr->xOffset = graphic.upLeftX;
	lsPtr->yOffset = graphic.upLeftY;
	lsPtr->xSize  = graphic.lowRightX - graphic.upLeftX;
	lsPtr->ySize  = graphic.lowRightY - graphic.upLeftY;
	lsPtr->hardXSize = right - left;
	lsPtr->hardYSize = bottom - top;
	lsPtr->spriteCount = count;
	WriteLogicalSprite(lsPtr);
	return lsPtr;
}

//=============================================================================

void
WriteHardSprite (struct _hSprite *hsPtr)
{
	struct _hSprite lhSprite;

	lhSprite = (*hsPtr);	/* struct copy */
	lhSprite.yOffset = Swab(lhSprite.yOffset);
	lhSprite.startChar = Swab(lhSprite.startChar);
	lhSprite.xOffset = Swab(lhSprite.xOffset);
	fwrite((char *)(&lhSprite),sizeof(lhSprite),1,fp2);
	return;
}

//=============================================================================

byte
sizebyte ( word y, word x)
{
	byte foo = 0;

	foo = ( y / 8 ) - 1;
	foo |=(( ( x / 8 ) - 1) << 2);
	return foo;
}

//=============================================================================

/*
 * d	destination
 * s	source
 * xf	x flip boolean
 * yf	y flip boolean
 *
 */

void
MoveChar ( char *d, char *s, boolean xf, boolean yf )
{
	int i;
	char ff;

	ff = ( xf ? 1 : 0 ) | ( yf ? 2 : 0 );

	for ( i = 0 ; i < 32 ; i ++ )
		d[charFlip[ff][i]] = *s++;
	return;
}

//=============================================================================

void
FlipSprite(char *d, char *s, int xs, int ys, boolean xf, boolean yf)

/*
 * d == destination array
 * s == source array
 * xs == xsize in characters (1..4)
 * ys == ysize in characters (1..4)
 * xf == xflip ( FALSE: no flip, TRUE: flip )
 * yf == yflip ( FALSE: no flip, TRUE: flip )
 */

{
	char ff;
	int numchars,i;

	numchars = xs * ys;
#ifndef FLIPENABLED
	for ( i = 0 ; i < numchars*32 ; i++ )
		*d++ = *s++;
#endif

#ifdef FLIPENABLED
	ff = ( xf ? 16 : 0 ) | ( yf ? 32 : 0 ) | (( ys -1 )<<2) | ( xs-1 );

	for ( i = 0 ; i < numchars ; i ++ )
		MoveChar ( d+fliptable[ff][i]*32,s+i*32,xf,yf);
#endif
	return;
}

//=============================================================================

boolean
CompChar(int i, char *c, int xs, int ys )
{
	int	b;	/* # of bytes to compare */

	int j;

	b = xs * ys * 32;	/* size in chars * # of bytes in char */

	for ( j = 0 ; j < b ; j++ )
		if ( chars[i+j] != c[j] )
			return boolean::FALSE;
	return boolean::TRUE;
}

//=============================================================================

boolean
Unique(struct _hSprite *hsPtr, byte *hc, int currChar)
{
	byte flip[512];
	boolean xf;
	boolean yf;
	short xs, ys; int i,j;
	int k,l;

	xs = xsize[hsPtr->size]/8;
	ys = ysize[hsPtr->size]/8;


	for ( xf = boolean::FALSE , k = 0 ; k < 2 ; k++, xf = !xf )
		for ( yf = boolean::FALSE , l = 0 ; l < 2 ; l++, yf = !yf ) {
			FlipSprite(flip,hc, xs, ys, xf, yf);
			for ( i = 0 ; i < currChar ; i++ )
				if (CompChar(i*32, flip, xs, ys)) {
					hsPtr->startChar= i | ( yf ? YFLIP : 0 ) | ( xf ? XFLIP : 0 );
					return boolean::FALSE;
		 		}
		}
	hsPtr->startChar = i;
	for ( i = 0 , j = xs*ys*32 ; i < j ; i++ )
		chars[i+currChar*32] = hc[i];
	return boolean::TRUE;
}

//=============================================================================

void
MemWriteChar ( short x, short y , char far *buffer)
{
	short i,j,mod;
	char huge *bitMap;
	char byte;
	char huge *endBuffer;

	endBuffer = graphic.imagePtr+graphic.sizeX * graphic.sizeY;

	if (graphic.flag)
	 {
		bitMap = (char huge *) graphic.imagePtr;

		bitMap += ( y * graphic.sizeX ) + x;
		mod = graphic.sizeX - 8;
		for ( j = 0 ; j < 8 ; j ++ )
		{
			for ( i = 0 ; i < 4 ; i ++ )
			{
				if(bitMap < endBuffer)
				 {
					byte = (((*bitMap++)&0xf)<<4);
					byte |= ((*bitMap++)&0xf) ;
				 }
				else
					byte = 0;
				*buffer++ = byte;
			}
			bitMap += mod;
		}
	 }
	return;
}

//=============================================================================

void
BuildChars(struct _hSprite *hsPtr, struct _lSprite *lsPtr, char *hc)
{
	int x, y;
	int xs, ys;
	int xt, yt;

	xs = hsPtr->xOffset+lsPtr->xOffset;
	ys = hsPtr->yOffset+lsPtr->yOffset;
	xt = xsize[hsPtr->size]+xs;
	yt = ysize[hsPtr->size]+ys;

	for ( x = xs ; x < xt ; x+= 8 )
		for ( y = ys ; y < yt ; y += 8 ) {
			MemWriteChar(x, y, hc);
			hc += 32;
		}
	return;
}

//=============================================================================

boolean
LoadChrData()
{
	int	numChars;
	FILE *fp;
//	chrNameboolean = boolean::FALSE;
	fp = fopen(chrSaveName,"rb");
	if (fp)
	 {
		fseek(fp,0,2);	/* seek to EOF */
		if(ftell(fp) & 31)
		 {
			MessageBox(&subScreen,239, 255, "Not a valid CHR file");
			fclose(fp);
			return(boolean::FALSE);
		 }
		numChars = ftell(fp) / 32;	/* # of bytes per character */
		fseek(fp,0,0);	/* seek to BOF */
		for ( currChar = 0 ; currChar < numChars ; currChar++ )
			fread(chars+(currChar*32),32,1,fp);
		fclose(fp);
//		chrNameboolean = boolean::TRUE;
     }
	return(boolean::TRUE);
}

//=============================================================================

void
CreateHardwareSprite(struct _lSprite *lsPtr)
{
	/* now, go through the data again, creating hardware sprite
	   records based on the lsprite data */

	struct PANE *sPtr;

	struct _hSprite hs;
	char hc[512];
	int	numChars;
	FILE *fp;

	currChar = 0;
	/* if there's a current .CHR file, load those characters in */

	if (chrNameboolean)
		if(!LoadChrData())
			return;

	sPtr = spriteBase.next;

	while ( sPtr )
	{
		hs.yOffset	= sPtr->y - lsPtr->yOffset;
		hs.size		= sizebyte( sPtr->sizey , sPtr->sizex );
		hs.pad		= 0;
		hs.startChar = 0;
		hs.xOffset = sPtr->x - lsPtr->xOffset;
		BuildChars(&hs, lsPtr, hc);
		if (Unique(&hs, hc, currChar))
			currChar += (sPtr->sizey/8) * (sPtr->sizex /8);
		sPtr = sPtr->next;
		WriteHardSprite(&hs);
	}
	return;
}

//=============================================================================

void
WriteChar ( short x, short y )
{
	short i,j,mod;
	char far *bitMap = NULL;
	char byte;

	if (graphic.flag)
		bitMap = (char far *) graphic.imagePtr;

	bitMap += ( y * graphic.sizeX ) + x;
	mod = graphic.sizeX - 8;
	for ( j = 0 ; j < 8 ; j ++ )
	{
		for ( i = 0 ; i < 4 ; i ++ )
		{
			byte = ((*(bitMap++)&0xf)<<4) | (*(bitMap++)&0xf) ;
			fputc(byte,fp2);
		}
		bitMap += mod;
	}
	return;
}

//=============================================================================

void SaveSprite( void )
	{
	if (graphic.flag)
		{
		struct _lSprite *lsPtr;

#if 0
		srcFile = new SourceOutput( sprSaveName, ios::out|ios::binary );


		delete srcFile;
#endif

		fp2 = fopen (sprSaveName,"wb");
		CreateHardwareSprite(lsPtr = CreateLogicalSprite());
		fclose(fp2);

		if (!chrNameboolean) {
			strcpy (chrSaveName,sprSaveName);
			strcpy(FindChar(chrSaveName,'.'),".chr");
			}
		fp2 = fopen (chrSaveName,"wb");
		fwrite(chars,currChar*32,1,fp2);
		fclose(fp2);
		}
	}

//=============================================================================

