//==============================================================================
// snes.cpp:
//==============================================================================

#include "grabber.hpp"

#define CHAR_FLIPX 0x4000
#define CHAR_FLIPY 0x8000

extern int charPlanes;
extern boolean mode7,sixteenBySixteen;
extern int paloffset;

//==============================================================================

//int planeOffset[16] = {0,1,16,17,32,33,48,49, 64,65,80,81,96,97,112,113};
int planeOffset[8] = {0,1,16,17,32,33,48,49};

static int maskArray[1+8] = {0, 1,3,7,0xf,0x1f,0x3f,0x7f,0xff};
												// 0, to offset by charPlanes-1

//==============================================================================

boolean
	Make7Char(int x, int y)
	{
	int c,d;

	for (c=0;c<CHAR_Y;c++)
		for (d=0;d<CHAR_X;d++)
		 	tempchar[(c*8)+d] = GetPixel(x+d,y+c);
	return(boolean::TRUE);
	}

//==============================================================================

xTab[4] = { 0,CHAR_X,0,CHAR_X };
yTab[4] = { 0,0,CHAR_Y,CHAR_Y };


void
	CreateYFlip( void )
	{
	int x,planes,p;

	for(planes=0;planes<charPlanes;planes++)
		for(x=0;x<16;x+=2)
			{
			p = planeOffset[planes];
			tempcharY[p+(14-x)]  = tempchar[p+x];
			}
	}

//==============================================================================

void
	CreateXFlip( void )
	{
	int i,temp,temp2;

//	for(i=0;i<8*charPlanes;i++)
	for(i=0;i<charSize;i++)
		{
		temp = tempchar[i];
		temp2 = (temp & 0x1)<<7;
		temp2 |= (temp & 0x2)<<5;
		temp2 |= (temp & 0x4)<<3;
		temp2 |= (temp & 0x8)<<1;
		temp2 |= (temp & 0x10)>>1;
		temp2 |= (temp & 0x20)>>3;
		temp2 |= (temp & 0x40)>>5;
		temp2 |= (temp & 0x80)>>7;
		tempcharX[i] = temp2;
		}
	}

//==============================================================================

void
	CreateXYFlip()
	{
	int i,temp,temp2;

//	for(i=0;i<8*charPlanes;i++)
	for(i=0;i<charSize;i++)
		{
		temp = tempcharY[i];
		temp2 = (temp & 0x1)<<7;
		temp2 |= (temp & 0x2)<<5;
		temp2 |= (temp & 0x4)<<3;
		temp2 |= (temp & 0x8)<<1;
		temp2 |= (temp & 0x10)>>1;
		temp2 |= (temp & 0x20)>>3;
		temp2 |= (temp & 0x40)>>5;
		temp2 |= (temp & 0x80)>>7;
		tempcharXY[i] = temp2;
		}
	}

boolean
	MakeChar(int x, int y)
	{
	int yOff,xOff,i,pal,o,outCount;
	unsigned char planes[8],temp;
	boolean good = boolean::TRUE;

	if(mode7)
		return(Make7Char(x,y));

	pal = 0xFFFF;
	if(sixteenBySixteen)
		outCount = 4;
	else
		outCount = 1;

	for (yOff=0;yOff<CHAR_Y;yOff++)
	 {
		for(i=0;i<charPlanes;i++)
			planes[i] = 0;

		for(o=0;o<outCount;o++)
		 {
    		for (xOff=0;xOff<CHAR_X;xOff++)
		 	 {
		 		temp = GetPixel(x+xOff+xTab[o],y+yOff+yTab[o]);

				if(charPlanes < 8)									// don't bother checking palettes if in 256 color mode
			 	 {
					if(pal == 0xFFFF)
			 	 	 {
						if ( temp )
							pal = temp >> (8-charPlanes);
			 	 	 }
					else
			 	 	 {
						if (temp & maskArray[charPlanes]) 		// check for color zero
				 	 	 {
			 				if (pal != temp >> (8-charPlanes) )
								good = boolean::FALSE;
				 	 	 }
						else
							if(temp > maskArray[charPlanes])		// if color zero of some other pallete, incorrect
								good = boolean::FALSE;
			 	 	 }
					temp &= maskArray[charPlanes];
			 	 }

				for(i=0;i<charPlanes;i++)
			 	 {
					planes[i] <<= 1;
					planes[i] |= temp & 1;
					temp >>= 1;
			 	 }
		 	 }
			for(i=0;i<charPlanes;i++)
				tempchar[(yOff*2)+(planeOffset[i])] = planes[i];
//??16		tempchar[(yOff*2)+(planeOffset[i])+(charPlanes*8*o)] = planes[i];

		 }
	 }
	if(pal == 0xffff)
		palette = 0;
	else
		palette = pal;

	palette += paloffset;
	if ( palette > 8 ) Error( 10, "Palette overflow!\n" );

	palette <<= 10;

	CreateXFlip();
	CreateYFlip();
	CreateXYFlip();
	return(good);
}

//==============================================================================

