//=============================================================================
//   input.cpp: handle I/O
//=============================================================================
// NOTE: all input handlers should properly ignore a key input of 0
//=============================================================================

#define INCL_BASE

#include <conio.h>
#if !defined( __OS2__ )
#include <bios.h>
#include <dos.h>
#endif

#ifdef DOSX286
#include <phapi.h>
#endif

#include "base.hpp"
#include "global.hpp"

#include "config.hpp"
#include "screen.hpp"
#include "list.hpp"
#include "keys.hpp"
#include "monkeys.hpp"
#include "layer.hpp"
#include "window.hpp"
#include "message.hpp"
#include "input.hpp"
#include "gadget.hpp"
#include "object.hpp"
#include "listrect.hpp"
#include "mouse.hpp"
#include "menu.hpp"
#include "drmon.hpp"
#include "display.hpp"
#include "error.hpp"
#include "macro.hpp"
#include "debug.hpp"

#ifdef __OS2__
#define INCL_BASE
#include <bse.h>
#endif

//=============================================================================

_input inBase;
char far *pointerScreen;
int pointerScreenWidth;
FLAG mouseBEnable;
char copyBuffer[ CLIPBOARDSIZE ];                                 // buffer for cut/copy/paste

//=============================================================================

int pointerX,pointerY,pointerChar,pointerAttr;
FLAG pointerDrawn = boolean::FALSE;

//============================================================================

_input::_input()
{
	inputType = INP_NONE;
	mouseX = mouseY = mouseButtons = 0;
	fineMouseX = fineMouseY = 0;
//	oldMouseX = oldMouseY = oldMouseButtons = 0;
	inputMask = INPF_KEY|INPF_MOUSEMOVE;

	cmdText = NULL;
	errPtr = NULL;
}

//============================================================================

void
ErasePointer()
{
	if(pointerDrawn)
	 {
	pointerScreen += (pointerScreenWidth*charSize*pointerY)+(charSize*pointerX);
		*pointerScreen++ = pointerChar;
		*pointerScreen = pointerAttr;
		pointerDrawn = boolean::FALSE;
	 }
}

//=============================================================================

void
DrawPointer( char far *screen,int mouseX, int mouseY,int screenWidth, int screenHeight)
        {
        if ( pointerDrawn = haveMouse )
                {
        	pointerX = mouseX;
        	pointerY = mouseY;
        	pointerScreen = screen;
        	pointerScreenWidth = screenWidth;
                screen += (screenWidth*charSize*mouseY)+(charSize*mouseX);
        	pointerChar = *screen;
        	*screen++ = mouseChar;
        	pointerAttr = *screen;
        	*screen = mouseAttr;
                }
        }

//=============================================================================

void
UpdateCursor()
{
	_layer *lPtr;
	lPtr =  (layBase.Tail());

	if ( lPtr->cursorOn )
	 {
		CursorOn();
		PositionCursor(lPtr->cursorX+lPtr->xPos,lPtr->cursorY+lPtr->yPos);
		SetCursorStartStop(lPtr->cStartLine,lPtr->cStopLine);
	 }
	else
		CursorOff();
}

//=============================================================================
// 0 = no shift keys pressed, 1 = at least one shift key pressed

static unsigned char far* keyboardStatus;
#ifdef __OS2__
static KBDINFO ki;
#endif

unsigned char
GetShiftStatus(void)
{
	unsigned char status;

	status = *keyboardStatus;
	status = ((status & 0x02)>>1) | (status & 1);
	return( status );
}

//==============================================================================

unsigned char
GetScrollLockStatus(void)
{
	unsigned char status;

	status = *keyboardStatus;
	status = (status & 0x010)>>4;
	return( status );
}

//=============================================================================

#ifdef __MSDOS__
static int dosgetch( void )
	{
	int rc;
	_asm {
		mov ax,0x800;
		int 21h;
		mov rc,ax;
		}
	return rc & 0xFF;
	}
#else
#define dosgetch()	getch()
#endif

//============================================================================

unsigned int
GetDirectKey(void)
{
	unsigned int fullKey;
	unsigned char key;

	if ( (key=dosgetch()) == 0 )
		fullKey = dosgetch() | 0x100 | ((UWORD)(GetShiftStatus()) << 9) | ((UWORD)(GetScrollLockStatus()) << 10);
	else
	 {
		fullKey = key;
		if(!(frontObj->inputFlags & INPF_CASESENSE))
			if(fullKey >= 'a' && fullKey <= 'z')                    // convert all keys to upper case
				fullKey -= ('a'-'A');                                           // to prevent confusion
	 }
	return(fullKey);
}

//=============================================================================

FLAG
InputPending(_input *in)
{
	int shift,scroll;
	short oldMouseX,oldMouseY,oldMouseButtons;

#ifdef __OS2__
	KbdGetStatus( &ki, 0 );
#endif

	if(macroKeyPtr)
	 {
		in->inputType = INP_KEY;
		return(boolean::TRUE);
	 }
	shift = GetShiftStatus();

	if(haveMouse)
	 {
		oldMouseX = in->fineMouseX/8;
		oldMouseY = in->fineMouseY/8;
		oldMouseButtons = in->mouseButtons;

		in->mouseButtons = GetMouse(&in->fineMouseX,&in->fineMouseY);
		in->mouseX = in->fineMouseX/8;
		in->mouseY = in->fineMouseY/8;

		if(in->mouseButtons != oldMouseButtons)
		 {
			if(in->mouseButtons & MOUSEF_BRIGHT && !(oldMouseButtons & MOUSEF_BRIGHT))
			 {
				if(shift)
					in->inputType = INP_MOUSE_RIGHTBUTTON_SHIFTDOWN;
				else
					in->inputType = INP_MOUSE_RIGHTBUTTON_DOWN;
				return(boolean::TRUE);
			 }
			if(in->mouseButtons & MOUSEF_BLEFT && !(oldMouseButtons & MOUSEF_BLEFT))
			 {
				if(shift)
					in->inputType = INP_MOUSE_LEFTBUTTON_SHIFTDOWN;
				else
					in->inputType = INP_MOUSE_LEFTBUTTON_DOWN;
				return(boolean::TRUE);
			 }

			if(oldMouseButtons & MOUSEF_BRIGHT && !(in->mouseButtons & MOUSEF_BRIGHT))
			 {
				in->inputType = INP_MOUSE_RIGHTBUTTON_UP;
				return(boolean::TRUE);
			 }
			if(oldMouseButtons & MOUSEF_BLEFT && !(in->mouseButtons & MOUSEF_BLEFT))
			 {
				in->inputType = INP_MOUSE_LEFTBUTTON_UP;
				return(boolean::TRUE);
			 }
		 }
		if((in->mouseX != oldMouseX || in->mouseY != oldMouseY) && (in->inputMask & INPF_MOUSEMOVE))
		 {
			in->inputType = INP_MOUSEMOVE;
			return(boolean::TRUE);
		 }
	 }
#ifdef __OS2__
	KBDKEYINFO ki;
	KbdPeek( &ki, 0 );
#endif

	if (
#if defined( __OS2__ )
	( ki.fbStatus & 64 )
#else
	_bios_keybrd( _KEYBRD_READY )
//	bioskey( 1 )
#endif
	&& ( in->inputMask & INPF_KEY ) )
		{
		in->inputType = INP_KEY;
		in->fullKey = GetFullKey();
		return(boolean::TRUE);
		}

	in->inputType = INP_NONE;
	return(boolean::FALSE);
}

//=============================================================================

void
ReSizeMouse(void)
{
	SetMouseBounds(0,(displayWidth*8)-8,0,(displayHeight*8)-8);
}

//=============================================================================

void
InitMouse( void )
	{
	if ( haveMouse = CheckMouse() )
		ReSizeMouse();
#if 0
	if(CheckMouse())
	 {
		haveMouse = boolean::TRUE;
		ReSizeMouse();
	 }
	else
		haveMouse = boolean::FALSE;
#endif
	}

//=============================================================================

#ifdef __MSDOS__
int
CtrlBrkHander(void)
{
	return(1);
}
#endif

//==============================================================================

void
InitInput(void)
{
#ifdef __OS2__
	ki.cb = sizeof( ki );
	keyboardStatus = (unsigned char*)&( ki.fsState );
#endif
#ifdef DOSX286
	unsigned short sel;

	DosGetBIOSSeg( &sel );
	keyboardStatus = (unsigned char far*)MAKEP( sel, 0x17 );
#else
#ifdef __MSDOS__
	keyboardStatus = (unsigned char far*)0x417;
#endif
#endif
	InitMouse();
	inBase.mouseButtons = GetMouse(&inBase.fineMouseX,&inBase.fineMouseY);
	inBase.mouseX = inBase.fineMouseX/8;
	inBase.mouseY = inBase.fineMouseY/8;
	inBase.inputMask = INPF_KEY|INPF_MOUSEMOVE;
	copyBuffer[0] = '\0';                                   // start paste buffer with empty string

#ifdef __MSDOS__
	ctrlbrk(&CtrlBrkHander);
#endif
}

//=============================================================================
// kts new code to re-map input events
//============================================================================
// remaps input events into INP_WINDOW_MESSAGE events
// will operate in place if in == out
// currently only re-maps key events

void
RemapEvent(_input* in, _input* out,_inputRemap* remap)
{

	assert(in);
	assert(out);
	assert(remap);

	boolean doit = boolean::FALSE;
	unsigned int message;

	switch(in->inputType)
	 {
		case INP_KEY:
			{
			_inputKeyRemap* pKey = remap->keyArray;
			unsigned int key = in->fullKey;

			while(!doit && pKey && pKey->oldKey)
			 {
				if(pKey->oldKey == key)
				 {
					message = pKey->newMessage;
					doit = boolean::TRUE;
				 }
				pKey++;							// go to next entry in key array
			 }
			break;
			}
		case INP_COMMAND:
			doit = boolean::FALSE;
			break;
	 }

	if(doit)
	 {
		out->inputType = INP_WINDOW_MESSAGE;
		out->message = message;
	 }
	else
		if(out != in)
			*out = *in;					// just copy struct

}

//============================================================================
