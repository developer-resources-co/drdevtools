/*
 * gadget.c -- General purpose gadget/icon handling code.
 * (c) 1991 Developer Resources
 * Written By Scott Statton
 */


// pclib
#include <pclib/general.h>
#include <pclib/iff.h>
#include <pclib/grphport.h>

// local
#include "menu.hpp"
#include "sprite.hpp"

#include "utypes.hpp"
#include	"input.hpp"
#include "global.hpp"
#include "sq.hpp"
#include "spmain.hpp"
#include	"keys.hpp"
#include	"general.hpp"

#include "gadget.hpp"

/***************************************************************************/

struct _gadget gadgetBase;

/***************************************************************************/

void
NullRoutine(struct _gadget *g, struct _input *i)
{
	return;
}

/***************************************************************************/

void
InitGadgets(void)
{
	gadgetBase.next = gadgetBase.prev = NULL;
	gadgetBase.routine = NullRoutine;
	return;
}

/***************************************************************************/



struct _gadget *
CreateGadget(struct _gadget *base, word x, word y, word sizeX, word sizeY, byte type, word identity,
				void far *dp1, void far *dp2, void far *dp3, void (*routine)(struct _gadget *, struct _input *))
{
	struct _gadget *gPtr;

	gPtr = (struct _gadget *)malloc(sizeof(struct _gadget));
	if(!gPtr)
		Error("Could not create gadget ... not enough memory\n");
	gPtr->next = NULL;
	gPtr->prev = FindTail(base);
	gPtr->prev->next = gPtr;
	gPtr->offsetX = x; gPtr->offsetY = y;
	gPtr->sizeX = sizeX; gPtr->sizeY = sizeY;
	gPtr->type = type; gPtr->identity = identity;
	gPtr->dataPtr1 = dp1; gPtr->dataPtr2 = dp2;
	gPtr->dataPtr3 = dp3;
	gPtr->routine = routine;
	return gPtr;
}

/***************************************************************************/


struct _gadget *
FindTail(struct _gadget *gPtr)
{
	while(gPtr && gPtr->next) gPtr = gPtr->next;
	return gPtr;
}

/***************************************************************************/

struct _gadget *
FindGadget(struct _gadget *base, word identity)
{
	struct _gadget *gPtr = base;

	while (gPtr && (gPtr->identity != identity))
		gPtr = gPtr->next;
	return gPtr;
}

/***************************************************************************/

void
KillMultipleGadgets(struct _gadget *gPtr)
{
	gPtr = FindTail(gPtr);
	while(gPtr->prev) {
		gPtr = gPtr->prev;
		KillGadget(gPtr->next);
		}
	return;
}


void
KillGadget(struct _gadget *gPtr)
{
	if(gPtr->prev)
		gPtr->prev->next = gPtr->next;
	if (gPtr->next)
		gPtr->next->prev = gPtr->prev;
	free(gPtr);
	return;
}

/***************************************************************************/

void
CreateMultipleGadgets(struct _multigadget mgPtr[], int n)
{
	while(n--)
	{
		CreateGadget(
			&gadgetBase,
			mgPtr[n].offsetX/2,
			mgPtr[n].offsetY,
			((mgPtr[n].sizeX+1)/2)-1,
			mgPtr[n].sizeY,
			mgPtr[n].type,
			mgPtr[n].identity,
			NULL, NULL, NULL, 	/* set DP1...3  to null for now */
			mgPtr[n].routine);
	}
	return;
}

/***************************************************************************/


boolean
DoHitGadget(struct _gadget *base, struct _input *in)
{
	struct _gadget *gPtr;
	word mouseX = in->mouseX;
	word mouseY = in->mouseY;

	gPtr = FindTail(base);
	while (gPtr)
	{
		if (( mouseX >= gPtr->offsetX && mouseX < (gPtr->offsetX+gPtr->sizeX))
		 && ( mouseY >= gPtr->offsetY && mouseY < (gPtr->offsetY+gPtr->sizeY)))
		{
		 	(*gPtr->routine)(gPtr, in);
			return boolean::TRUE;
		}
		gPtr = gPtr->prev;
	}
	return boolean::FALSE;
}

/***************************************************************************/

boolean
StrGadgInput(struct _string *stPtr,struct _input *in)
{
	char *tPtr,*tPtr2;
	int i;
	boolean inputUsed = boolean::TRUE;

	if (in->inputType != INP_KEY )
		return boolean::FALSE;

	switch(in->fullKey)
	{
		case KEY_DELETE:
			tPtr = &stPtr->text[stPtr->cursorX];
			tPtr2 = tPtr+1;
			while(*tPtr2)
				*tPtr++ = *tPtr2++;
			*tPtr = ' ';
			break;
		case KEY_RIGHT:
			if(stPtr->cursorX < stPtr->xSize)
				stPtr->cursorX++;
			break;
		case KEY_LEFT:
			if(stPtr->cursorX > 0)
				stPtr->cursorX--;
			break;
		case KEY_CTRLX:							// erase string in window
			stPtr->text[0] = 0;
			stPtr->cursorX = 0;
			PadString(stPtr->text,stPtr->xSize-1);
			break;

		case KEY_CTRLLEFT:
			if(stPtr->text[stPtr->cursorX] != ' ' && stPtr->cursorX > 0)
				stPtr->cursorX--;
			while(stPtr->text[stPtr->cursorX] == ' ' && stPtr->cursorX > 0)
				stPtr->cursorX--;
			while(stPtr->text[stPtr->cursorX] != ' ' && stPtr->cursorX > 0)
				stPtr->cursorX--;
			while(stPtr->text[stPtr->cursorX] == ' ' && stPtr->cursorX < stPtr->xSize)
				stPtr->cursorX++;
			break;
		case KEY_CTRLRIGHT:
			while(stPtr->text[stPtr->cursorX] != ' ' && stPtr->cursorX < stPtr->xSize)
				stPtr->cursorX++;
			while(stPtr->text[stPtr->cursorX] == ' ' && stPtr->cursorX < stPtr->xSize)
				stPtr->cursorX++;
			break;
		case KEY_HOME:
			stPtr->cursorX = 0;
			break;
		case KEY_END:
			i = stPtr->xSize-1;
			while(stPtr->text[i] == ' ' && i > 0)
				i--;
			stPtr->cursorX = i;
			break;
		case KEY_CTRLEND:
			stPtr->text[stPtr->cursorX] = 0;
			PadString(stPtr->text,stPtr->xSize-1);
			break;
		case KEY_CTRLHOME:
			tPtr2= &stPtr->text[stPtr->cursorX];
			tPtr = &stPtr->text[0];
			while(*tPtr2)
				*tPtr++ = *tPtr2++;
			stPtr->cursorX = 0;
			PadString(stPtr->text,stPtr->xSize-1);
			break;
		case KEY_BACKSPACE:
			if(stPtr->cursorX > 0)
			 {
				stPtr->cursorX--;
				tPtr = &stPtr->text[stPtr->cursorX];
				tPtr2 = tPtr+1;
				while(*tPtr2)
					*tPtr++ = *tPtr2++;
				*tPtr = ' ';
			 }
			break;
		default:
			inputUsed = boolean::FALSE;
			if(in->fullKey >= ' ' && in->fullKey <= 0x7e)
			 {
				if(stPtr->cursorX < stPtr->xSize)
				 {
					stPtr->text[stPtr->cursorX] = (unsigned char)in->fullKey;
					stPtr->cursorX++;
					inputUsed = boolean::TRUE;
				 }
			 }
			break;
	 }
	 return inputUsed;
}
