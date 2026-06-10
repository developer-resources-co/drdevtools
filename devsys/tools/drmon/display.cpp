//=============================================================================
// display.c: display handling(built on screens)
//=============================================================================

#define INCL_BASE

#include "moninc.hpp"
#include "display.hpp"


//=============================================================================

#ifdef __OS2__
#include <bse.h>
char _far16 *scrBuffer;
#else
char far* scrBuffer;
#endif

char far *scrBuffer2 = NULL;
unsigned int displayWidth,displayHeight;
uint virtualXOffset,virtualYOffset;
uint virtualXSize = 0,virtualYSize = 0;

//=============================================================================

void
DisplayPointer(void)
{
	if
	 (
		inBase.mouseX >= virtualXOffset &&
		inBase.mouseX < screenWidth+virtualXOffset &&
		inBase.mouseY >= virtualYOffset &&
		inBase.mouseY < screenHeight+virtualYOffset
	 )
		DrawPointer(scrBuffer,inBase.mouseX-virtualXOffset,inBase.mouseY-virtualYOffset,screenWidth,screenHeight);

	if
	 (
		twoScreen &&
		inBase.mouseX >= screenWidth+virtualXOffset &&
		inBase.mouseX < screenWidth+virtualXOffset &&
		inBase.mouseY >= virtualYOffset &&
		inBase.mouseY < screenHeight+virtualYOffset
	 )
		DrawPointer(scrBuffer2,inBase.mouseX-(screenWidth+virtualXOffset),inBase.mouseY-virtualYOffset,screenWidth, screenHeight);
}

//==============================================================================

void
ScrollToMouse(_input *in)
{
	if(in->mouseX < virtualXOffset)
	 {
		virtualXOffset = in->mouseX;
		refreshAll = boolean::TRUE;
	 }
	if(in->mouseX >= virtualXOffset + screenWidth)
	 {
		virtualXOffset = in->mouseX-(screenWidth-1);
		refreshAll = boolean::TRUE;
	 }
	if(in->mouseY < virtualYOffset)
	 {
		virtualYOffset = in->mouseY;
		refreshAll = boolean::TRUE;
	 }
	if(in->mouseY >= virtualYOffset + screenHeight)
	 {
		virtualYOffset = in->mouseY-(screenHeight-1);
		refreshAll = boolean::TRUE;
	 }

}

//==============================================================================

void
UpdateScreen(void)
{
	ErasePointer();

	DisplayLayers(scrBuffer,-virtualXOffset,-virtualYOffset,backgroundChar, backgroundAttr,screenWidth,screenHeight);
	if(twoScreen)
		DisplayLayers(scrBuffer2,-(screenWidth+virtualXOffset),-virtualYOffset,backgroundChar2,backgroundAttr2,screenWidth2,screenHeight2);

	DisplayPointer();

	CopyScreen(screen,scrBuffer,screenSize);
	if(twoScreen)
		CopyScreen(screen2,scrBuffer2,screenSize2);
	refreshAll = boolean::FALSE;
}

//=============================================================================

unsigned char far *
GetScrAddr(int x,int y)
{
	if(x >= screenWidth)
		return( (unsigned char far*)(scrBuffer2 + (screenWidth*y*2) + ((x-screenWidth)*2)));
	return( (unsigned char far*)(scrBuffer + (screenWidth*y*2) + (x*2)));
}

//=============================================================================

void
XorDisplayRect(int startX, int startY, int endX, int endY)
{
	int i,j;
	unsigned char far *sPtr;

	ErasePointer();
	if(endX < startX)
		endX = startX;

	if(endY < startY)
		endY = startY;

	for(i=startY; i<= endY;++i)
	 {
		for(j = 0; j <= endX-startX; j++)
		 {
			sPtr = GetScrAddr(startX+j,i)+1;
			*sPtr = *sPtr ^ 0x7f;
		 }
	 }
	DisplayPointer();
}

//=============================================================================

void
ReadDisplayRect(char *buffer,int startX, int startY, int endX, int endY)
{
	int i,j;
	char far *sPtr;

	if(endX < startX)
		endX = startX;

	if(endY < startY)
		endY = startY;

	for(i=startY; i<= endY;++i)
	 {
		for(j = 0; j <= endX-startX; j++)
		 {
			sPtr = (char far*)GetScrAddr(startX+j,i);
			*buffer++ = *sPtr;
		 }
	 }
	*buffer = 0;
}

//=============================================================================

void
SetupDisplay(void)
	{

						// kts check for no memory
#ifdef __MSDOS__
	if ( scrBuffer ) free( scrBuffer );
	scrBuffer = (char far *)farmalloc(screenSize);
#endif

#ifdef __OS2__
	VioGetBuf( (PULONG16)&scrBuffer, &screenSize, 0 );
#endif


	ClearScrBuff(scrBuffer,backgroundChar,backgroundAttr,screenSize);

	if ( twoScreen )
		{
		if(scrBuffer2)
			free(scrBuffer2);
		scrBuffer2 = (char far *)farmalloc(screenSize2);
		ClearScrBuff(scrBuffer2,backgroundChar2,backgroundAttr2,screenSize2);
		}

	DisplayPointer();                               // cause pointer to have a starting point

	virtualXOffset = 0;
	virtualYOffset = 0;

	refreshAll = boolean::TRUE;
	ReSizeMouse();
	}

//==============================================================================

void
InitDisplay()
	{
	if(twoScreen)
		{
		displayWidth = virtualXSize<(screenWidth+screenWidth2)?(screenWidth+screenWidth2):virtualXSize;
		displayHeight = virtualYSize<screenHeight?screenHeight:virtualYSize;
		}
	else
		{
		displayWidth = virtualXSize<screenWidth?screenWidth:virtualXSize;
		displayHeight = virtualYSize<screenHeight?screenHeight:virtualYSize;
		}
	}

//=============================================================================

void
ScrollDisplayUp(int distance)
{
	virtualYOffset = virtualYOffset+(distance)<displayHeight-screenHeight?virtualYOffset+(distance):displayHeight-screenHeight;
	refreshAll = boolean::TRUE;
}

void
ScrollDisplayDown(int distance)
{
	virtualYOffset = virtualYOffset-(distance)>0?virtualYOffset-(distance):0;
	refreshAll = boolean::TRUE;
}

void
ScrollDisplayLeft(int distance)
{
	virtualXOffset = virtualXOffset-(distance)>0?virtualXOffset-(distance):0;
	refreshAll = boolean::TRUE;
}

void
ScrollDisplayRight(int distance)
{
	virtualXOffset = virtualXOffset+(distance)<displayWidth-screenWidth?virtualXOffset+(distance):displayWidth-screenWidth;
	refreshAll = boolean::TRUE;
}

//=============================================================================
