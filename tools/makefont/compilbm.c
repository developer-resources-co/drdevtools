//=============================================================================
// compilbm.c: grabber version
//=============================================================================

#include <dos.h>

#include "\gfxtools\lib\global.h"
#include "\gfxtools\lib\gfx.h"
#include "ilbmread.h"
#include "stdio.h"
#include "stdlib.h"
#include "alloc.h"
#include "\gfxtools\lib\general.h"
#include "grabber.h"

int charHeight,height,width,pixelWidth;
int nPlanes;							// # of planes in the image
int nPlanesTotal;						// including mask

extern FLAG displayOn;
struct _pal {char red,green,blue;};
struct _pal pal[256];
unsigned char far *image[1000];


int DoColorMap (char huge *chunkpointer,unsigned long chunksize)
{
  int a;
  unsigned char red,green,blue;
   for (a=0;a<chunksize/3;a++)
   {
    pal[a].red =   (*(chunkpointer++))>>2;
    pal[a].green = (*(chunkpointer++))>>2;
    pal[a].blue =  (*(chunkpointer++))>>2;
   }
   if(displayOn)
   	SetPal ();
}

SetPal ()
{
  struct REGPACK reg;
  reg.r_ax=0x1012;
  reg.r_bx=0;
  reg.r_cx=256;
  reg.r_dx=FP_OFF(pal);
  reg.r_es=FP_SEG(pal);
  intr (0x10,&reg);
}

int compression;
struct BitmapHeader *bmhd;


/* IFF format stores word and longs in 68000 format */
/* So these routines swap the low and high parts */
unsigned int swaphilo (unsigned int a)
{
 return (((a & 0xff)<<8) + (a >> 8));
}

unsigned long swaplong (unsigned long a)
{
 unsigned int b;
 unsigned int c;
 b = a >> 16;
 c = a & 0xffff;
 b = swaphilo (b);
 c = swaphilo (c);
 return (((unsigned long) c <<16) + b);
}



#define mskNone 0
#define mskHasMask 1
#define mskHasTransparentColor 2
#define mskLasso 3

/* process the header.  Check for compression and size */
int DoBitmapHeader (char huge *chunkpointer,unsigned long chunksize)
{
	int i;
	unsigned int xw,yw;
	bmhd = (struct BitmapHeader *) chunkpointer;
	compression = bmhd->compression;
	xw = swaphilo (bmhd->w);
	yw = swaphilo (bmhd->h);
	width = xw/8;										// note: this means any file not character bound on the x axis will get the last pixels truncated
	pixelWidth = xw;
	height = yw;
	nPlanes = bmhd->nplanes;
	charHeight = yw/8 + !!(yw & 7);					// add one if map is not character bound
	if (bmhd->masking == mskHasMask || bmhd->masking > mskLasso)
		{
		nPlanes = bmhd->nplanes-1;
		nPlanesTotal = bmhd->nplanes;
		Error ("Picture has a mask of some sort...try shutting off stencil option.");
		}
	if(nPlanes > 8)
		Error ("Picture has too many bit planes(8 max)");

	for(i=0;i<height;i++)
		image[i] = SafeFarMalloc(pixelWidth);

	//image = SafeFarMalloc(((unsigned long)pixelWidth)*height);
	//image = SafeFarMalloc(70000);
}


/* Turbo C's putimage format.  used for faster displaying. */
struct scanlineimage
{
 int xsize,ysize;
 unsigned char data[8][2048/8];		//1024/8];
 int pad;
};

struct scanlineimage sl;


/* uncompresses run-length data */
char huge *
	UncompressScanLine (char *destination,char huge *source)
	{
	int column=0;
	int oldcolumn;
	signed char commandbyte,data;

	if (!compression)
		{
		if ( destination )
			{
		   for (column=0;column<width;column++)
				*(destination+column)=*(source++);
			}
		else
			source += width;
		}
	else
		{
		while (column<width)
			{
			oldcolumn = column;
			commandbyte = *source++;
			if ( commandbyte < 0 )
				{
				if ( destination )
					{
					data = *(source++);
					while (column<oldcolumn-commandbyte+1)
						*(destination+column++)=data;
					}
				}
			else
				{
				if ( destination )
					{
					while (column<oldcolumn+commandbyte+1)
						if ( destination )
							*(destination+column++)=*(source++);
						else
							++source;
					}
				}
			}
		}
	return (source);
	}



PutPixel(unsigned long x,unsigned long y,unsigned char pix)
{
	if(displayOn && x < 320 && y < 200)
    	VGAPutPixel(x,y,pix);
	*(image[y]+x) = pix;
}


unsigned char
GetPixel(unsigned long x,unsigned long y)
{
	return(*(image[y]+x));
}

DisplayScanLine (int row)
{
 int a,c;
 unsigned char b;
 unsigned char bitshift,mask;
 for (a=0;a<pixelWidth;a++)
  {
    bitshift = 7-(a%8);
    mask = 1<<bitshift;
    b =  (((sl.data[7][a/8])&mask)>>bitshift);
    b += (((sl.data[6][a/8])&mask)>>bitshift)<<1;
    b += (((sl.data[5][a/8])&mask)>>bitshift)<<2;
    b += (((sl.data[4][a/8])&mask)>>bitshift)<<3;
    b += (((sl.data[3][a/8])&mask)>>bitshift)<<4;
    b += (((sl.data[2][a/8])&mask)>>bitshift)<<5;
    b += (((sl.data[1][a/8])&mask)>>bitshift)<<6;
    b += (((sl.data[0][a/8])&mask)>>bitshift)<<7;
	PutPixel(a,row,b);
    //SQ_put_pixel (a,row,b);
  }
}



/* Process the ILBM BODY chunk. */
int DoILBMBody (char huge *chunkpointer,unsigned long chunksize)
{
 int row;
 int a,b;
 sl.xsize = (width*8)-1;
 sl.ysize = 1;
 sl.pad = 0;

 for(a = 0;a<8;a++)
	for(b=0;b<2048/8;b++)
		sl.data[a][b] = 0;					// clear scanLineImage in case we have fewer than 8 bit-planes
 for (row = 0; row < height; row ++)
  {
      for (a=0;a<nPlanes;a++)
         chunkpointer = UncompressScanLine (sl.data[7-a],chunkpointer);
      DisplayScanLine (row);
  }
}



/* General purpose IFF reading. Chunk "preprocessing" */

DoIff (FILE *fp,int (*ParseFunction) (char huge *chunkpointer,unsigned long chunksize),unsigned long size)
{
char huge *pointertodata;
pointertodata = SafeFarMalloc (size);
if (size > 0x0000ffff)
    {
     fread (pointertodata          ,size>>1,1,fp);
     fread (pointertodata+(size>>1),size>>1,1,fp);
    }
 else
     fread (pointertodata,size,1,fp);
ParseFunction (pointertodata,size);
farfree ((void far *)pointertodata);
}


/* Find chunks and get sizes.  */
ReadILBM (char *filename)
{
 unsigned long size;
 long a;
 FILE *fp;
 char temp[15];
 if ((fp = fopen (filename,"rb"))==NULL)
   {
   sprintf (temp,"IFF Error, file not found: %s",filename);
   Error (temp);
   }
 do
 {
 a = DEAD;
 fread (&a,4,1,fp);
 fread (&size,4,1,fp);
 size = swaplong (size);
 if (size&1) size++;
 if (a==CMAP) DoIff (fp,DoColorMap,size);
 else if (a==BODY) DoIff (fp,DoILBMBody,size);
 else if (a==BMHD) DoIff (fp,DoBitmapHeader,size);
 else if (a==FORM) fseek (fp,4,1);
 else fseek(fp,size,1);
 }
 while (a!=0&&size!=0&&!feof(fp));
 fclose (fp);
}

