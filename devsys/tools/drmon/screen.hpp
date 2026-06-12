/*===========================================================================*/
/*==   screen.h: screen headers											    =*/
/*===========================================================================*/

#define charSize 2

#include <conio.h>
extern char far *screen,far *screen2;

extern unsigned short screenWidth,screenHeight,screenSize;
extern unsigned short screenWidth2,screenHeight2,screenSize2;

extern unsigned char fillChar;
extern int cursorX,cursorY;
extern unsigned int displayMode;
extern FLAG curOn;
extern int cursorInsert;
extern FLAG twoScreen;
extern FLAG primaryDispCGA;
extern FLAG writeProtect;


extern unsigned int displayMode;

//=============================================================================

void
PositionCursor(int xPos,int yPos);

//=============================================================================

void
CursorOff(void);

//=============================================================================

void
CursorOn(void);

//=============================================================================

void
InitScreen(void);

void
SetupScreen(void);

//=============================================================================

void
ClearScrBuff(char far *scrBuff,unsigned char fillChar,unsigned char charAttr,int screenSize);

//=============================================================================

void
CloseScreen(void);

//=============================================================================

