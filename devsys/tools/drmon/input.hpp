//=============================================================================
//   input.h: handle i/o
//=============================================================================

#ifndef DRMON_input_H
#define DRMON_input_H

#include "mouse.hpp"

//=============================================================================

enum
{
	CLIPBOARDSIZE = 160
};

enum
{
	INP_NONE,
	INP_KEY,
	INP_MOUSEMOVE,
	INP_MOUSE_LEFTBUTTON_DOWN,
	INP_MOUSE_LEFTBUTTON_UP,
	INP_MOUSE_MIDDLEBUTTON_DOWN,
	INP_MOUSE_MIDDLEBUTTON_UP,
	INP_MOUSE_RIGHTBUTTON_DOWN,
	INP_MOUSE_RIGHTBUTTON_UP,
	INP_RESIZE,
	INP_COMMAND,
	INP_MOUSE_LEFTBUTTON_SHIFTDOWN,
	INP_MOUSE_RIGHTBUTTON_SHIFTDOWN,
	INP_WINDOW_MESSAGE,					// input message specific to window it is sent to
										// (most other messages are re-mapped into
										// window messages
};

enum
{
	MOUSEB_BLEFT = BUTTON_LEFT,
	MOUSEF_BLEFT = 1<<MOUSEB_BLEFT,

	MOUSEB_BRIGHT = BUTTON_RIGHT,
	MOUSEF_BRIGHT = 1<<MOUSEB_BRIGHT,
};

//=============================================================================
// input enables

enum InputType
{
	INPB_KEY,
	INPB_MOUSEMOVE,
	INPB_MOUSEBUTTONS,
	INPB_CASESENSE,
};

const int INPF_KEY = 1<<INPB_KEY;
const int INPF_MOUSEMOVE = 1<<INPB_MOUSEMOVE;
const int INPF_MOUSEBUTTONS = 1<<INPB_MOUSEBUTTONS;
const int INPF_CASESENSE = 1<<INPB_CASESENSE;

//=============================================================================

typedef unsigned int key;

class _input
{
public:
	int inputType;
	int message;
	unsigned int fullKey;
	short mouseX,mouseY,mouseButtons;
	short fineMouseX,fineMouseY;
//	short oldMouseX,oldMouseY,oldMouseButtons;
	int inputMask;
	char **cmdText;
	errorcode *errPtr;

	_input();
};

//=============================================================================

extern _input inBase;
extern FLAG mouseBEnable;
extern char copyBuffer[CLIPBOARDSIZE];					// buffer for cut/copy/paste

struct _layer;
extern char strString[];

//=============================================================================

void
ErasePointer(void);

void
DrawPointer(char far *screen,int mouseX, int mouseY,int screenWidth,int screenHeight);

void
UpdateCursor(void);

unsigned char
GetShiftStatus(void);

unsigned int
GetDirectKey(void);

FLAG
InputPending(_input *in);

void
InitMouse(void);

void
ReSizeMouse(void);

void
InitInput(void);

//============================================================================
// kts new remap stuff

struct _inputKeyRemap
{
	unsigned int oldKey,newMessage;
};

class _inputRemap
{
public:
	_inputRemap() { keyArray = NULL; }
	_inputKeyRemap* keyArray;				// ptr to array of _inputKeyRemaps, oldKey 0 terminated
};

//============================================================================

void
RemapEvent(_input* in, _input* out,_inputRemap* remap);

//=============================================================================
#endif
//=============================================================================
