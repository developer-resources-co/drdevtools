//=============================================================================
//  gadget.c: hit box handling
//=============================================================================

#include	"base.hpp"
#include	"global.hpp"

#include	"display.hpp"
#include	"general.hpp"
#include	"config.hpp"
#include	"list.hpp"
#include	"keys.hpp"
#include	"monkeys.hpp"
#include	"input.hpp"
#include	"gadget.hpp"
#include	"object.hpp"
#include	"listrect.hpp"
#include	"layer.hpp"
#include	"window.hpp"
#include	"message.hpp"
#include	"menu.hpp"
#include	"error.hpp"
#include	"gadget.hpp"

//=============================================================================

_gadget *pGadgDown;
int gadgOldInputMask;
uint insertDefault;

//=============================================================================

void
InitGadgetList(_gadget *gBase)
{
	InitListBase(&gBase->link);
}

//=============================================================================

_gadget *
NewGadget(_gadget *gList)
{
	_gadget *gPtr;
	gPtr = (_gadget *)malloc(sizeof(_gadget));
	if(gPtr)
	 {
		AddListNode((_list *)gList,(_list *)gPtr);
		gPtr->routine = 0;
		gPtr->pGNum = 0;
		gPtr->nGNum = 0;
		gPtr->gadgText = 0;
		gPtr->cursorX = 0;
		gPtr->mode = 0;
		gPtr->gType = 0;				// 0 = not defined
	 }
	return(gPtr);
}

//=============================================================================

_gadget *
MakeGadget(_window *pWindow, int xPos,int yPos,int xSize,int ySize,int gNum)
{
	_gadget  *gPtr;
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = xSize;
	gPtr->ySize = ySize;
	gPtr->xPos = xPos;
	gPtr->yPos = yPos;
	gPtr->gadgText = NULL;
	gPtr->gNum = gNum;
	gPtr->gadgAttr = windowBorderAttr;
	return(gPtr);
}

//=============================================================================
// given ptr to base, & gadget id num desired, find gadget

_gadget *
FindGadget(_gadget *gPtr,int gNum)
{
	while(gPtr != 0 && gPtr->gNum != gNum)
		gPtr = (_gadget *)gPtr->link.next;
	return(gPtr);
}

//=============================================================================

_gadget *
NewGadgetInit(_gadget *gList,int xPos,int yPos,int xSize, int ySize)
{
	_gadget *gPtr;
	if((gPtr = NewGadget(gList))!= 0)
	 {
		gPtr->xPos = xPos;
		gPtr->xPos = yPos;
		gPtr->xPos = xSize;
		gPtr->xPos = ySize;
	 }
	return(gPtr);
}

//=============================================================================

void
DeleteGadget(_gadget *gPtr)
{
	DeleteListNode((_list *)gPtr);
}

//=============================================================================

void
DeleteAllGadgets(_gadget *gPtr)
{
	while(gPtr->link.next)
		DeleteGadget((_gadget *)gPtr->link.next);
}

//=============================================================================

_gadget *
GadgetHit(_gadget *gList,int xPos,int yPos)
{
	gList = (_gadget *)GetLastNode((_list *)gList);
	while(gList)
	 {
		if(gList->xPos <= xPos && (gList->xPos+gList->xSize) > xPos &&
		   gList->yPos <= yPos && (gList->yPos+gList->ySize) > yPos)
			return(gList);
		gList = (_gadget *)gList->link.prev;
	 }
	return(gList);				// note: will always be zero here
}

//=============================================================================

void
DrawGadgets( _window* pWindow )
	{
	assert( pWindow );

	for ( _gadget* gPtr = &pWindow->gadgBase; gPtr = (_gadget*)gPtr->link.next; )
		{
		if ( gPtr->gadgText )
			{
			PrintLayAttr( pWindow, gPtr->gadgText,
				gPtr->xPos, gPtr->yPos, gPtr->gadgAttr );

			int xStr = strlen( gPtr->gadgText );

#if 0
			PrintLayAttr( pWindow, "           ",
				gPtr->xPos + xStr, gPtr->yPos, gPtr->gadgAttr );
#endif
			}
		}
	}

//=============================================================================

void
AddSysGadgets(_window *pWindow)
{
	_gadget *gPtr;
										// add drag gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 999;					// as large as possible
	gPtr->ySize = 1;
	gPtr->xPos = 0;
	gPtr->yPos = 0;
	gPtr->gadgText = NULL;					// no render text
	gPtr->routine = NULL;
	gPtr->gNum =GGAD_DRAG;

										// add close gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 1;
	gPtr->ySize = 1;
	gPtr->xPos = 1;
	gPtr->yPos = 0;
	gPtr->gadgText = "þ";
	gPtr->routine = NULL;
	gPtr->gNum = GGAD_CLOSE;
	gPtr->gadgAttr = windowBorderAttr;

	if((pWindow->xMin != pWindow->xMax) || (pWindow->yMin != pWindow->yMax))
	 {
											// add resize gadget
		gPtr = NewGadget(&pWindow->gadgBase);
		gPtr->xSize = 1;
		gPtr->ySize = 1;
		gPtr->xPos = pWindow->xSize-1;
		gPtr->yPos = pWindow->ySize-1;
		gPtr->gadgText = "¼";								// OLD: "+";
		gPtr->routine = NULL;
		gPtr->gNum = GGAD_RESIZE;
		gPtr->gadgAttr = windowBorderAttr;

											// add zoom gadget
		gPtr = NewGadget(&pWindow->gadgBase);
		gPtr->xSize = 1;
		gPtr->ySize = 1;
		gPtr->xPos = pWindow->xSize-1;
		gPtr->yPos = 0;
		gPtr->gadgText = "";
		gPtr->routine = NULL;
		gPtr->gNum = GGAD_ZOOM;
		gPtr->gadgAttr = windowBorderAttr;
	 }
	// In case adding gadgets (especially a close gadget)
	// has changed position of any of the text in top border
	PrintTitle(pWindow,windowTitleHighlightAttr);
	DrawBorder(pWindow,pWindow->border,pWindow->xSize,pWindow->ySize);
}

//=============================================================================
// proportional gadget support


#define DTLEN 50
char dragText[(DTLEN*2)+1] = "°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n°\n";

void
AddVScrollGadgetsCoord(_window* pWindow,int upNum,int downNum,int scrollNum,int scBoxNum,int xPos,int yTop, int yBot)
{
	_gadget *gPtr;
										// add up gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 1;
	gPtr->ySize = 1;
	gPtr->xPos = xPos;
	gPtr->yPos = yTop;
	gPtr->gadgText = "";
	gPtr->gNum = upNum;
	gPtr->gadgAttr = windowBorderAttr;
											// add down gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 1;
	gPtr->ySize = 1;
	gPtr->xPos = xPos;
	gPtr->yPos = yBot;
	gPtr->gadgText = "";
	gPtr->gNum = downNum;
	gPtr->gadgAttr = windowBorderAttr;

	if(yBot-yTop > 1)
		{
											// add scroll gadget
		gPtr = NewGadget(&pWindow->gadgBase);
		gPtr->xSize = 1;
		gPtr->ySize = yBot-(yTop+1);
		gPtr->xPos = xPos;
		gPtr->yPos = yTop+1;
		gPtr->gadgText = &dragText[(DTLEN-gPtr->ySize)*2];
		gPtr->gNum = scrollNum;
		gPtr->gadgAttr = windowBorderAttr;
											// add scroll box gadget
		gPtr = NewGadget(&pWindow->gadgBase);
		gPtr->xSize = 1;
		gPtr->ySize = 1;
		gPtr->xPos = xPos;
		gPtr->yPos = yTop+1;
		gPtr->gadgText = strdup( "Û" );
		gPtr->gNum = scBoxNum;
		gPtr->pGNum = scrollNum;					// remember your parent
		gPtr->gadgAttr = gadgPropAttr;
		}
	pGadgDown = NULL;
}

//=============================================================================

void
AddVScrollGadgets(_window *pWindow,int upNum,int downNum,int scrollNum,int scBoxNum)
{
	AddVScrollGadgetsCoord(pWindow,upNum,downNum,scrollNum,scBoxNum,pWindow->xSize-1,1,pWindow->ySize-2);
}

//=============================================================================
// prop = proportional position of gadget(0-65535)

void
UpdateVScrollGadg( _window *pWindow, unsigned gNum,
	uword prop, uword propShown )
	{
	unsigned long size,temp;
	_gadget *gPtr,*gParentPtr;

	gPtr = FindGadget(&pWindow->gadgBase,gNum);
	if(!gPtr)
		return;
	assert(gPtr->pGNum);
	gParentPtr = FindGadget(&pWindow->gadgBase,gPtr->pGNum);
	assert(gParentPtr);
	size = gParentPtr->ySize;
	assert(size);
	temp = 65535/size;
	assert(temp);
	gPtr->yPos = gParentPtr->yPos + prop/temp;

	// New Amiga-like proportional gadgets, thanks to Bill
	if ( propShown )
		{
		int yOldSize = gPtr->ySize;

		gPtr->ySize = ( (long)propShown * size) / 65535;
		if ( gPtr->ySize == 0 )
			gPtr->ySize = 1;

		if ( gPtr->ySize != yOldSize )
			{
			free( gPtr->gadgText );
			gPtr->gadgText = (char*)malloc( (gPtr->ySize*2+1) * sizeof(char) );
			assert( gPtr->gadgText );
			strncpy( gPtr->gadgText,
				"\
Û\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\n\
Û\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\n\
Û\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\n\
Û\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\n\
Û\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\n\
Û\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\n\
Û\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\n\
Û\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\nÛ\n\
				",
				gPtr->ySize*2 );
			*( gPtr->gadgText + gPtr->ySize*2 ) = '\0';
			}
		}
	}

//=============================================================================

unsigned int
GetPropGadgPos(_window *pWindow,unsigned int gNum,int yOffset)
{
	unsigned int size,temp;
	_gadget *gPtr,*gParentPtr;
	gPtr = FindGadget(&pWindow->gadgBase,gNum);
	gParentPtr = FindGadget(&pWindow->gadgBase,gPtr->pGNum);

	yOffset -= gParentPtr->yPos;
	if(yOffset < 0)
		return(0);
	size = gParentPtr->ySize-1;
	if(yOffset > size)
		return(65535);
	temp = 65535/size;
	return(yOffset*temp);
}

//=============================================================================

unsigned int
GetNormalizedPropGadgPos(_window *pWindow,unsigned int gNum, int yOffset,unsigned int maxVal)
{
	unsigned int temp,val;
	val = GetPropGadgPos(pWindow,gNum,yOffset);
	if(maxVal > 1)
		temp = 65535/(maxVal-1);
	else
		return(0);
	return(val/temp);
}

//=============================================================================

void
PropGadgDown(_object* pObject, _input* in,_gadget* gPtr)
{
//	gadgOldInputMask = in->inputMask;
	gadgOldInputMask = pObject->inputFlags;
	pObject->inputFlags = INPF_MOUSEBUTTONS|INPF_MOUSEMOVE;
	gPtr->gadgAttr = gadgPropHighlightAttr;
	pGadgDown = gPtr;
}

//=============================================================================
// returns true if gadget was lifted, false if no gadget was down

FLAG
PropGadgUp(_object *pObject,_input *in)
{
	if(pGadgDown)
	 {
		pObject->inputFlags = gadgOldInputMask;
		pGadgDown->gadgAttr = gadgPropAttr;
		pGadgDown = NULL;
		return(boolean::TRUE);
	 }
	return(boolean::FALSE);
}

//=============================================================================

void
ActivateStrGadget(_window* pWindow,_gadget *gPtr)
{
//	PadString(gPtr->gadgText,gPtr->xSize-1);
	PadString(gPtr->gadgText,gPtr->xSize);

	pWindow->cursorOn = boolean::TRUE;
	pWindow->cursorX = gPtr->cursorX+gPtr->xPos;
	pWindow->cursorY = gPtr->yPos;
	if(gPtr->mode & STRGADF_MODE_INSERT)
		pWindow->cStartLine = 4;
	else
		pWindow->cStartLine = 6;
	UpdateCursor();
}

//=============================================================================

void
DeactivateStrGadget(_window* pWindow)
{
	pWindow->cursorOn = boolean::FALSE;
	UpdateCursor();
}

//=============================================================================

FLAG
StrGadgInput(_window* pWindow,_gadget *gPtr,_input *in)
{
	FLAG inputUsed = boolean::TRUE;
	char *tPtr,*tPtr2;
	int i;

	if(in->inputType == INP_KEY)
		switch(in->fullKey)
		 {
			case KEY_INSERT:
				gPtr->mode ^= STRGADF_MODE_INSERT;
				if(gPtr->mode & STRGADF_MODE_INSERT)
					pWindow->cStartLine = 4;
				else
					pWindow->cStartLine = 6;
				UpdateCursor();
				break;
			case KEY_DELETE:
				tPtr = &gPtr->gadgText[gPtr->cursorX];
				tPtr2 = tPtr+1;
				while(*tPtr2)
					*tPtr++ = *tPtr2++;
				*tPtr = ' ';
				break;
			case CMD_CUT:
				strncpy(&copyBuffer[0],gPtr->gadgText,CLIPBOARDSIZE);
				gPtr->gadgText[0] = 0;
				gPtr->cursorX = 0;
				PadString(gPtr->gadgText,gPtr->xSize-1);
				break;
			case CMD_COPY:
				strncpy(&copyBuffer[0],gPtr->gadgText,CLIPBOARDSIZE);
				break;
			case CMD_PASTE:
				UnPadString(copyBuffer);
				strncpy((gPtr->gadgText)+gPtr->cursorX,&copyBuffer[0],(gPtr->xSize-1)-gPtr->cursorX);
				gPtr->cursorX += strlen(copyBuffer);
				if(gPtr->cursorX >= gPtr->xSize)
					gPtr->cursorX = gPtr->xSize-1;
				PadString(gPtr->gadgText,gPtr->xSize-1);
				break;
			case KEY_RIGHT:
				if(gPtr->cursorX < gPtr->xSize)
					gPtr->cursorX++;
				break;
			case KEY_LEFT:
				if(gPtr->cursorX > 0)
					gPtr->cursorX--;
				break;
			case KEY_CTRLX:							// erase string in window
				gPtr->gadgText[0] = 0;
				gPtr->cursorX = 0;
				PadString(gPtr->gadgText,gPtr->xSize-1);
				break;

			case KEY_CTRLLEFT:
				if(gPtr->gadgText[gPtr->cursorX] != ' ' && gPtr->cursorX > 0)
					gPtr->cursorX--;
				while(gPtr->gadgText[gPtr->cursorX] == ' ' && gPtr->cursorX > 0)
					gPtr->cursorX--;
				while(gPtr->gadgText[gPtr->cursorX] != ' ' && gPtr->cursorX > 0)
					gPtr->cursorX--;
				while(gPtr->gadgText[gPtr->cursorX] == ' ' && gPtr->cursorX < gPtr->xSize)
					gPtr->cursorX++;
				break;
			case KEY_CTRLRIGHT:
				while(gPtr->gadgText[gPtr->cursorX] != ' ' && gPtr->cursorX < gPtr->xSize)
					gPtr->cursorX++;
				while(gPtr->gadgText[gPtr->cursorX] == ' ' && gPtr->cursorX < gPtr->xSize)
					gPtr->cursorX++;
				break;
			case KEY_HOME:
				gPtr->cursorX = 0;
				break;
			case KEY_END:
				i = gPtr->xSize-1;
				while(gPtr->gadgText[i] == ' ' && i > 0)
					i--;
				gPtr->cursorX = i;
				break;
			case KEY_CTRLEND:
				gPtr->gadgText[gPtr->cursorX] = 0;
				PadString(gPtr->gadgText,gPtr->xSize-1);
				break;
			case KEY_CTRLHOME:
				tPtr2= &gPtr->gadgText[gPtr->cursorX];
				tPtr = &gPtr->gadgText[0];
				while(*tPtr2)
					*tPtr++ = *tPtr2++;
				gPtr->cursorX = 0;
				PadString(gPtr->gadgText,gPtr->xSize-1);
				break;
			case KEY_BACKSPACE:
				if(gPtr->cursorX > 0)
			 	 {
					gPtr->cursorX--;
					tPtr = &gPtr->gadgText[gPtr->cursorX];
					tPtr2 = tPtr+1;
					while(*tPtr2)
						*tPtr++ = *tPtr2++;
					*tPtr = ' ';
			 	 }
				break;
			default:
				if(in->fullKey >= ' ' && in->fullKey <= 0x7e)
			 	 {
					if(gPtr->cursorX < gPtr->xSize)
				 	 {
						if(gPtr->mode & STRGADF_MODE_INSERT)
					 	 {
							tPtr = &gPtr->gadgText[gPtr->xSize-2];
							tPtr2 = tPtr+1;
							//while(*tPtr2 != gPtr->gadgText[gPtr->cursorX])
							while(tPtr2 != &gPtr->gadgText[gPtr->cursorX])
								*tPtr2-- = *tPtr--;
					 	 }

						gPtr->gadgText[gPtr->cursorX] = (unsigned char)in->fullKey;
						gPtr->cursorX++;
				 	 }
			 	 }
				else
					inputUsed = boolean::FALSE;
				break;
	 	 }
	pWindow->cursorX = gPtr->cursorX+gPtr->xPos;
	pWindow->cursorY = gPtr->yPos;
	return(inputUsed);
}

//=============================================================================

#define 	STR_XSIZE 59
#define		STR_YSIZE 7

#define	STRGAD_STRX	3
#define	STRGAD_STRY	2

#define	STRGAD_OKX	51
#define	STRGAD_OKY	4

#define	STRGAD_STR	GAD_USER+1
#define	STRGAD_OK	STRGAD_STR+1

//=============================================================================

char strString[53];

//=============================================================================

void
AddStrGadgets(_window* pWindow)
{
	_gadget  *gPtr;
							// input gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 53;
	gPtr->ySize = 1;
	gPtr->xPos = STRGAD_STRX;
	gPtr->yPos = STRGAD_STRY;
	gPtr->gadgText = strString;
	gPtr->gNum = STRGAD_STR;
	gPtr->gadgAttr = windowBorderAttr;
	gPtr->gType = GADTYPE_STRING;
	gPtr->mode = insertDefault?STRGADF_MODE_INSERT:0;
	PrintLay(pWindow,"String",STRGAD_STRX,STRGAD_STRY-1);

							// ok gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 4;
	gPtr->ySize = 1;
	gPtr->xPos = STRGAD_OKX;
	gPtr->yPos = STRGAD_OKY;
	gPtr->gadgText = "[Ok]";
	gPtr->gNum = STRGAD_OK;
	gPtr->gadgAttr = windowBorderAttr;
}

//=============================================================================

FLAG
StrWinInput(_input *in,_object *oPtr)
{
	FLAG inputUsed = boolean::TRUE;
	FLAG iUsed = boolean::FALSE;
	FLAG leave = boolean::FALSE;
	void (*strRoutine)(char *string,void *data);

	_gadget  *gPtr;
    _window* pWindow;
    _listRectDesc *lrPtr;

    pWindow = (_window*)oPtr->layer;

	switch(in->inputType)
	 {
		case INP_KEY:
			if(pWindow->activeGadget)
				iUsed = StrGadgInput(pWindow,pWindow->activeGadget,in);
			if(iUsed == boolean::FALSE)
				switch(in->fullKey)
	 		 	 {
					case CMD_LIST_ABORT:
						leave = boolean::TRUE;
						break;
					case KEY_RETURN:
						switch(pWindow->activeGadget->gNum)
						 {
							case STRGAD_STR:                	// kts: this is bug, fix it!
								strRoutine = (void (*)(char *,void *))oPtr->dataPtr;
								(*strRoutine)(strString,oPtr->dataPtr2);
								leave = boolean::TRUE;
								break;
						 }
						break;
					default:
						inputUsed = boolean::FALSE;
						break;
	 		 	 }
			break;
		case INP_MOUSE_LEFTBUTTON_DOWN:
			if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
			 {
				switch(gPtr->gNum)
				 {
					case GGAD_CLOSE:
						leave = boolean::TRUE;
						break;
					case STRGAD_STR:
						pWindow->activeGadget = gPtr;
						ActivateStrGadget(pWindow,gPtr);
						break;
					case STRGAD_OK:
						strRoutine = (void (*)(char *,void *))oPtr->dataPtr;
						(*strRoutine)(strString,oPtr->dataPtr2);
						leave = boolean::TRUE;
						break;
					default:
							inputUsed = boolean::FALSE;
						break;
				 }
			 }
			break;
		default:
			inputUsed = boolean::FALSE;
			break;
	 }

	if(inputUsed)
	 {
		DrawGadgets(pWindow);
	 }

	if(leave)
	 {
//		CloseWindow((_window *)oPtr->layer);
		delete (_window *)oPtr->layer;
//		DeleteObject(oPtr);
		delete oPtr;
	 }
	return(inputUsed);
}

//=============================================================================
// this works even if the layer pointer is NULL

int
AutoCenterXOnDisplay(_layer *lPtr,int size)
{
	if(lPtr)
		return(AutoPosition(lPtr->xPos,lPtr->xSize,size,displayWidth));
	else
		return(AutoPosition(0,displayWidth,STR_XSIZE,displayWidth));
}

//=============================================================================

int
AutoCenterYOnDisplay(_layer *lPtr,int size)
{
	if(lPtr)
		return(AutoPosition(lPtr->yPos,lPtr->ySize,size,displayHeight));
	else
		return(AutoPosition(0,displayHeight,STR_YSIZE,displayHeight));
}

//=============================================================================
// dataPtr->routine to execute when string entered
// dataPtr2-> ?? whatever user wants, will get passed to final routine

void
GetString(void (*strRoutine)(char *string,void *dataPtr),void *dataPtr,char *title,_layer *backLPtr)
{
	_object *oPtr;
	_window *pWindow;
	int xPos,yPos;

	xPos = AutoCenterXOnDisplay(backLPtr,STR_XSIZE);
	yPos = AutoCenterYOnDisplay(backLPtr,STR_YSIZE);

    pWindow = new _window(xPos,yPos,STR_XSIZE,STR_YSIZE,title,(unsigned char)strAttr,(char)strChar);
	if(pWindow)
	 {
		pWindow->cursorOn = boolean::TRUE;

		oPtr = AddObject();
		oPtr->inputFlags = INPF_KEY|INPF_MOUSEBUTTONS|INPF_CASESENSE;
		oPtr->inputRoutine = StrWinInput;
		oPtr->layer =pWindow;
		oPtr->dataPtr = (char*)strRoutine;			// YUK!
		oPtr->dataPtr2 = (char *)dataPtr;
		pWindow->data = (void *)oPtr;

		AddSysGadgets(pWindow);
		AddStrGadgets(pWindow);
		pWindow->activeGadget = FindGadget(&pWindow->gadgBase,STRGAD_STR);
		ActivateStrGadget(pWindow,pWindow->activeGadget);
		ActivateFrontWindow();
		DrawGadgets(pWindow);
	 }
	else
		PrintError(ERROR_NOMEM);
}

//=============================================================================
