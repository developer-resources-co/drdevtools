//=============================================================================
//	window.hpp: windowing functions using layers
//=============================================================================

#ifndef _window_hpp_
#define _window_hpp_

#include "layer.hpp"
#include "gadget.hpp"

//=============================================================================

class _window : public _shadowedLayer
	{
public:
	_window( int xPos,int yPos,unsigned int xSize,unsigned int ySize, char *title = NULL, unsigned char charAttr = ATTR_NORMAL,char fillChar = ' ', char border[8] = "зд©ЁЁюды");
	~_window();

    _gadget gadgBase;
	_gadget *activeGadget;		// for string gadgets, user supported

	};

void
ZoomWindow(_object *oPtr);

_object *
FindWindow(char *wName);

_layer *
FindNumberedWindow(int num);

void
CopyBorder(char destB[8],char srcB[8]);

void
DrawBorder(_layer *lPtr, char border[8], int xSize, int ySize);

void
DrawBottomBorder(_layer *lPtr, char border[8],int xSize,int ySize);

void
PrintTitle(_window *pWindow,unsigned char attr);

void
FillWindow(_layer *lPtr);

void
FillWindowSize(_layer *lPtr, int ySize2);

void
ResizeWindow(_layer *lPtr,int newX,int newY);

unsigned int
AutoPosition(unsigned int backPos,unsigned int backSize,unsigned int frontSize,unsigned int edge);

unsigned int
AutoPlace(int pos,unsigned int frontSize,unsigned int edge);

unsigned int
AutoCenter(unsigned int size,unsigned int edge);

void
UnHighLightTitle(_object *oPtr);

void
HighLightTitle(_object *oPtr);

void
ForceWindowResize ( _window *pWindow, _object *oPtr,FLAG(*inputRoutine)(_input *,_object *) );

FLAG
MouseInsideWindow(_input *in,_layer *lPtr);

_layer *
GetLayerMouseOver(_input *in);

void
ActivateFrontWindow(void);

//=============================================================================

#endif

//=============================================================================
