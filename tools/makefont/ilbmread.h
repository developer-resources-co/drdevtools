#include "stdio.h"

#define FORM 0x4d524f46
#define ILBM 0x4d424c49
#define CMAP 0x50414d43
#define BMHD 0x44484d42
#define BODY 0x59444f42
#define DEAD 0x00000000

struct BitmapHeader
{
 unsigned int w,h;
 int x,y;
 unsigned char nplanes;
 unsigned char masking;
 unsigned char compression;
 unsigned char pad;
 unsigned int transparentColor;
 unsigned char xaspect,yaspect;
 int pagewidth, pageheight;
 };

int DoColorMap (char huge *chunkpointer,unsigned long chunksize);
unsigned int swaphilo (unsigned int a);
unsigned long swaplong (unsigned long a);
int DoBitmapHeader (char huge *chunkpointer,unsigned long chunksize);
char huge *UncompressScanLine (char *destination,char huge *source);
int DisplayScanLine (int row);
int DoILBMBody (char huge *chunkpointer,unsigned long chunksize);
int DoIff (FILE *fp,int (*ParseFunction) (char huge *chunkpointer,unsigned long chunksize),unsigned long size);
int ReadILBM (char *filename);
