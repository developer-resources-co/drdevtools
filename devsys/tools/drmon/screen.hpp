/*===========================================================================*/
/*==   screen.h: screen headers											    =*/
/*===========================================================================*/

#define charSize 2

#ifdef __OS2__
extern char _far16 *screen;
extern char _far16 *screen2;
#elif __WATCOMC__ || __SC__
extern char far* screen, far* screen2;
#else
#include <conio.h>
extern text_info ti;
extern char far *screen,far *screen2;
#endif

extern unsigned short screenWidth,screenHeight,screenSize;
extern unsigned short screenWidth2,screenHeight2,screenSize2;

extern unsigned char fillChar;
extern int cursorX,cursorY;
extern unsigned int displayMode;
extern FLAG curOn;
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

void
SetCursorStartStop(int startLine,int stopLine);


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

