//=============================================================================
//   input.c: handle i/o
//=============================================================================
// NOTE: all input handlers should properly ignore a key input of 0
//=============================================================================

// pclib
#include <pclib\general.h>
#include <pclib\iff.h>
#include <pclib\grphport.h>

// local
#include "menu.hpp"
#include "sprite.hpp"

#include	<bios.h>
#include <conio.h>

#include	"utypes.hpp"
#include	"global.hpp"

#include	"keys.hpp"
#include	"input.hpp"
#include	"mouse.hpp"
#include	"gadget.hpp"
#include	"spgadget.hpp"

//=============================================================================

struct _input inBase;
unsigned char far *pointerScreen;
boolean mouseBEnable;
char copyBuffer[CLIPBOARDSIZE];					// buffer for cut/copy/paste
boolean quitboolean;
boolean haveMouse;

extern struct _gadget *previousGadget;

//=============================================================================


unsigned char
GetShiftStatus(void)
{
	unsigned char status;
	status = *(unsigned char far *)0x417;
	status = ((status & 0x02)>>1) | (status & 1);
	return(status);
}

//=============================================================================

unsigned int
GetDirectKey(void)
{
    unsigned int fullKey;
    unsigned char key;
	key = getch();
	if(key == 0)
    	fullKey = getch() | 0x100 | (uword)(GetShiftStatus()) << 9;
	else
	 {
    	fullKey = key;
		if(fullKey >= 'a' && fullKey <= 'z')			// convert all keys to upper case
			fullKey -= ('a'-'A');						// to prevent confusion
	 }
	return(fullKey);
}

//=============================================================================

void
TurnOffMouseMove(void)
{
	inBase.inputMask &= (~(INPF_MOUSEMOVE));
	return;
}

void
TurnOnMouseMove(void)
{
	inBase.inputMask |= (INPF_MOUSEMOVE);
	return;
}

boolean
InputPending(struct _input *in)
{
	int shift;
	shift = GetShiftStatus();

	if(haveMouse)
	 {
		in->oldMouseX = in->mouseX;
		in->oldMouseY = in->mouseY;
		in->oldMouseButtons = in->mouseButtons;

		in->mouseButtons = GetMouse(&in->mouseX,&in->mouseY);
		in->mouseX = in->mouseX / 2;

		if(in->mouseButtons != in->oldMouseButtons)
		 {
			if(in->mouseButtons & MOUSEF_BRIGHT && !(in->oldMouseButtons & MOUSEF_BRIGHT))
			 {
				if(shift)
					in->inputType = INP_MOUSE_RIGHTBUTTON_SHIFTDOWN;
				else
					in->inputType = INP_MOUSE_RIGHTBUTTON_DOWN;
				return(boolean::TRUE);
			 }
			if(in->mouseButtons & MOUSEF_BLEFT && !(in->oldMouseButtons & MOUSEF_BLEFT))
			 {
				if(shift)
			   		in->inputType = INP_MOUSE_LEFTBUTTON_SHIFTDOWN;
				else
			   		in->inputType = INP_MOUSE_LEFTBUTTON_DOWN;
				return(boolean::TRUE);
			 }

			if(in->oldMouseButtons & MOUSEF_BRIGHT && !(in->mouseButtons & MOUSEF_BRIGHT))
			 {
				in->inputType = INP_MOUSE_RIGHTBUTTON_UP;
				return(boolean::TRUE);
			 }
			if(in->oldMouseButtons & MOUSEF_BLEFT && !(in->mouseButtons & MOUSEF_BLEFT))
			 {
				in->inputType = INP_MOUSE_LEFTBUTTON_UP;
				return(boolean::TRUE);
			 }
		 }
		if((in->mouseX != in->oldMouseX || in->mouseY != in->oldMouseY) && (in->inputMask & INPF_MOUSEMOVE))
	 	 {
			in->inputType = INP_MOUSEMOVE;
			return(boolean::TRUE);
	 	 }
	 }
	if(bioskey(1) && (in->inputMask & INPF_KEY))
	 {
		in->inputType = INP_KEY;
		in->fullKey = GetDirectKey();
		return(boolean::TRUE);
	 }


	in->inputType = INP_NONE;
	return(boolean::FALSE);
}

//=============================================================================

void
InitMouse(void)
{
	if(CheckMouse())
	 {
//    	SetGraphicPointer (p1,0,0);
    	ShowMouse ();
    	SetMouseBounds (0,639,0,199);
    	SetMouse (320,100);
		haveMouse = boolean::TRUE;
		HideMouse();
	 }
	else
		haveMouse = boolean::FALSE;
}

//=============================================================================

void
InitInput(void)
{
	InitMouse();
	inBase.mouseButtons = GetMouse(&inBase.mouseX,&inBase.mouseY);
	inBase.inputMask = INPF_KEY|INPF_MOUSEMOVE;
	copyBuffer[0] = 0;					// start paste buffer with empty string
	quitboolean = boolean::FALSE; /* do not quit */
	InitGadgets();		/* init gadget driver in general */
	SpriteInitGadgets();	/* load sprite gadgets */
	previousGadget = NULL;
}

//=============================================================================
